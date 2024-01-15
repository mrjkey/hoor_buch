// ClientApp/include/gui.h

#pragma once

#include <vector>
#include "datatypes.h"
#include <SFML/Audio.hpp>
#include "GLFW/glfw3.h"
#include "imgui.h"

#include "AudioBookPlayer.h"

void SetModernImGuiStyle();
void SetDarkStyle();
void DisplayAlbumArt(GLuint textureId);
void DisplayLibrary(AudioBookPlayer *player);
void DisplayNavigationPanel();
void gui_playback_buttons(AudioBookPlayer *player);
void gui_setup(GLFWwindow **window);