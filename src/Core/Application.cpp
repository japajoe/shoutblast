#include "Application.hpp"
#include "../Utilities/Dialog.hpp"
#include "../Utilities/File.hpp"
#include "../Utilities/String.hpp"
#include "../Utilities/Time.hpp"
#include "../../libs/imgui/include/imgui.h"
#include "../../libs/glfw/include/GLFW/glfw3.h"
#include <iostream>
#include <thread>

namespace ShoutBlast
{
    Application::Application() : ApplicationBase()
    {
        selectedGenreId = 0;
        selectedStationId = 0;
    }

    Application::Application(const char *title, uint32_t width, uint32_t height, bool vsync) 
        : ApplicationBase(title, width, height, vsync)
    {
        selectedGenreId = 0;
        selectedStationId = 0;
        stationsVisible = false;
    }

    void Application::OnLoad()
    {
        threadPool = std::make_unique<ThreadPool>(4);

        GetGenres();

        stream.onMetadata = [this] (const std::string &md) {
            currentTrack = stream.GetCurrentTrack();
            std::string title = "ShoutBlast - " + currentTrack;
            SetTitle(title.c_str());
        };

        // stream.onConnected = [this] (const std::unordered_map<std::string,std::string> &headers) {
        //     for(const auto& [key,value] : headers)
        //         std::cout << key << ": " << value << '\n';
        // };

        // stream.onDisconnected = [this] () {
        //     std::cout << "Disconnected\n";
        // };

        visualizer.Generate();

        textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
		textEditor.SetShowHorizontalScrollbar(false);
		textEditor.SetShowWhitespaces(false);
        textEditor.SetText("void main() {\n\n}");

		auto palette = textEditor.GetDarkPalette();
		palette[12] = ImColor(ImVec4(0.15f, 0.16f, 0.17f, 1.00f)); //background
		textEditor.SetPalette(palette);

    }

    void Application::OnDestroy()
    {
        stream.Stop();
    }

    void Application::OnUpdate()
    {
        if(genreQueue.GetSize() > 0)
        {
            if(genreQueue.TryDequeue(genres))
            {
            }
        }

        if(stationQueue.GetSize() > 0)
        {
            if(stationQueue.TryDequeue(stations))
            {
            }
        }

        if(urlQueue.GetSize() > 0)
        {
            if(urlQueue.TryDequeue(currentUrl))
            {
                stream.Play(currentUrl);
            }
        }

        visualizer.OnUpdate(stream.GetOutputBuffer());

        static float timer = 0.0;

        if(timer >= 30.0f)
        {
            if(stationsVisible && selectedStationId > 0)
                GetStations(selectedGenreId);
            timer -= 30.0f;
        }
        else
        {
            timer += Time::GetDeltaTime();
        }
    }

    void Application::OnGUI()
    {
        ShowMenu();
        ShowGenres();
        ShowPlayer();
        ShowStations();
        ShowVisualizer();
        ShowCodeEditor();
    }

    void Application::ShowMenu()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open Network Stream..."))
                {

                }

                if (ImGui::MenuItem("Open Shader...", "Ctrl+O"))
                {
                    ShowOpenFileDialog();
                }

                if (ImGui::MenuItem("Save Shader As...", "Ctrl+S"))
                {
                    ShowSaveFileDialog();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit", "Alt+F4"))
                {

                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGuiIO &io = ImGui::GetIO();

        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_O))
        {
            ShowOpenFileDialog();
        }

        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
        {
            ShowSaveFileDialog();
        }
    }

    void Application::ShowPlayer()
    {
        float volume = stream.GetVolume();
        char time[16];

        stream.GetPlaybackTime(time, 16);

        ImGui::Begin("Player");
        if(currentStation.size() > 0)
            ImGui::Text(currentStation.c_str());
        ImGui::Text(currentTrack.c_str());
        ImGui::SetNextItemWidth(100);
        if(ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f))
            stream.SetVolume(volume);
        ImGui::Text(time);
        ImGui::End();
    }

    void Application::ShowGenres()
    {
        uint32_t currentGenre = selectedGenreId;
        
        ImGui::Begin("Genres");
        if(genres.size() > 0)
            DrawGenres(genres, selectedGenreId);
        ImGui::End();

        if(currentGenre != selectedGenreId)
        {
            currentGenre = selectedGenreId;
            GetStations(selectedGenreId);
        }
    }

    void Application::ShowStations()
    {
        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
                                    ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;

        bool visible = false;

        ImGui::Begin("Stations");

        if (stations.size() > 0)
        {
            if (ImGui::BeginTable("StationTable", 4, flags, ImVec2(0, 0)))
            {
                visible = true;

                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Genre", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Bitrate", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableSetupColumn("Listeners", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableHeadersRow();

                int i = 0;
                for (auto& station : stations)
                {
                    // 1. Push a unique ID for this specific row based on loop index
                    ImGui::PushID(i); 

                    ImGui::TableNextRow();
                    
                    ImGui::TableSetColumnIndex(0);
                    
                    // We still compare against station.ID for your logic, 
                    // but ImGui now sees this Selectable as unique due to PushID
                    bool isSelected = (selectedStationId == station.ID);
                    
                    if (ImGui::Selectable(station.Name.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        selectedStationId = station.ID;
                    }

                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        currentStation = station.Name;
                        GetStationUrl(selectedStationId);
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(station.Genre.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d", station.Bitrate);

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%d", station.Listeners);

                    // 2. Pop the ID to clean up the stack for the next iteration
                    ImGui::PopID();
                    i++;
                }
                
                ImGui::EndTable();
            }
        }
        
        ImGui::End();

        stationsVisible = visible;
    }

    void Application::ShowVisualizer()
    {
        visualizer.OnGUI();
    }

    void Application::ShowCodeEditor()
    {
        ImGui::Begin("Code");
        float footerHeight = ImGui::GetFrameHeightWithSpacing();
        textEditor.Render("Editor", ImVec2(0, -footerHeight));

        ImGui::Spacing();

        if (ImGui::Button("Compile")) 
        { 
            std::string code = textEditor.GetText();

            if(!code.empty())
            {
                if(!visualizer.CompileShader(code, currentError))
                {

                }
            }

        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Log")) 
        { 
            currentError.clear();
        }

        ImGui::End();

        ImGui::Begin("Log");
        ImGui::TextUnformatted(currentError.c_str());
        ImGui::End();
    }

    void Application::DrawGenres(const std::vector<Genre>& genres, uint32_t &selectedGenreId)
    {
        for (const auto& genre : genres)
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

            // Apply selection highlighting
            if (selectedGenreId == genre.id)
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            if (genre.subGenres.empty())
            {
                flags |= ImGuiTreeNodeFlags_Leaf;
            }

            // Use the ID as the unique identifier for ImGui's internal state
            bool isOpen = ImGui::TreeNodeEx((void*)(intptr_t)genre.id, flags, "%s", genre.name.c_str());

            // Update selection when the label is clicked
            if (ImGui::IsItemClicked())
            {
                selectedGenreId = genre.id;
                // You can also trigger your station loading here
            }

            if (isOpen)
            {
                if (!genre.subGenres.empty())
                {
                    // Pass the selection reference down the stack
                    DrawGenres(genre.subGenres, selectedGenreId);
                }
                ImGui::TreePop();
            }
        }
    }

    void Application::GetGenres()
    {
        threadPool->Enqueue([this] () {
            try 
            {
                std::vector<Genre> genres = shoutcast.GetGenres();
                if(genres.size() > 0)
                {
                    genreQueue.Enqueue(genres);
                }
            }
            catch (const std::exception& e) 
            {
            }
        });
    }

    void Application::GetStations(uint32_t genreId)
    {
        if(genres.size() == 0)
            return;

        auto genre = FindGenreById(genres, genreId);

        if(!genre)
            return;

        std::string name = genre->name;

        threadPool->Enqueue([this, name]() {
            try 
            {
                std::vector<Station> stations = shoutcast.GetStationsByGenre(name);
                if(stations.size() > 0)
                    this->stationQueue.Enqueue(stations);
            }
            catch (const std::exception& e) 
            {
            }
        });
    }

    void Application::GetStationUrl(uint64_t stationId)
    {
        threadPool->Enqueue([this, stationId]() {
            try 
            {
                auto url = shoutcast.GetStationUrl(stationId);
                urlQueue.Enqueue(url);
            }
            catch (const std::exception& e) 
            {
            }
        });
    }

    Genre* Application::FindGenreById(std::vector<Genre>& genres, uint32_t id)
    {
        for (auto& genre : genres)
        {
            // Check if this is the genre we are looking for
            if (genre.id == id)
            {
                return &genre;
            }

            // If not, recursively search through this genre's sub-genres
            if (!genre.subGenres.empty())
            {
                Genre* found = FindGenreById(genre.subGenres, id);
                
                // If the recursive call found a match, return it up the stack
                if (found != nullptr)
                {
                    return found;
                }
            }
        }

        // If the loop finishes without a match at this level or below, return nullptr
        return nullptr;
    }

    void Application::ShowOpenFileDialog()
    {
        static std::string initialDirectory = "./";

        OpenFileDialog dialog;
        dialog.SetInitialDirectory(initialDirectory);
        
        if (dialog.ShowDialog() == DialogResult::OK)
        {
            std::string selectedPath = dialog.GetFilePath();
            if(File::Exists(selectedPath))
            {
                std::string code = File::ReadAllText(selectedPath);
                textEditor.SetText(code);
                initialDirectory = File::GetDirectoryPath(selectedPath) + "/";
            }
        }
    }

    void Application::ShowSaveFileDialog()
    {
        static std::string initialDirectory = "./";

        SaveFileDialog dialog;
        dialog.SetInitialDirectory(initialDirectory);
        if (dialog.ShowDialog() == DialogResult::OK)
        {
            std::string selectedPath = dialog.GetFilePath();
            
            if(!String::EndsWith(selectedPath, ".glsl"))
                selectedPath += ".glsl";

            std::string code = textEditor.GetText();
            File::WriteAllText(selectedPath, code);
            initialDirectory = File::GetDirectoryPath(selectedPath) + "/";
        }
    }

}