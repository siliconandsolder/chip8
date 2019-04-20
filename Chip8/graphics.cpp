#include <Windows.h>
#include "graphics.h"

static const short NUM_ROWS = 32;
static const short NUM_COLS = 64;
static const short WIN_WIDTH = 1024;
static const short WIN_HEIGHT = 512;
static const short RECT_SIZE = 16;
static const short OFFSET = 1;

static unsigned getFlippedY(unsigned y)
{
	return WIN_HEIGHT - y;
}

void setupScreen(GSI * gi, Chip8 * chip)
{
	gi->chip_ = chip;

	slWindow(WIN_WIDTH, WIN_HEIGHT, "Chip 8 Emulator", false);
	slSetBackColor(0, 0, 0);
	slSetForeColor(1, 1, 1, 1);

	for (int y = 0; y < NUM_ROWS; ++y)
		for (int x = 0; x < NUM_COLS; ++x)
			gi->gBuffer_[x][y] = 0;

	for(int i = 0; i < 16; ++i)
		gi->keys_[i] = 0;
		
	gi->soundFileId_ = slLoadWAV("C:\\Users\\Ben\\Documents\\Fanshawe_2018_Fall\\Other\\Chip8\\Chip8\\Sound\\beep.wav");
	gi->loopSoundId_ = 0;
}

void clearScreen(GSI * gsi)
{
	for (int y = 0; y < NUM_ROWS; ++y)
		for (int x = 0; x < NUM_COLS; ++x)
			gsi->gBuffer_[x][y] = 0;
}

bool drawSprite(GSI * gsi, uint16_t xCoord, uint16_t yCoord, uint16_t height)
{
	return (gsi->gBuffer_[xCoord][yCoord] ^= 1) == 0;
}

void playSound(GSI * gsi)
{
	/*if (slSoundLooping(gsi->loopSoundId_) == 0)
		gsi->loopSoundId_ = slSoundLoop(gsi->soundFileId_);*/
	Beep(500, 500);
}

void stopSound(GSI * gsi)
{
	if (slSoundPlaying(gsi->loopSoundId_) != 0)
		slSoundStop(gsi->loopSoundId_);
}

void drawScreen(GSI * gsi)
{
	if (!gsi->chip_->drawFlag_)
		return;

	for (int y = 0; y < NUM_ROWS; ++y)
		for (int x = 0; x < NUM_COLS; ++x)
			if (gsi->gBuffer_[x][y] == 1)
				slRectangleFill(x * RECT_SIZE + (RECT_SIZE / 2), getFlippedY(y * RECT_SIZE + (RECT_SIZE / 2)), RECT_SIZE, RECT_SIZE);

	slRender();
	gsi->chip_->drawFlag_ = false;
}

void getInput(GSI * gsi)
{
	for (int i = 0; i < 16; ++i)
		gsi->chip_->key_[i] = 0;

	if (slGetKey('1') != 0)
	{
		gsi->chip_->key_[0x0] = 1;
	}
	else if (slGetKey('2') != 0)
	{
		gsi->chip_->key_[0x1] = 1;
	}
	else if (slGetKey('3') != 0)
	{
		gsi->chip_->key_[0x2] = 1;
	}
	else if (slGetKey('4') != 0)
	{
		gsi->chip_->key_[0x3] = 1;
	}
	else if (slGetKey('Q') != 0)
	{
		gsi->chip_->key_[0x4] = 1;
	}
	else if (slGetKey('W') != 0)
	{
		gsi->chip_->key_[0x5] = 1;
	}
	else if (slGetKey('E') != 0)
	{
		gsi->chip_->key_[0x6] = 1;
	}
	else if (slGetKey('R') != 0)
	{
		gsi->chip_->key_[0x7] = 1;
	}
	else if (slGetKey('A') != 0)
	{
		gsi->chip_->key_[0x8] = 1;
	}
	else if (slGetKey('S') != 0)
	{
		gsi->chip_->key_[0x9] = 1;
	}
	else if (slGetKey('D') != 0)
	{
		gsi->chip_->key_[0xA] = 1;
	}
	else if (slGetKey('F') != 0)
	{
		gsi->chip_->key_[0xB] = 1;
	}
	else if (slGetKey('Z') != 0)
	{
		gsi->chip_->key_[0xC] = 1;
	}
	else if (slGetKey('X') != 0)
	{
		gsi->chip_->key_[0xD] = 1;
	}
	else if (slGetKey('C') != 0)
	{
		gsi->chip_->key_[0xE] = 1;
	}
	else if (slGetKey('V') != 0)
	{
		gsi->chip_->key_[0xF] = 1;
	}

	do
	{
		// debug keys
		if (slGetKey('B') != 0)
		{
			if (gsi->chip_->inDebug_ == false)
			{
				gsi->chip_->inDebug_ = true;
				const char * toggled = (gsi->chip_->inDebug_) ? "ON." : "OFF.";
				printf("Debug Mode %s\n", toggled);
			}
		}

		if (slGetKey('G') != 0)
		{
			if (gsi->chip_->inDebug_ == true)
			{
				gsi->chip_->inDebug_ = false;
				const char * toggled = (gsi->chip_->inDebug_) ? "ON." : "OFF.";
				printf("Debug Mode %s\n", toggled);
			}
		}

		if (slGetKey('P') != 0)
		{
			if (gsi->chip_->printInst_ == false)
			{
				gsi->chip_->printInst_ = true;
				const char * toggled = (gsi->chip_->printInst_) ? "ON." : "OFF.";
				printf("Print-Instruction Mode %s\n", toggled);
			}
		}

		if (slGetKey('L') != 0)
		{
			if (gsi->chip_->printInst_ == true)
			{
				gsi->chip_->printInst_ = false;
				const char * toggled = (gsi->chip_->printInst_) ? "ON." : "OFF.";
				printf("Print-Instruction Mode %s\n", toggled);
			}
		}

		if (slGetKey('O') != 0)
		{
			if (gsi->chip_->dumpRegs_ == false)
			{
				gsi->chip_->dumpRegs_ = true;
				const char * toggled = (gsi->chip_->dumpRegs_) ? "ON." : "OFF.";
				printf("Register-Dump Mode %s\n", toggled);
			}
		}

		if (slGetKey('K') != 0)
		{
			if (gsi->chip_->dumpRegs_ == true)
			{
				gsi->chip_->dumpRegs_ = false;
				const char * toggled = (gsi->chip_->dumpRegs_) ? "ON." : "OFF.";
				printf("Register-Dump Mode %s\n", toggled);
			}
		}

		if (slGetKey('N') != 0 && gsi->chip_->inDebug_)
			if (gsi->chip_->goNext_ == false)
				gsi->chip_->goNext_ = true;
		
		if (gsi->chip_->inDebug_)
		{
			bool draw = gsi->chip_->drawFlag_;
			gsi->chip_->drawFlag_ = true;
			drawScreen(gsi);
			gsi->chip_->drawFlag_ = draw;
		}
	} 
	while (!gsi->chip_->goNext_ && gsi->chip_->inDebug_);
}

void cleanUpGraphics(GSI * gsi)
{
	gsi->chip_->drawFlag_ = true;
	clearScreen(gsi);
	drawScreen(gsi);
	stopSound(gsi);
}
