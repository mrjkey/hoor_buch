// ClientApp/src/gui.cpp

#include "gui.h"

#include "ImGuiFileDialog.h"
#include "LibraryManager.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

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

void DisplayAlbumArt(GLuint textureId)
{
    // Display album art
    ImGui::Image((void *)(intptr_t)textureId, ImVec2(100, 100));
}

void AddBookDialog(AudioBookPlayer *player)
{
    // Open a file dialog to select a new audiobook (directory)
    IGFD::FileDialogConfig fileDialogConfig;
    fileDialogConfig.path = player->library_directory;
    ImGuiFileDialog::Instance()->OpenDialog("add_book", "Choose Directory", nullptr, fileDialogConfig);
}

void AddBookFunctionality(AudioBookPlayer *player)
{
    // Add a new audiobook to the library
    if (ImGui::Button("Add Audiobook"))
    {
        AddBookDialog(player);
    }
    ImVec2 popupSize = ImVec2(600, 400);
    ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);
    if (ImGuiFileDialog::Instance()->Display("add_book"))
    {

        // action if OK
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string directoryPath = ImGuiFileDialog::Instance()->GetCurrentPath();

            // Convert to an absolute path
            std::filesystem::path absPath = std::filesystem::absolute(directoryPath);

            // If you need it as a string
            std::string abs_book_path = absPath.string();

            add_new_audiobook(&player->library, abs_book_path);

            remove_duplicate_audiobooks(&player->library);

            // Save the library
            CreateOrUpdateLibraryIndex(player->library_file_path, player->library);

            // close
            ImGuiFileDialog::Instance()->Close();
        }
    }
}

// Display function for the library
void DisplayLibrary(AudioBookPlayer *player)
{
    // nest the library as a child of another component
    ImGui::BeginChild("Library", ImVec2(0, 0), true);

    // button to add a new audiobook
    AddBookFunctionality(player);

    ImGui::Separator();
    // Display each audiobook in the library, and the progress of each.
    // The library should have a scroll bar if it is too large to fit in the window.
    // The audiobooks should be selectable so that the user can choose which one to play.
    // Selecting an audio book should load it into the player.
    for (int i = 0; i < player->library.size(); i++)
    {
        // begin a child component for each audiobook
        // ImGui::BeginChild(player->library[i].title.c_str(), ImVec2(0, 0), true);
        // Create a selectable block for each audiobook
        // The selectable ID needs to be unique for each book, hence using the book index
        std::string selectableId = player->library[i].title.c_str();
        if (ImGui::Selectable(selectableId.c_str(), player->currentBookIndex == i, ImGuiSelectableFlags_SpanAllColumns))
        {
            player->selectBook(i); // This function should set the selectedBookIndex in your player
        }

        // Display the audiobook title
        // ImGui::Text(player->library[i].title.c_str());

        // Display the audiobook progress
        ImGui::ProgressBar(player->library[i].progress, ImVec2(-FLT_MIN, 0.0f), "Progress");

        // Display the audiobook author
        // ImGui::Text(player->library[i].author.c_str());

        // Display the audiobook duration
        ImGui::Text(std::to_string(player->library[i].duration).c_str());

        // Display the audiobook cover art
        // DisplayAlbumArt(audiobooks[i].textureId);

        // add a space between each audiobook
        ImGui::Spacing();

        // add a separator between each audiobook
        ImGui::Separator();

        // Selecting an audiobook should load it into the player
        if (ImGui::IsItemClicked())
        {
            // get the book that was clicked
            player->selectBook(i);

            // music.openFromFile(audiobooks[i].path);
            // music.play();
        }

        // end the child component for each audiobook
        // ImGui::EndChild();
    }

    ImGui::EndChild();
}

void DisplayNavigationPanel()
{
    // Left side panel for navigation
    ImGui::BeginChild("NavigationPanel", ImVec2(200, 0), true);
    // Categories or playlists can be listed here
    ImGui::Text("All Audiobooks");
    ImGui::Text("Recently Played");
    ImGui::Text("Favorites");
    // ... other categories
    ImGui::EndChild();
}

void gui_playback_buttons(AudioBookPlayer *player)
{
    // Display playback buttons
    if (ImGui::Button("Back 1m"))
    {
        std::cout << "Back 1m" << std::endl;
        // Back 1 minute
        player->rewind(60);
    }

    ImGui::SameLine();

    if (ImGui::Button("Back 10s"))
    {
        std::cout << "Back 10s" << std::endl;
        // Back 10 seconds
        player->rewind(10);
    }

    ImGui::SameLine();

    if (ImGui::Button("Play"))
    {
        std::cout << "Play" << std::endl;
        // Trigger audio playback
        player->play();
    }

    ImGui::SameLine();

    if (ImGui::Button("Pause"))
    {
        std::cout << "Pause" << std::endl;
        // Pause the audio
        player->pause();
    }

    ImGui::SameLine();

    if (ImGui::Button("Foward 10s"))
    {
        std::cout << "Foward 10s" << std::endl;
        // Foward 10 seconds
        player->fast_forward(10);
    }

    ImGui::SameLine();

    if (ImGui::Button("Foward 1m"))
    {
        std::cout << "Foward 1m" << std::endl;
        // Foward 1 minute
        player->fast_forward(60);
    }
}

void gui_setup(GLFWwindow **window)
{
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
    // SetModernImGuiStyle();
    SetDarkStyle();

    // GLuint albumArtTexture = LoadTexture("path_to_album_art.png"); // Implement this function to load texture

    // DisplayNavigationPanel();
    // DisplayAlbumArt(albumArtTexture);
}