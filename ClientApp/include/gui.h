// ClientApp/include/gui.h

#pragma once

#include <vector>
#include "datatypes.h"
#include <SFML/Audio.hpp>
#include "GLFW/glfw3.h"
#include "imgui.h"

void SetModernImGuiStyle();
void SetDarkStyle();
void DisplayAlbumArt(GLuint textureId);
void DisplayLibrary(const std::vector<Audiobook> &audiobooks, sf::Music &music);
void DisplayNavigationPanel();