#include <iostream>
#include "math_utils.h"
#include "connection.h"
#include <boost/asio.hpp>

// #include <gtk/gtk.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include <GL/gl.h>

using boost::asio::ip::tcp;
namespace asio = boost::asio;

int main(int argc, char **argv)
{
    // Check the number of parameters
    // if (argc < 3)
    // {
    //     std::cerr << "Usage: " << argv[0] << " <port> <peer_address> <peer_port>" << std::endl;
    //     return 1;
    // }

    // int port = std::stoi(argv[1]);
    // std::string peer_address = argv[2];
    // int peer_port = std::stoi(argv[3]);

    // std::cout << "Starting Client on port " << port << std::endl;
    // std::cout << "Connecting to peer at " << peer_address << ":" << peer_port << std::endl;

    std::cout << "Starting on port " << std::endl;

    // try
    // {
    //     if (argc != 4)
    //     {
    //         std::cerr << "Usage: ClientApp <server> <port> <mode: server|client>" << std::endl;
    //         return 1;
    //     }

    //     asio::io_context io_context;
    //     short port = std::stoi(argv[2]);

    //     if (std::string(argv[3]) == "server")
    //     {
    //         start_server(io_context, port, "../test.txt");
    //     }
    //     else
    //     {
    //         start_client(io_context, argv[1], port, "test.txt");
    //     }
    // }
    // catch (std::exception &e)
    // {
    //     std::cerr << e.what() << std::endl;
    // }

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(640, 480, "My Window", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Here you can build your ImGui interfaces
        ImGui::Begin("Hello, world!");
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

    return 0;
}