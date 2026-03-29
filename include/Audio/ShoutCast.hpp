#ifndef SHOUTBLAST_SHOUTCAST_HPP
#define SHOUTBLAST_SHOUTCAST_HPP

#include "../Utilities/HttpClient.hpp"
#include "Genre.hpp"
#include <string>
#include <cstdint>
#include <vector>

namespace ShoutBlast
{
    struct Station
    {
        int64_t ID;
        std::string Name;
        std::string Format;
        int32_t Bitrate;
        std::string Genre;
        std::string CurrentTrack;
        int32_t Listeners;
        bool IsRadionomy;
        std::string IceUrl;
        std::string StreamUrl;
        int AACEnabled;
        bool IsPlaying;
        bool IsAACEnabled;
        static std::vector<Station> Deserialize(const std::string &json);
    };

    class ShoutCast
    {
    public:
        ShoutCast();
        std::vector<Genre> GetGenres();
        std::vector<Station> GetStationsByGenre(const std::string &genre);
        std::string GetStationUrl(uint64_t id);
    private:
        HttpClient client;
        std::string userAgent;
        std::string cookie;
        void SetHeadersForPostRequest(HttpRequest *request);
    };
}

#endif