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
void DisplayLibrary(const std::vector<Audiobook> &audiobooks, sf::Music &music)
{
    if (ImGui::Begin("Library", nullptr, ImGuiWindowFlags_NoCollapse))
    {
        for (const auto &book : audiobooks)
        {
            if (ImGui::TreeNode(book.title.c_str()))
            {
                // Display progress bar for the audiobook
                ImGui::ProgressBar(book.progress, ImVec2(-FLT_MIN, 0.0f), "Progress");

                if (ImGui::Button("Play"))
                {
                    // Trigger audio playback
                    music.play();
                }
                ImGui::SameLine();
                if (ImGui::Button("Pause"))
                {
                    // Pause the audio
                    music.pause();
                }
                ImGui::SameLine();
                if (ImGui::Button("Stop"))
                {
                    // Stop the audio
                    music.stop();
                }

                // Display info and controls for each audiobook here
                // ...

                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
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
