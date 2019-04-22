/**	@file chip8.cpp
@author Benjamin Godin
@date 2019-04-21
@version 1.0.0
@note Developed for C++17/vc14.1
@brief Chip8 functionality
*/

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <direct.h>
#include "chip8.hpp"
#include "graphics.hpp"

static const uint8_t fontsetSize = 80;
static uint8_t delayCounter = 0;
static uint8_t soundCounter = 0;
static const uint32_t fullNano = 16'666'666;
static const uint32_t halfNano = 8'333'333;
static std::chrono::time_point<std::chrono::system_clock> soundTime;
static std::chrono::time_point<std::chrono::system_clock> delayTime;
static time_t tRand;

/**
@name:		initChip
@purpose:	Initialzes a Chip8 struct
@param:		Chip8 *
@return:	void
*/
void initChip(Chip8 * chip)
{
	chip->progCounter_ = 0x200;
	chip->opCode_ = 0;
	chip->regIndex_ = 0;
	chip->stackPointer_ = 0;
	chip->drawFlag_ = false;

	// debug flags
	chip->inDebug_ = chip->dumpRegs_ = chip->printInst_ = chip->goNext_ = false;

	// clear memory
	memset(chip->mem_, 0, MEMSIZE);

	// clear stack
	memset(chip->stack_, 0, STACKSIZE);

	// clear registers V0 to VF
	memset(chip->vReg_, 0, VREGSIZE);

	// load fontset
	memcpy_s(chip->mem_, MEMSIZE, font, fontsetSize);

	// reset timers
	chip->delayTimer_ = chip->soundTimer_ = 0;
	chip->soundPlaying_ = chip->isDelay_ = false;

	srand((unsigned)time(NULL));
}

/**
@name:		loadGame
@purpose:	Loads a game into a Chip8's memory
@param:		Chip8 *, const char *
@return:	void
*/
void loadGame(Chip8 * chip, const char * path)
{
	FILE * file;
	if (fopen_s(&file, path, "rb") != 0)
	{
		fprintf(stderr, "Could not open file %s\n", path);
		exit(1);
	}

	fseek(file, 0L, SEEK_END);
	size_t fileSize = ftell(file);

	if (fileSize > ROMSIZE)
	{
		fprintf(stderr, "The file \"%s\" exceeded the maximum ROM size, which is %d bytes.\n", path, ROMSIZE);
		exit(1);
	}

	rewind(file);
	fread((chip->mem_ + 512), sizeof(uint8_t), fileSize, file);
	fclose(file);
}

/**
@name:		executeCode
@purpose:	Executes the opcode at the PC's address
@param:		Chip8 *, GSI *
@return:	void
*/
void executeCode(Chip8 * chip, GSI * gsi)
{
	chip->goNext_ = false;

	if (chip->delayTimer_ > 0)
	{
		// ticks are 1/60 of a second
		// actual timestamps are used, because not each iteration of the main loop takes the same number of nanoseconds.
		auto now = std::chrono::system_clock::now();
		uint32_t nano = (uint32_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now - delayTime).count();
		uint8_t ticks = (nano + halfNano) / fullNano;

		if (ticks > 0)
		{
			if (chip->delayTimer_ >= ticks)
				chip->delayTimer_ -= ticks;
			else
				chip->delayTimer_ = 0;

			// this is only reset if there was a tick because each iteration is less than 1/60 of a second
			delayTime = std::chrono::system_clock::now();
		}
		
		if (chip->delayTimer_ == 0)
			chip->isDelay_ = false;
	}

	if (chip->soundTimer_ > 0)
	{
		// ticks are 1/60 of a second
		// actual timestamps are used, because not each iteration of the main loop takes the same number of nanoseconds.
		auto now = std::chrono::system_clock::now();
		uint32_t nano = (uint32_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now - soundTime).count();
		uint8_t ticks = (nano + halfNano) / fullNano;

		if (ticks > 0)
		{
			if (chip->soundTimer_ >= ticks)
				chip->soundTimer_ -= ticks;
			else
				chip->soundTimer_ = 0;

			// this is only reset if there was a tick because each iteration is less than 1/60 of a second
			soundTime = std::chrono::system_clock::now();
		}

		if (chip->soundTimer_ == 0)
			chip->soundPlaying_ = false;
	}
	
	chip->opCode_ = (chip->mem_[chip->progCounter_] << 8) | (chip->mem_[chip->progCounter_ + 1]);

	unsigned xIdx = (chip->opCode_ & 0x0F00) >> 8;
	unsigned yIdx = (chip->opCode_ & 0x00F0) >> 4;
	uint8_t nVal = static_cast<uint8_t>(chip->opCode_ & 0x00FF);
	uint8_t regVal = chip->vReg_[(chip->opCode_ & 0x0F00) >> 8];

	// print memory address hex, memory address local, opcode
	if (chip->printInst_)
		printf("%.4u  %.4X  %.4X\n", chip->progCounter_, chip->progCounter_, chip->opCode_);

	switch (chip->opCode_ & static_cast<uint16_t>(0xF000))
	{
		case 0x000:
			switch (chip->opCode_ & static_cast<uint16_t>(0x0FFF))
			{
				case CALL_RCA_ADDR:
					chip->progCounter_ +=2;
					break;
				case CLEAR_SCREEN:
					clearScreen(gsi);
					break;
				case RETURN:
				{	
					--chip->stackPointer_;
					chip->progCounter_ = chip->stack_[chip->stackPointer_];
					chip->progCounter_ += 2;
				}
					break;
				default:
					printf("Unknown opcode: %x", chip->opCode_);
					cleanUpGraphics(gsi);
					exit(1);
					break;
			}
			break;
		case GOTO_ADDR:
			chip->progCounter_ = (chip->opCode_ & 0x0FFF);
			break;
		case CALL_SUB:
		{
			chip->stack_[chip->stackPointer_] = chip->progCounter_;
			++chip->stackPointer_;
			chip->progCounter_ = (chip->opCode_ & 0x0FFF);
		}
			break;
		case VX_SKIP_EQUAL_ADDR:
		{
			if (nVal == regVal)
				chip->progCounter_ += 4;
			else
				chip->progCounter_ += 2;
		}	
			break;
		case VX_SKIP_NEQUAL_ADDR:
		{
			if (nVal != regVal)
				chip->progCounter_ += 4;
			else
				chip->progCounter_ += 2;
		}
			break;
		case VX_NOT_VY:
		{
			if (chip->vReg_[xIdx] != chip->vReg_[yIdx])
				chip->progCounter_ += 4;
			else
				chip->progCounter_ += 2;
		}
			break;
		case SET_VX_TO_ADDR:
		{
			chip->vReg_[xIdx] = nVal;
			chip->progCounter_ += 2;
		}
			break;
		case SET_VX_VX_PLUS_ADDR:
		{
			chip->vReg_[xIdx] += nVal;
			chip->progCounter_ += 2;
		}
			break;
		case 0x8000:
			switch (chip->opCode_ & static_cast<uint16_t>(0xF00F))
			{
				case SET_VX_TO_VY:
				{
					chip->vReg_[xIdx] = chip->vReg_[yIdx];
					chip->progCounter_ += 2;
				}
					break;
				case SET_VX_VX_OR_VY:
				{
					chip->vReg_[xIdx] |= chip->vReg_[yIdx];
					chip->progCounter_ += 2;
				}
					break;
				case SET_VX_VX_AND_VY:
				{
					chip->vReg_[xIdx] &= chip->vReg_[yIdx];
					chip->progCounter_ += 2;
				}
					break;
				case SET_VX_VX_XOR_VY:
				{
					chip->vReg_[xIdx] ^= chip->vReg_[yIdx];
					chip->progCounter_ += 2;
				}
					break;
				case SET_VX_VX_PLUS_VY:
				{
					
					if (chip->vReg_[yIdx] > (0xFF - chip->vReg_[xIdx]))
						chip->vReg_[0xF] = 1;
					else 
						chip->vReg_[0xF] = 0;

					chip->vReg_[xIdx] += chip->vReg_[yIdx];
					chip->progCounter_ += 2;
				}
					break;
				case SET_VX_VX_MINUS_VY:
				{
					if (chip->vReg_[yIdx] > chip->vReg_[xIdx])
						chip->vReg_[0xF] = 0;
					else
						chip->vReg_[0xF] = 1;

					chip->vReg_[xIdx] -= chip->vReg_[yIdx];
					chip->progCounter_ += 2;
				}
					break;
				case SET_VX_SHIFT_ONE_RIGHT:
				{
					chip->vReg_[0xF] = chip->vReg_[xIdx] & 1;	// xVal & 0000 0001 
					chip->vReg_[xIdx] >>= 1;
					chip->progCounter_ += 2;
				}
					break;
				case SET_VX_VY_MINUS_VX:
				{
					if (chip->vReg_[xIdx] > chip->vReg_[yIdx])
						chip->vReg_[0xF] = 0;
					else
						chip->vReg_[0xF] = 1;

					chip->vReg_[xIdx] = chip->vReg_[yIdx] - chip->vReg_[xIdx];
					chip->progCounter_ += 2;
				}
					break;
				case SET_VX_SHIFT_ONE_LEFT:
				{
					chip->vReg_[0xF] = chip->vReg_[xIdx] & 128;	// xVal & 1000 0000 
					chip->vReg_[xIdx] <<= 1;
					chip->progCounter_ += 2;
				}
					break;
				default:
					printf("Unknown opcode: %x", chip->opCode_);
					cleanUpGraphics(gsi);
					exit(1);
					break;
			}
			break;
		case CHECK_VX_IS_VY:
		{	
			if (chip->vReg_[xIdx] == chip->vReg_[yIdx])
				chip->progCounter_ += 2;
			else
				chip->progCounter_ += 4;
		}
			break;
		case SET_INDEX_TO_ADDR_VAL:
		{
			chip->regIndex_ = (chip->opCode_ & 0x0FFF);
			chip->progCounter_ += 2;
		}
			break;
		case JUMP_TO_ADDR_PLUS_V0:
		{
			chip->progCounter_ = (chip->opCode_ & 0x0FFF) + chip->vReg_[0];
		}
			break;
		case SET_VX_RAND_AND_NN:
		{
			chip->vReg_[xIdx] = (rand() % 256) & nVal;
			chip->progCounter_ += 2;
		}
			break;
		case DRAW_VX_VY_N:
		{
			uint16_t xCoord = chip->vReg_[xIdx];
			uint16_t yCoord = chip->vReg_[yIdx];
			uint16_t height = (chip->opCode_ & 0x000F);

			chip->vReg_[0xF] = 0;
			for (int y = 0; y < height; ++y)
			{
				uint16_t pixelRow = chip->mem_[chip->regIndex_ + y];
				for (int x = 0; x < 8; ++x)
				{
					if ((pixelRow & (0x80 >> x)) != 0)
						if(drawSprite(gsi, x + xCoord, y + yCoord, height))
							chip->vReg_[0xF] = 1;
				}
			}
			chip->drawFlag_ = true;
			chip->progCounter_ += 2;
		}
			break;
		case 0xE000:
			switch (chip->opCode_ & static_cast<uint16_t>(0xF0FF))
			{
				case SKIP_IF_KEY_PRESSED:
				{
					if (chip->key_[chip->vReg_[xIdx]] != 0)
						chip->progCounter_ += 4;
					else
						chip->progCounter_ += 2;
				}
					break;
				case SKIP_IF_KEY_NT_PRESSED:
				{
					if (chip->key_[chip->vReg_[xIdx]] == 0)
						chip->progCounter_ += 4;
					else
						chip->progCounter_ += 2;
				}
					break;
				default:
					printf("Unknown opcode: %x", chip->opCode_);
					cleanUpGraphics(gsi);
					exit(1);
					break;
			}
			break;
		case 0xF000:
			switch (chip->opCode_ & static_cast<uint16_t>(0xF0FF))
			{
				case SET_VX_TO_DELAY_TIMER:
				{
					chip->vReg_[xIdx] = chip->delayTimer_;
					chip->progCounter_ += 2;
				}
					break;
				case WAIT_FOR_KEY_PRESS_VX:
				{
					bool isPressed = false;
					for (int i = 0; i < 16; ++i)
					{
						if (chip->key_[i] != 0)
						{
							isPressed = true;
							chip->vReg_[xIdx] = i;
						}
					}

					if (!isPressed)
						return;

					chip->progCounter_ += 2;
				}
					break;
				case SET_DELAY_TIMER_TO_VX:
				{
					chip->delayTimer_ = chip->vReg_[xIdx];
					
					if (!chip->isDelay_)
					{
						delayTime = std::chrono::system_clock::now();
						chip->isDelay_ = true;
					}
						
					chip->progCounter_ += 2;
				}
					break;
				case SET_SOUND_TIMER_TO_VX:
				{
					chip->soundTimer_ = chip->vReg_[xIdx];

					if (!chip->soundPlaying_)
					{
						soundTime = std::chrono::system_clock::now();
						chip->soundPlaying_ = true;
					}

					chip->progCounter_ += 2;
				}
					break;
				case SET_INDEX_PLUS_VX:
				{
					if (chip->regIndex_ + chip->vReg_[xIdx] > 0x0FFF)
						chip->vReg_[0xF] = 1;
					else
						chip->vReg_[0xF] = 0;

					chip->regIndex_ += chip->vReg_[xIdx];
					chip->progCounter_ += 2;
				}
					break;
				case SET_INDEX_TO_SPRITE:
				{
					chip->regIndex_ = chip->vReg_[xIdx] * 5; // 4x5 font, offset by 5 to find character
					chip->progCounter_ += 2;
				}
					break;
				case STORE_BINARY_DEC_VX:
				{
					uint8_t xVal = chip->vReg_[xIdx];

					chip->mem_[chip->regIndex_] = (xVal / 100) % 10;
					chip->mem_[chip->regIndex_ + 1] = (xVal / 10) % 10;
					chip->mem_[chip->regIndex_ + 2] = xVal % 10;
					
					chip->progCounter_ += 2;
				}
					break;
				case STORE_V0_TO_VX_AT_IDX:
				{
					for (unsigned i = 0; i <= xIdx; ++i)
						chip->mem_[chip->regIndex_ + i] = chip->vReg_[i];

					chip->regIndex_ += chip->vReg_[xIdx] + 1;
					chip->progCounter_ += 2;
				}
					break;
				case FILL_V0_TO_VX_AT_IDX:
				{
					for (unsigned i = 0; i <= xIdx; ++i)
						chip->vReg_[i] = chip->mem_[chip->regIndex_ + i];

					chip->regIndex_ += chip->vReg_[xIdx] + 1;
					chip->progCounter_ += 2;
				}
					break;
				default:
					printf("Unknown opcode: %x", chip->opCode_);
					cleanUpGraphics(gsi);
					exit(1);
					break;
			}
			break;
		default:
			printf("Unknown opcode: %x", chip->opCode_);
			cleanUpGraphics(gsi);
			exit(1);
			break;
	}

	// dump registers, mem address at index
	if (chip->dumpRegs_)
	{
		printf("Register Values:\n");
		for (int i = 0; i < 0xF; i += 4)
			printf("%.4X  %.4X  %.4X  %.4X\n", chip->vReg_[i], chip->vReg_[i + 1], chip->vReg_[i + 2], chip->vReg_[i + 3]);

		printf("Address of index: %.4X\n", chip->regIndex_);
		printf("Value at index: %.4x\n", chip->mem_[chip->regIndex_]);

		printf("Stack:\n");
		if (chip->stackPointer_ == 0)
			printf("No stack!\n");

		for(unsigned i = 0; i < chip->stackPointer_; ++i)
			printf("%u: %.4X\n", i, chip->stack_[i]);
	}
}
