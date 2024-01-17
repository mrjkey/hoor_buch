// src/main.cpp

#include <iostream>
#include <boost/asio.hpp>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include <GL/gl.h>
#include <SFML/Audio.hpp>
#include <fstream>
#include <filesystem>
#include <string>
#include "ImGuiFileDialog.h"
#include <tuple>

#include "gui.h"
#include "connection.h"
#include "BookProgress.h"
#include "AudioBookPlayer.h"
#include "LibraryManager.h"
// #include <portaudio.h>

using boost::asio::ip::tcp;
namespace asio = boost::asio;

void setup_for_main(AudioBookPlayer *player)
{
    std::cout << "Starting Client Application" << std::endl;

    // check if a yaml file exists for the libarary
    // if not, create one
    if (std::filesystem::exists("config.yaml"))
    {
        YAML::Node config = YAML::LoadFile("config.yaml");

        if (!config["audiobooks_directory"])
        {
            std::cout << "no directory in yaml file, adding one" << std::endl;
            // Update the audiobooks_directory value

            config["audiobooks_directory"] = ".";
            std::ofstream fout("config.yaml");
            fout << config;
            fout.close();
        }
    }
    else
    {
        // create a yaml file
        YAML::Node config;
        config["audiobooks_directory"] = ".";
        std::ofstream fout("config.yaml");
        fout << config;
        fout.close();
    }

    player->loadLibrary("config.yaml");
}

void gui_choose_library_dialog(AudioBookPlayer *player)
{
    // Open Dialog Simple
    if (ImGui::Button("Select a library directory"))
    {
        // Here we set up the file dialog before opening the popup
        YAML::Node config;
        if (std::filesystem::exists("config.yaml"))
        {
            config = YAML::LoadFile("config.yaml");
        }
        else
        {
            std::cerr << "config.yaml not found, defaulting to current directory." << std::endl;
            config["audiobooks_directory"] = ".";
        }

        std::string initialPath = config["audiobooks_directory"] ? config["audiobooks_directory"].as<std::string>() : ".";
        IGFD::FileDialogConfig fileDialogConfig;
        fileDialogConfig.path = initialPath;

        ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose Directory", nullptr, fileDialogConfig);
        // ImGui::OpenPopup("Choose Library Folder");
    }

    // Always center this window when appearing
    // ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    // ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    // Set the size of the popup before it opens
    ImVec2 popupSize = ImVec2(600, 400);                   // Set the size you desire for the popup
    ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always); // Use ImGuiCond_Always or ImGuiCond_FirstUseEver

    // Modal window for file dialog
    // if (ImGui::BeginPopupModal("Choose Library Folder", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    // {
    //     ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey");
    if (ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey"))
    {

        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string directoryPath = ImGuiFileDialog::Instance()->GetCurrentPath();

            // Convert to an absolute path
            std::filesystem::path absPath = std::filesystem::absolute(directoryPath);

            // If you need it as a string
            std::string absolutePathString = absPath.string();

            // Load the YAML file
            YAML::Node config = YAML::LoadFile("config.yaml");

            std::cout << absolutePathString << std::endl;
            // Update the audiobooks_directory value

            config["audiobooks_directory"] = absolutePathString;

            // Write the file back
            std::ofstream fout("config.yaml");
            fout << config;
            fout.close();
            player->loadLibrary("config.yaml");
            // action

            // close
            ImGuiFileDialog::Instance()->Close();
        }

        // ImGui::EndPopup();
    }
}

void gui_new_frame()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void gui_audio_book_player_window(sf::Music *music, AudioBookPlayer *player)
{
    // Fill the entire window with the Audio Book Player UI
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImVec2 viewportSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(viewportSize);

    if (ImGui::Begin("Audio Book Player", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
    {
        // Display the playback buttons
        gui_playback_buttons(player);

        // volume slider
        float volume = music->getVolume();
        if (ImGui::SliderFloat("Volume", &volume, 0.0f, 100.0f))
        {
            music->setVolume(volume);
        }

        gui_choose_library_dialog(player);

        // Display the library
        DisplayLibrary(player);
    }
    ImGui::End();
}

void gui_render_frame(GLFWwindow *window)
{
    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void execute_periodic_functions(AudioBookPlayer *player)
{
    const float updateInterval_object = 0.5f; // Update every 500 milliseconds
    const float updateInterval_file = 3.0f;   // Update every 3 seconds
    static float elapsedTime_object = 0.0f;   // Time elapsed since the last update
    static float elapsedTime_file = 0.0f;     // Time elapsed since the last update

    // Execute periodic functions
    static float lastFrameTime = 0.0f;
    float currentFrameTime = glfwGetTime();
    float deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;

    // Update logic
    if (player->is_playing) // Implement this method to check if the audiobook is playing
    {
        elapsedTime_object += deltaTime;
        elapsedTime_file += deltaTime;
        if (elapsedTime_object >= updateInterval_object)
        {
            // Update last played position
            player->currentBook->last_played_position = player->music.getPlayingOffset().asSeconds();

            // Reset elapsed time
            elapsedTime_object = 0.0f;
        }
        if (elapsedTime_file >= updateInterval_file)
        {
            // Update the audiobook info file
            CreateOrUpdateAudiobookInfo(player->currentBook->path, *player->currentBook);

            // Reset elapsed time
            elapsedTime_file = 0.0f;
        }
        // if the audio file has ended and the next file exists, play the next file
        if (player->music.getStatus() == sf::SoundSource::Status::Stopped && player->currentBook->files.size() > 1)
        {
            // get the index of the current file
            int currentFileIndex = std::distance(player->currentBook->files.begin(), std::find(player->currentBook->files.begin(), player->currentBook->files.end(), player->currentBook->last_played_file));

            // if the current file is not the last file in the list
            if (currentFileIndex != player->currentBook->files.size() - 1)
            {
                // play the next file
                player->music.openFromFile(player->currentBook->path + "/" + player->currentBook->files[currentFileIndex + 1]);
                player->music.play();
                player->currentBook->last_played_file = player->currentBook->files[currentFileIndex + 1];
                player->currentBook->last_played_position = 0;
            }
        }
    }
}

void main_loop(GLFWwindow *window, AudioBookPlayer *player)
{
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        // Start the Dear ImGui frame
        gui_new_frame();

        // Fill the entire window with the Audio Book Player UI
        gui_audio_book_player_window(&player->music, player);

        // Rendering
        gui_render_frame(window);

        execute_periodic_functions(player);
    }
}

void shutdown(GLFWwindow *window)
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int argc, char **argv)
{
    GLFWwindow *window;
    AudioBookPlayer player;

    gui_setup(&window);
    setup_for_main(&player);
    remove_duplicate_audiobooks(&player.library);

    player.selectBook(0);

    // Main loop
    main_loop(window, &player);

    // Cleanup
    shutdown(window);

    return 0;
}