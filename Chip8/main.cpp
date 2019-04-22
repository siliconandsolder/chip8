#include "graphics.h"
#include <chrono>
#include <thread>

// chip8.exe <program_path> --<speed>

int main(int argc, char * argv[])
{
	Chip8 chip;
	GSI gsi;

	// default speed if there are no arguments
	long speed = MED_SPEED;

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
			}
			else if (strcmp(argv[1], medFlag) == 0)
			{
				speed = MED_SPEED;
			}
			else if (strcmp(argv[1], fastFlag) == 0)
			{
				speed = FAST_SPEED;
			}
			else
			{
				printf("Speed flag not recognized!\n");
				exit(1);
			}
		}
		else
		{
			memset(path, 0, 256);
			memcpy_s(path, 256, argv[1], strlen(argv[1]));
		}
	}
	else if (argc == 3)
	{
		memset(path, 0, 256);
		memcpy_s(path, 256, argv[1], strlen(argv[1]));

		char flagCheck[3];
		strncpy(flagCheck, argv[2], 2);
		flagCheck[2] = '\0';
		if (strcmp(flagCheck, "--") == 0)
		{
			if (strcmp(argv[2], slowFlag) == 0)
			{
				long speed = SLOW_SPEED;
			}
			else if (strcmp(argv[2], medFlag) == 0)
			{
				long speed = MED_SPEED;
			}
			else if (strcmp(argv[2], fastFlag) == 0)
			{
				long speed = FAST_SPEED;
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
	loadGame(&chip, path);
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