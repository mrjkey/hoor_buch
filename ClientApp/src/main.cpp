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

void setup_for_main(GLFWwindow **window, sf::Music *music, AudioBookPlayer *player)
{
    std::cout << "Starting Client Application" << std::endl;

    // initialize glfw
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Create a windowed mode window and its OpenGL context
    *window = glfwCreateWindow(900, 600, "Audio Books Baby!", NULL, NULL);
    if (!*window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Make the window's context current
    glfwMakeContextCurrent(*window);

    // Initialize ImGui and its bindings for OpenGL3 and GLFW
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(*window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // Replace with your OpenGL version if needed

    // After creating the ImGui context, apply the new style
    SetModernImGuiStyle();

    // GLuint albumArtTexture = LoadTexture("path_to_album_art.png"); // Implement this function to load texture

    // DisplayNavigationPanel();
    // DisplayAlbumArt(albumArtTexture);

    // Load a music to play
    if (!music->openFromFile("D:\\Torrents\\Books\\The Rook\\The Rook-Part08.mp3"))
    {
        std::cerr << "Failed to load music" << std::endl;
        throw std::runtime_error("Failed to load music");
    }

    // check if a yaml file exists for the libarary
    // if not, create one
    if (std::filesystem::exists("config.yaml"))
    {
        YAML::Node config = YAML::LoadFile("config.yaml");

        if (!config["audiobooks_directory"])
        {
            std::cout << "no directory in yaml file, adding one" << std::endl;
            // Update the audiobooks_directory value

            config["audiobooks_directory"] = "C:\\Users";
            std::ofstream fout("config.yaml");
            fout << config;
            fout.close();
        }
    }
    else
    {
        // create a yaml file
        YAML::Node config;
        config["audiobooks_directory"] = "D:\\Torrents\\Books";
        std::ofstream fout("config.yaml");
        fout << config;
        fout.close();
    }

    player->loadLibrary("config.yaml");

    // std::string audiobookDirectory = "D:\\Torrents\\Books\\The Rook";
}

void gui_playback_buttons(sf::Music *music)
{
    if (ImGui::Button("Play"))
    {
        std::cout << "Play" << std::endl;
        // Trigger audio playback
        music->play();
    }

    ImGui::SameLine();

    if (ImGui::Button("Pause"))
    {
        std::cout << "Pause" << std::endl;
        // Pause the audio
        music->pause();
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop"))
    {
        std::cout << "Stop" << std::endl;
        // Stop the audio because
        music->stop();
    }
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

        gui_playback_buttons(music);

        // Display progress bar for the audiobook
        ImGui::ProgressBar(0.5f, ImVec2(-FLT_MIN, 0.0f), "Progress");

        gui_choose_library_dialog(player);

        // Display the library
        DisplayLibrary(player, *music);

        // Display the book progress
        // DisplayBookProgress();
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

void main_loop(GLFWwindow *window, sf::Music *music, AudioBookPlayer *player)
{
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        // Start the Dear ImGui frame
        gui_new_frame();

        // Fill the entire window with the Audio Book Player UI
        gui_audio_book_player_window(music, player);

        // Rendering
        gui_render_frame(window);
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
    sf::Music music;
    AudioBookPlayer player;
    std::vector<Audiobook> audiobooks;
    setup_for_main(&window, &music, &player);

    // Main loop
    main_loop(window, &music, &player);

    // Cleanup
    shutdown(window);

    return 0;
}