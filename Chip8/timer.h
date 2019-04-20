#pragma once
#include <thread>
#include <memory>
#include "chip8.h"

typedef struct
{
	int soundFileId_;
	int loopSoundId_;
	std::unique_ptr<std::thread> timeThread_;
	Chip8 * chip_;
} Timer;

void setupTimer(Timer * time, Chip8 * chip);
void runTimer();
void cleanupTimer();