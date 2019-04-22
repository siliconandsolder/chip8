/**	@file graphics.cpp
@author Benjamin Godin
@date 2019-04-21
@version 1.0.0
@note Developed for C++17/vc14.1
@brief Graphics and Input functionality
*/

#include <chrono>
#include "graphics.hpp"

static const short NUM_ROWS = 32;
static const short NUM_COLS = 64;
static const short WIN_WIDTH = 1024;
static const short WIN_HEIGHT = 512;
static const short RECT_SIZE = 16;
static const short HALF_RECT_SIZE = RECT_SIZE / 2;
static const short OFFSET = 1;
static const char keys[] = "1234QWERASDFZXCV";
static std::chrono::time_point<std::chrono::system_clock> drawDelay;

/**
@name:		getFlippedY
@purpose:	SIGIL's y-axis is the reverse of most GUI y-axes. This reverses the y-coordinate around.
@param:		unsigned
@return:	unsigned
*/
static unsigned getFlippedY(unsigned y)
{
	return WIN_HEIGHT - y;
}

/**
@name:		setupScreen
@purpose:	Prepares the screen and input keys
@param:		GSI *, Chip8 *
@return:	void
*/
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

	drawDelay = std::chrono::system_clock::now();
}

/**
@name:		clearScreen
@purpose:	Clears the screen buffer 
@param:		GSI *
@return:	void
*/
void clearScreen(GSI * gsi)
{
	for (int y = 0; y < NUM_ROWS; ++y)
		for (int x = 0; x < NUM_COLS; ++x)
			gsi->gBuffer_[x][y] = 0;
}

/**
@name:		drawSprite
@purpose:	Flips the bit at a coordinate in the buffer. Returns true if that bit is now 0.
@param:		GSI *, uint16_t, uint16_t, uint16_t
@return:	bool
*/
bool drawSprite(GSI * gsi, uint16_t xCoord, uint16_t yCoord, uint16_t height)
{
	return (gsi->gBuffer_[xCoord][yCoord] ^= 1) == 0;
}


/**
@name:		playSound
@purpose:	ABANDONED - Plays a sound file on a loop, if it is not already playing.
@param:		GSI *
@return:	void
*/
void playSound(GSI * gsi)
{
	/*if (slSoundLooping(gsi->loopSoundId_) == 0)
		gsi->loopSoundId_ = slSoundLoop(gsi->soundFileId_);*/
}

/**
@name:		stopSound
@purpose:	ABANDONED - Stops a sound file, if it is playing.
@param:		GSI *
@return:	void
*/
void stopSound(GSI * gsi)
{
	/*if (slSoundPlaying(gsi->loopSoundId_) != 0)
		slSoundStop(gsi->loopSoundId_);*/
}

/**
@name:		drawScreen
@purpose:	Draws the buffer to the screen
@param:		GSI *
@return:	void
*/
void drawScreen(GSI * gsi)
{
	auto now = std::chrono::system_clock::now();
	uint32_t mSecs = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(now - drawDelay).count();

	// Due to the fact that slRender() also handles inputs, many
	// endgame screens can actually freeze SIGIL.
	// slRender() is called if no draw command has been issued for 750 milliseconds.
	if (!gsi->chip_->drawFlag_ && mSecs < 750)
		return;

	for (int y = 0; y < NUM_ROWS; ++y)
		for (int x = 0; x < NUM_COLS; ++x)
			if (gsi->gBuffer_[x][y] == 1)
				slRectangleFill(x * RECT_SIZE + HALF_RECT_SIZE, getFlippedY(y * RECT_SIZE + HALF_RECT_SIZE), RECT_SIZE, RECT_SIZE);

	slRender();
	gsi->chip_->drawFlag_ = false;
	drawDelay = std::chrono::system_clock::now();
}

/**
@name:		getInput
@purpose:	Detects if any input keys are currently pressed
@param:		GSI *
@return:	void
*/
void getInput(GSI * gsi)
{
	memset(gsi->chip_->key_, 0, 16);	// clear the key buffer

	for (int i = 0; i < 16; ++i)
		if (slGetKey(keys[i]) != 0)
			gsi->chip_->key_[i] = 1;

	do
	{
		// debug keys
		if (slGetKey('B') != 0)
		{
			if (gsi->chip_->inDebug_ == false)
			{
				gsi->chip_->inDebug_ = true;
				printf("Debug Mode ON.\n");
			}
		}

		if (slGetKey('G') != 0)
		{
			if (gsi->chip_->inDebug_ == true)
			{
				gsi->chip_->inDebug_ = false;
				printf("Debug Mode OFF.\n");
			}
		}

		if (slGetKey('P') != 0)
		{
			if (gsi->chip_->printInst_ == false)
			{
				gsi->chip_->printInst_ = true;
				printf("Print-Instruction Mode ON.\n");
			}
		}

		if (slGetKey('L') != 0)
		{
			if (gsi->chip_->printInst_ == true)
			{
				gsi->chip_->printInst_ = false;
				printf("Print-Instruction Mode OFF.\n");
			}
		}

		if (slGetKey('O') != 0)
		{
			if (gsi->chip_->dumpRegs_ == false)
			{
				gsi->chip_->dumpRegs_ = true;
				printf("Register-Dump Mode ON.\n");
			}
		}

		if (slGetKey('K') != 0)
		{
			if (gsi->chip_->dumpRegs_ == true)
			{
				gsi->chip_->dumpRegs_ = false;
				printf("Register-Dump Mode OFF.\n");
			}
		}

		if (slGetKey('N') != 0 && gsi->chip_->inDebug_)
			if (gsi->chip_->goNext_ == false)
				gsi->chip_->goNext_ = true;
		
		// Same problem as before: we need to call slRender() to get
		// key inputs.
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

/**
@name:		cleanUpGraphics
@purpose:	Cleans up the screen before exiting the program
@param:		GSI *
@return:	void
*/
void cleanUpGraphics(GSI * gsi)
{
	gsi->chip_->drawFlag_ = true;
	clearScreen(gsi);
	drawScreen(gsi);
	stopSound(gsi);
}
