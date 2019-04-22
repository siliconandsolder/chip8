/**	@file graphics.hpp
@author Benjamin Godin
@date 2019-04-21
@version 1.0.0
@note Developed for C++17/vc14.1
@brief Function declarations and struct for Graphics and Input
*/

#pragma once
#include <sl.h>
#include <cstdint>
#include <cstdio>
#include "chip8.hpp"

// GSI - Graphics, Sound, and Input
typedef struct GSI
{
	uint8_t gBuffer_[64][32];
	uint8_t keys_[16];
	Chip8 * chip_;
	int soundFileId_;
	int loopSoundId_;
} GSI;

void setupScreen(GSI * gi, Chip8 * chip);
void clearScreen(GSI * gsi);
void cleanUpGraphics(GSI * gsi);
bool drawSprite(GSI * gsi, uint16_t xCoord, uint16_t yCoord, uint16_t height);
void playSound(GSI * gsi);
void stopSound(GSI * gsi);
void drawScreen(GSI * gsi);
void getInput(GSI * gsi);