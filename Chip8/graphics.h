#pragma once
#include <sl.h>
#include <cstdint>
#include <cstdio>
#include "chip8.h"

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