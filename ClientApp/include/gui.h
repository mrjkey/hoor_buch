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
void DisplayLibrary(AudioBookPlayer *player, sf::Music &music);
void DisplayNavigationPanel();