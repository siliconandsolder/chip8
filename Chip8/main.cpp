#include "graphics.h"
#include <chrono>
#include <thread>
#include <regex>

// chip8.exe <program_path> --<speed>

int main(int argc, char * argv[])
{
	Chip8 chip;
	GSI gsi;

	// default speed if there are no arguments
	long speed = MED_SPEED;
	chip.speed_ = MED;

	char path[256] = "\\Games\\PONG.bin";
	char slowFlag[] = "--slow";
	char medFlag[] = "--med";
	char fastFlag[] = "--fast";

	if (argc == 2)
	{
		char flagCheck[3];
		strncpy(flagCheck, argv[1], 2);
		flagCheck[2] = '\0';
		if (strcmp(flagCheck, "--") == 0)
		{
			if (strcmp(argv[1], slowFlag) == 0)
			{
				speed = SLOW_SPEED;
				chip.speed_ = SLOW;
			}
			else if (strcmp(argv[1], medFlag) == 0)
			{
				speed = MED_SPEED;
				chip.speed_ = MED;
			}
			else if (strcmp(argv[1], fastFlag) == 0)
			{
				speed = FAST_SPEED;
				chip.speed_ = FAST;
			}
			else
			{
				printf("Speed flag not recognized!\n");
				exit(1);
			}
		}
		else
		{
			memcpy_s(path, 256, '\0', 256);
			memcpy_s(path, 256, argv[1], strlen(argv[1]));
		}
	}
	else if (argc == 3)
	{
		memcpy_s(path, 256, '\0', 256);
		memcpy_s(path, 256, argv[2], strlen(argv[2]));

		char flagCheck[3];
		strncpy(flagCheck, argv[2], 2);
		flagCheck[2] = '\0';
		if (strcmp(flagCheck, "--") == 0)
		{
			if (strcmp(argv[2], slowFlag) == 0)
			{
				long speed = SLOW_SPEED;
				chip.speed_ = SLOW;
			}
			else if (strcmp(argv[2], medFlag) == 0)
			{
				long speed = MED_SPEED;
				chip.speed_ = MED;
			}
			else if (strcmp(argv[2], fastFlag) == 0)
			{
				long speed = FAST_SPEED;
				chip.speed_ = FAST;
			}
			else
			{
				printf("Speed flag not recognized!\n");
				exit(1);
			}
		}
	}
	else if (argc > 3)
	{
		printf("Too many arguments!\n");
		exit(1);
	}

	initChip(&chip);
	setupScreen(&gsi, &chip);
	loadGame(&chip, "C:\\Users\\Ben\\Documents\\Fanshawe_2018_Fall\\Other\\Chip8\\x64\\Debug\\PONG.bin");
	slRender();

	while (!slGetKey(SL_KEY_ESCAPE))
	{
		getInput(&gsi);
		executeCode(&chip, &gsi);
		drawScreen(&gsi);

		std::this_thread::sleep_for(std::chrono::nanoseconds(speed));
	}

	cleanUpGraphics(&gsi);
	slClose();
	return 0;
}