// ClientApp/src/gui.cpp

#include "gui.h"

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

// Display function for the library
void DisplayLibrary(AudioBookPlayer *player, sf::Music &music)
{
    // nest the library as a child of another component
    ImGui::BeginChild("Library", ImVec2(0, 0), true);

    // button to add a new audiobook
    if (ImGui::Button("Add Audiobook"))
    {
        // Open a file dialog to select a new audiobook (directory)
        // ...

        // Read the audiobook metadata

        // Create the audiobook yaml file describing the audiobook

        // Add the audiobook to the library

        // Save the library

        // Reload the library
    }

    ImGui::Separator();
    // Display each audiobook in the library, and the progress of each.
    // The library should have a scroll bar if it is too large to fit in the window.
    // The audiobooks should be selectable so that the user can choose which one to play.
    // Selecting an audio book should load it into the player.
    for (int i = 0; i < player->library.size(); i++)
    {
        // Display the audiobook title
        ImGui::Text(player->library[i].title.c_str());

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
            // player->selectBook(i);
            // music.openFromFile(audiobooks[i].path);
            // music.play();
        }
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
