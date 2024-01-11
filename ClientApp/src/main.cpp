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

#include "math_utils.h"
#include "connection.h"
#include "BookProgress.h"
#include "AudioBookPlayer.h"

// #include <portaudio.h>

using boost::asio::ip::tcp;
namespace asio = boost::asio;

// Set ImGui style to a more sleek/modern appearance
void SetModernImGuiStyle()
{
    ImGuiStyle &style = ImGui::GetStyle();

    // Spacing
    style.WindowPadding = ImVec2(15, 15);
    style.WindowRounding = 5.0f;
    style.FramePadding = ImVec2(5, 5);
    style.FrameRounding = 4.0f;
    style.ItemSpacing = ImVec2(12, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.IndentSpacing = 25.0f;
    style.ScrollbarSize = 15.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 5.0f;
    style.GrabRounding = 3.0f;

    // Colors
    ImVec4 *colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    // ... and more as needed

    // Fonts
    // Here you can load and set the font to match Material Design fonts (Roboto is a common choice)
    // ImGui::GetIO().Fonts->AddFontFromFileTTF("path_to_font.ttf", size_pixels);
    // ImGui::GetIO().Fonts->Build();

    // Apply the style
    ImGui::GetStyle() = style;
}

void SetDarkStyle()
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.24f, 0.29f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    // ... other colors

    style.FrameRounding = 4.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    // ... other style settings
}

struct Audiobook
{
    std::string title;
    std::string path;
    float progress;
    std::vector<std::string> files;
    std::string last_played_file;
    int last_played_position;
};

// Forward declarations
void CreateOrUpdateLibraryIndex(const std::string &libraryPath, const std::vector<Audiobook> &audiobooks);
std::vector<Audiobook> ReadLibraryIndex(const std::string &libraryPath);
void CreateOrUpdateAudiobookInfo(const std::string &audiobookPath, const Audiobook &audiobook);
Audiobook ReadAudiobookInfo(const std::string &audiobookPath);

// Function implementations
void CreateOrUpdateLibraryIndex(const std::string &libraryPath, const std::vector<Audiobook> &audiobooks)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "audiobooks" << YAML::Value << YAML::BeginSeq;
    for (const auto &book : audiobooks)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "title" << YAML::Value << book.title;
        out << YAML::Key << "path" << YAML::Value << book.path;
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream fout(libraryPath);
    fout << out.c_str();
}

std::vector<Audiobook> ReadLibraryIndex(const std::string &libraryPath)
{
    std::vector<Audiobook> audiobooks;
    YAML::Node config = YAML::LoadFile(libraryPath);
    for (const auto &book : config["audiobooks"])
    {
        Audiobook audiobook;
        audiobook.title = book["title"].as<std::string>();
        audiobook.path = book["path"].as<std::string>();
        audiobooks.push_back(audiobook);
    }
    return audiobooks;
}

void CreateOrUpdateAudiobookInfo(const std::string &audiobookPath, const Audiobook &audiobook)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "title" << YAML::Value << audiobook.title;
    out << YAML::Key << "progress" << YAML::Value << audiobook.progress;
    out << YAML::Key << "files" << YAML::Value << YAML::Flow << audiobook.files;
    out << YAML::Key << "last_played_file" << YAML::Value << audiobook.last_played_file;
    out << YAML::Key << "last_played_position" << YAML::Value << audiobook.last_played_position;
    out << YAML::EndMap;

    std::ofstream fout(audiobookPath + "/audiobook_info.yaml");
    fout << out.c_str();
}

Audiobook ReadAudiobookInfo(const std::string &audiobookPath)
{
    Audiobook audiobook;
    YAML::Node config = YAML::LoadFile(audiobookPath + "/audiobook_info.yaml");
    audiobook.title = config["title"].as<std::string>();
    audiobook.progress = config["progress"].as<float>();
    audiobook.last_played_file = config["last_played_file"].as<std::string>();
    audiobook.last_played_position = config["last_played_position"].as<int>();
    for (const auto &file : config["files"])
    {
        audiobook.files.push_back(file.as<std::string>());
    }
    return audiobook;
}

int main(int argc, char **argv)
{

    std::string libraryPath = "D:\\Torrents\\Books\\library.yaml";
    std::string audiobookDirectory = "D:\\Torrents\\Books\\The Rook";

    // Read and create/update library index
    std::vector<Audiobook> audiobooks = ReadLibraryIndex(libraryPath);
    // ... modify audiobooks as needed
    CreateOrUpdateLibraryIndex(libraryPath, audiobooks);
    // Read and create/update individual audiobook info
    Audiobook audiobook = ReadAudiobookInfo(audiobookDirectory);
    // ... modify audiobook as needed
    CreateOrUpdateAudiobookInfo(audiobookDirectory, audiobook);

    std::cout << "Starting Client Application" << std::endl;

    // initialize glfw
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window = glfwCreateWindow(900, 600, "Audio Books Baby!", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize ImGui and its bindings for OpenGL3 and GLFW
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // Replace with your OpenGL version if needed

    // After creating the ImGui context, apply the new style
    SetModernImGuiStyle();

    // Load a music to play
    sf::Music music;
    if (!music.openFromFile("D:\\Torrents\\Books\\The Rook\\The Rook-Part08.mp3"))
        return -1; // error
    // music.play();
    AudioBookPlayer player;

    // check if a yaml file exists for the libarary
    // if not, create one
    if (std::filesystem::exists("library.yaml"))
    {
        YAML::Node config = YAML::LoadFile("library.yaml");

        if (!config["audiobooks_directory"])
        {
            std::cout << "no directory in yaml file, adding one" << std::endl;
            // Update the audiobooks_directory value

            config["audiobooks_directory"] = "C:\\Users";
            std::ofstream fout("library.yaml");
            fout << config;
            fout.close();
        }
    }
    else
    {
        // create a yaml file
        YAML::Node config;
        config["audiobooks_directory"] = "D:\\Torrents\\Books";
        std::ofstream fout("library.yaml");
        fout << config;
        fout.close();
    }

    player.loadLibrary("library.yaml");

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Fill the entire window with the Audio Book Player UI
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImVec2 viewportSize = ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowSize(viewportSize);
        ImGui::Begin("Audio Book Player - Hoor Buch", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        if (ImGui::Button("Play"))
        {
            std::cout << "Play" << std::endl;
            // Trigger audio playback
            music.play();
        }

        ImGui::SameLine();

        if (ImGui::Button("Pause"))
        {
            std::cout << "Pause" << std::endl;
            // Pause the audio
            music.pause();
        }

        ImGui::SameLine();

        if (ImGui::Button("Stop"))
        {
            std::cout << "Stop" << std::endl;
            // Stop the audio because
            music.stop();
        }

        // Open Dialog Simple
        if (ImGui::Button("Open Folder Dialog"))
        {
            // Here we set up the file dialog before opening the popup
            YAML::Node config;
            if (std::filesystem::exists("library.yaml"))
            {
                config = YAML::LoadFile("library.yaml");
            }
            else
            {
                std::cerr << "library.yaml not found, defaulting to current directory." << std::endl;
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
                YAML::Node config = YAML::LoadFile("library.yaml");

                std::cout << absolutePathString << std::endl;
                // Update the audiobooks_directory value

                config["audiobooks_directory"] = absolutePathString;

                // Write the file back
                std::ofstream fout("library.yaml");
                fout << config;
                fout.close();
                player.loadLibrary("library.yaml");
                // action

                // close
                ImGuiFileDialog::Instance()->Close();
            }

            // ImGui::EndPopup();
        }

        ImGui::Text("This is some useful text.");
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}