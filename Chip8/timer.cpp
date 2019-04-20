#include "timer.h"
#include <sl.h>

static bool isRunning = true;
static bool soundPlaying = false;
static Timer * timer;

void setupTimer(Timer * time, Chip8 * chip)
{
	timer = time;
	timer->chip_ = chip;
	//timer->soundFileId_ = slLoadWAV("****");
	timer->soundFileId_ = 0;
	timer->loopSoundId_ = 0;
	timer->timeThread_ = std::unique_ptr<std::thread>(new std::thread(runTimer));
}

void runTimer()
{
	while (isRunning)
	{
		if (timer->chip_->delayTimer_ > 0)
			--timer->chip_->delayTimer_;

		if (soundPlaying)
		{
			if (timer->chip_->soundTimer_ > 0)
				--timer->chip_->soundTimer_;
			else
			{
				slSoundStop(timer->loopSoundId_);
				soundPlaying = false;
			}
		}
		else if (!soundPlaying)
		{
			if (!soundPlaying && timer->chip_->soundTimer_ > 0)
				timer->loopSoundId_ = slSoundLoop(timer->soundFileId_);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	if (slSoundLooping(timer->loopSoundId_) != 0)
		slSoundStop(timer->loopSoundId_);
}

void cleanupTimer()
{
	isRunning = false;
	timer->timeThread_->join();
}
