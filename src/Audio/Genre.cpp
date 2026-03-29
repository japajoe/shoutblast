#include "Genre.hpp"
#include <iostream>
#include <algorithm>

namespace ShoutBlast
{
    Genre::Genre()
    {
        this->id = 0;
        this->parentId = 0;
    }

    Genre::Genre(const std::string &name, uint32_t id, uint32_t parentId)
    {
        this->name = name;
        this->id = id;
        this->parentId = parentId;
    }

    void Genre::AddSubGenre(const std::string &name, uint32_t id)
    {
        subGenres.emplace_back(name, id, this->id);
    }
    
    std::vector<Genre> Genre::CreateList(const std::unordered_map<uint32_t, Genre> &map)
    {
        std::vector<Genre> rootGenres;
        rootGenres.reserve(100); //Only need 25

        for(auto& [key,value] : map)
        {
            //auto genre = list.AddRootGenre(value.name, key);
            auto &genre = rootGenres.emplace_back(value.name, key, 0);

            for(auto &s : value.subGenres)
            {
                genre.AddSubGenre(s.name, s.id);
            }
        }

        std::sort(rootGenres.begin(), rootGenres.end(), [](const Genre& a, const Genre& b) {
            return a.id < b.id;
        });

        return rootGenres;
    }
}