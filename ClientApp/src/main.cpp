#include <iostream>
#include <boost/asio.hpp>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include <GL/gl.h>
#include <SFML/Audio.hpp>
#include <fstream>
#include "ImGuiFileDialog.h"

#include "math_utils.h"
#include "connection.h"
#include "BookProgress.h"
#include "AudioBookPlayer.h"

// #include <portaudio.h>

using boost::asio::ip::tcp;
namespace asio = boost::asio;

int main(int argc, char **argv)
{
    std::cout << "Starting Client Application" << std::endl;

    // initialize glfw
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window = glfwCreateWindow(640, 480, "My Window", NULL, NULL);
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
    }
    else
    {
        // create a yaml file
        YAML::Node config;
        config["audiobooks_directory"] = "D:\\Torrents\\Books";
        std::ofstream fout("library.yaml");
        fout << config;
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

        // Here you can build your ImGui interfaces
        ImGui::Begin("Audio Book Player - Hoor Buch");

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

        // open Dialog Simple
        if (ImGui::Button("Open File Dialog"))
        {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".cpp,.h,.hpp", config);
        }

        // display
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                // action
            }

            // close
            ImGuiFileDialog::Instance()->Close();
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