#ifndef SHOUTBLAST_APPLICASTION_HPP
#define SHOUTBLAST_APPLICASTION_HPP

#include "ApplicationBase.hpp"
#include "../Audio/ShoutCast.hpp"
#include "../Audio/Genre.hpp"
#include "../Audio/AudioStream.hpp"
#include "../Utilities/ConcurrentQueue.hpp"
#include "../Utilities/ThreadPool.hpp"
#include "../Graphics/Visualizer.hpp"
#include "../../libs/imgui/include/TextEditor.h"
#include <memory>
#include <atomic>

namespace ShoutBlast
{
    class Application : public ApplicationBase
    {
    public:
        Application();
        Application(const char *title, uint32_t width, uint32_t height, bool vsync = true);
        void OnLoad() override;
        void OnDestroy() override;
        void OnUpdate() override;
        void OnGUI() override;
    private:
        std::unique_ptr<ThreadPool> threadPool;
        AudioStream stream;
        ShoutCast shoutcast;
        Visualizer visualizer;
        ConcurrentQueue<std::vector<Genre>> genreQueue;
        ConcurrentQueue<std::vector<Station>> stationQueue;
        ConcurrentQueue<std::string> urlQueue;
        std::vector<Genre> genres;
        std::vector<Station> stations;
        std::string currentTrack;
        std::string currentUrl;
        std::string currentError;
        uint32_t selectedGenreId;
        uint32_t selectedStationId;
        std::string currentStation;
        bool stationsVisible;
        TextEditor textEditor;
        void ShowMenu();
        void ShowPlayer();
        void ShowGenres();
        void ShowStations();
        void ShowVisualizer();
        void ShowCodeEditor();
        void DrawGenres(const std::vector<Genre> &genres, uint32_t &selectedGenreId);
        void GetGenres();
        void GetStations(uint32_t genreId);
        void GetStationUrl(uint64_t stationId);
        Genre* FindGenreById(std::vector<Genre> &genres, uint32_t id);
        void ShowOpenFileDialog();
        void ShowSaveFileDialog();
    };
}

#endif