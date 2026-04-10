#include "ShoutCast.hpp"
#include "../Utilities/String.hpp"
#include "../../libs/kazuho/picojson.hpp"
#include <iostream>
#include <cstring>
#include <regex>

namespace ShoutBlast
{
    std::vector<Station> Station::Deserialize(const std::string &json)
    {
        picojson::value v;
        std::string err = picojson::parse(v, json);

        if (!err.empty()) 
        {
            std::cerr << "Error: " << err << std::endl;
            return {};
        }

        if (!v.is<picojson::array>()) 
        {
            std::cerr << "JSON is not an array" << std::endl;
            return {};
        }

        const picojson::array& list = v.get<picojson::array>();
        std::vector<Station> stations;

        for (const auto& item : list) 
        {
            if (item.is<picojson::object>()) 
            {
                const auto& obj = item.get<picojson::object>();
                Station s;

                s.ID = static_cast<int64_t>(obj.at("ID").get<double>());
                s.Name = obj.at("Name").get<std::string>();
                s.Format = obj.at("Format").get<std::string>();
                s.Bitrate = static_cast<int32_t>(obj.at("Bitrate").get<double>());
                s.Genre = obj.at("Genre").get<std::string>();
                s.CurrentTrack = obj.at("CurrentTrack").get<std::string>();
                s.Listeners = static_cast<int32_t>(obj.at("Listeners").get<double>());
                s.IsRadionomy = obj.at("IsRadionomy").get<bool>();
                s.IceUrl = obj.at("IceUrl").get<std::string>();
                
                if (obj.at("StreamUrl").is<picojson::null>()) 
                    s.StreamUrl = ""; 
                else 
                    s.StreamUrl = obj.at("StreamUrl").get<std::string>();

                s.AACEnabled = static_cast<int>(obj.at("AACEnabled").get<double>());
                s.IsPlaying = obj.at("IsPlaying").get<bool>();
                s.IsAACEnabled = obj.at("IsAACEnabled").get<bool>();

                stations.push_back(s);
            }
        }

        return stations;
    }

    ShoutCast::ShoutCast()
    {
        userAgent = "Mozilla/5.0 (X11; Linux x86_64; rv:143.0) Gecko/20100101 Firefox/143.0";
        cookie = "wt5bk7p1onnzcpypte9r2vza";
    }

    std::vector<Genre> ShoutCast::GetGenres()
    {
        std::string url = "https://directory.shoutcast.com";

        HttpRequest request(HttpMethod::Get, url);

        request.AddHeader("User-Agent", userAgent);
        request.AddHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
        request.AddHeader("Accept-Language", "en-US,en;q=0.5");
        request.AddHeader("Accept-Encoding", "gzip, deflate, br, zstd");
        request.AddHeader("Sec-GPC", "1");
        request.AddHeader("Connection", "keep-alive");
        request.AddHeader("Upgrade-Insecure-Requests", "1");
        request.AddHeader("Sec-Fetch-Dest", "document");
        request.AddHeader("Sec-Fetch-Mode", "navigate");
        request.AddHeader("Sec-Fetch-Site", "none");
        request.AddHeader("Sec-Fetch-User", "?1");
        request.AddHeader("Priority", "u=0, i");

        std::shared_ptr<HttpResponse> response = client.Send(request);

        if(response->GetStatus() != HttpStatusCode::Ok)
            return {};

        if(response->GetContentLength() == 0)
            return {};

        std::string content;

        if(!response->GetContentAsString(content))
            return {};
        
        // Regex Breakdown:
        // loadStationsByGenre : Matches function name
        // \(                  : Matches opening parenthesis
        // '                   : Matches opening single quote
        // ([^']*)             : Capture Group 1: Matches any char except a quote (the genre name)
        // '                   : Matches closing single quote
        // \s*,\s* : Matches comma with optional surrounding whitespace
        // ([0-9]+)            : Capture Group 2: Matches one or more digits (genreId)
        // \s*,\s* : Matches second comma with optional whitespace
        // ([0-9]+)            : Capture Group 3: Matches one or more digits (parentId)
        // \)                  : Matches closing parenthesis
        std::regex pattern("loadStationsByGenre\\('([^']*)'\\s*,\\s*([0-9]+)\\s*,\\s*([0-9]+)\\)");

        auto wordsBegin = std::sregex_iterator(content.begin(), content.end(), pattern);
        auto wordsEnd = std::sregex_iterator();
        
        std::unordered_map<uint32_t, Genre> genres;
                
        std::vector<Genre> matches;

        for (std::sregex_iterator i = wordsBegin; i != wordsEnd; ++i)
        {
            std::smatch match = *i;

            std::string m = match.str();

            if(String::Contains(m, "&amp;", false))
            {
                m = String::Replace(m, "&amp;", "&");
            }

            m = String::Replace(m, "loadStationsByGenre", "");
            m = String::Replace(m, "(", "");
            m = String::Replace(m, ")", "");
            m = String::Replace(m, "'", "");

            auto parameters = String::Split(m, ',', 3);

            if(parameters.size() == 3)
            {
                parameters[1] = String::Trim(parameters[1]);
                parameters[2] = String::Trim(parameters[2]);
            }
            
            std::string name = parameters[0];
            uint32_t id = 0;
            uint32_t parentId = 0;
            
            if(!String::TryParseUInt32(parameters[1], id))
                return {};
            
            if(!String::TryParseUInt32(parameters[2], parentId))
                return {};

            matches.push_back({name, id, parentId});
        }

        for(auto &m : matches)
        {
            if(m.parentId == 0)
            {
                genres[m.id] = {m.name, m.id, m.parentId};
            }
        }

        for(auto &m : matches)
        {
            if(m.parentId != 0)
            {
                if(!genres.contains(m.parentId))
                    return {};
                genres[m.parentId].subGenres.push_back({m.name, m.id, m.parentId});
            }
        }

        
        auto rootGenres = Genre::CreateList(genres);

        auto &cookies = response->Getcookies();

        if(cookies.size() > 0)
        {
            std::string key = "ASP.NET_SessionId=";
            
            size_t startPos = cookies[0].find(key);
            
            if (startPos == std::string::npos) 
                return {};
            
            startPos += key.length();

            size_t endPos = cookies[0].find(';', startPos);

            if (endPos == std::string::npos)
                cookie = cookies[0].substr(startPos);
            else
                cookie = cookies[0].substr(startPos, endPos - startPos);
        }

        return rootGenres;
    }

    std::vector<Station> ShoutCast::GetStationsByGenre(const std::string &genre)
    {
        std::string url = "https://directory.shoutcast.com/Home/BrowseByGenre";

        HttpRequest request(HttpMethod::Post, url);

        SetHeadersForPostRequest(&request);

        std::string formData = "genrename=" + String::UrlEncode(genre);
        MemoryStream contentStream(formData.data(), formData.size(), false);

        request.SetContent(&contentStream, "application/x-www-form-urlencoded; charset=UTF-8");

        std::shared_ptr<HttpResponse> response = client.Send(request);
        
        if(response->GetStatus() == HttpStatusCode::Ok)
        {
            std::string content;

            if(response->GetContentAsString(content))
            {
                auto stations = Station::Deserialize(content);
                std::vector<Station> filteredStations;
                for(auto &station : stations)
                {
                    if(!String::Contains(station.Format, "audio/mpeg", true))
                        continue;
                    filteredStations.push_back(station);
                }
                return filteredStations;
            }
        }

        return {};
    }

    std::string ShoutCast::GetStationUrl(uint64_t id)
    {
        std::string url = "https://directory.shoutcast.com/Player/GetStreamUrl";

        HttpRequest request(HttpMethod::Post, url);

        SetHeadersForPostRequest(&request);

        std::string formData = "station=" + std::to_string(id);
        MemoryStream contentStream(formData.data(), formData.size(), false);

        request.SetContent(&contentStream, "application/x-www-form-urlencoded; charset=UTF-8");

        std::shared_ptr<HttpResponse> response = client.Send(request);
        
        if(response->GetStatus() == HttpStatusCode::Ok)
        {
            std::string content;

            if(response->GetContentAsString(content))
            {
                if(String::Contains(content, "\""))
                {
                    content = String::Replace(content, "\"", "");
                }

                return content;
            }
        }

        return {};
    }

    void ShoutCast::SetHeadersForPostRequest(HttpRequest *request)
    {
        request->AddHeader("User-Agent", userAgent);
        request->AddHeader("Accept", "*/*");
        request->AddHeader("Accept-Language", "en-US,en;q=0.5");
        request->AddHeader("Accept-Encoding", "gzip, deflate, br, zstd");
        request->AddHeader("X-Requested-With", "XMLHttpRequest");
        request->AddHeader("Origin", " https://directory.shoutcast.com");
        request->AddHeader("Sec-GPC", "1");
        request->AddHeader("Connection", "keep-alive");
        request->AddHeader("Referer", "https://directory.shoutcast.com/");
        request->AddHeader("Cookie",  cookie);
        request->AddHeader("Sec-Fetch-Dest", "empty");
        request->AddHeader("Sec-Fetch-Mode", "cors");
        request->AddHeader("Sec-Fetch-Site", "same-origin");
        request->AddHeader("Priority", "u=0");
    }
}