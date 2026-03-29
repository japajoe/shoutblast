#ifndef SHOUTBLAST_GENRE_HPP
#define SHOUTBLAST_GENRE_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

namespace ShoutBlast
{
    struct Genre
    {
        std::string name;
        uint32_t id;
        uint32_t parentId;
        std::vector<Genre> subGenres;
        Genre();
        Genre(const std::string &name, uint32_t id, uint32_t parentId);
        void AddSubGenre(const std::string &name, uint32_t id);
        static std::vector<Genre> CreateList(const std::unordered_map<uint32_t, Genre> &map);
    };
}

#endif