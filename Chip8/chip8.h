#pragma once
#include <cstdio>
#include <atomic>
#include <chrono>
#include "font_set.h"

enum OpCode : uint16_t
{
	CALL_RCA_ADDR = 0x0000,			// 0NNN
	CLEAR_SCREEN = 0x0E00,			// 00E0
	RETURN = 0x00EE,				// 00EE
	GOTO_ADDR = 0x1000,				// 1NNN
	CALL_SUB = 0x2000,				// 2NNN
	VX_SKIP_EQUAL_ADDR = 0x3000,	// 3XNN
	VX_SKIP_NEQUAL_ADDR = 0x4000,	// 4XNN
	VX_NOT_VY = 0x5000,				// 5XY0
	SET_VX_TO_ADDR = 0x6000,		// 6XNN
	SET_VX_VX_PLUS_ADDR = 0x7000,	// 7XNN
	SET_VX_TO_VY = 0x8000,			// 8XY0
	SET_VX_VX_OR_VY = 0x8001,		// 8XY1
	SET_VX_VX_AND_VY = 0x8002,		// 8XY2
	SET_VX_VX_XOR_VY = 0x8003,		// 8XY3
	SET_VX_VX_PLUS_VY = 0x8004,		// 8XY4
	SET_VX_VX_MINUS_VY = 0x8005,	// 8XY5
	SET_VX_SHIFT_ONE_RIGHT = 0x8006,// 8XY6
	SET_VX_VY_MINUS_VX = 0x8007,	// 8XY7
	SET_VX_SHIFT_ONE_LEFT = 0x800E,	// 8XYE
	CHECK_VX_IS_VY = 0x9000,		// 9XY0
	SET_INDEX_TO_ADDR_VAL = 0xA000,	// ANNN
	JUMP_TO_ADDR_PLUS_V0 = 0xB000,	// BNNN
	SET_VX_RAND_AND_NN = 0xC000,	// CXNN
	DRAW_VX_VY_N = 0xD000,			// DXYN
	SKIP_IF_KEY_PRESSED = 0xE09E,	// EX9E 
	SKIP_IF_KEY_NT_PRESSED = 0xE0A1,// EXA1
	SET_VX_TO_DELAY_TIMER = 0xF007,	// FX07
	WAIT_FOR_KEY_PRESS_VX = 0xF00A,	// FX0A
	SET_DELAY_TIMER_TO_VX = 0xF015,	// FX15
	SET_SOUND_TIMER_TO_VX = 0xF018,	// FX18
	SET_INDEX_PLUS_VX = 0xF01E,		// FX1E
	SET_INDEX_TO_SPRITE = 0xF029,	// FX29
	STORE_BINARY_DEC_VX = 0xF033,	// FX33
	STORE_V0_TO_VX_AT_IDX = 0xF055,	// FX55
	FILL_V0_TO_VX_AT_IDX = 0xF065	// FX65
};

#define SLOW_SPEED	1'851'852	// 540Hz
#define MED_SPEED	1'041'667	// 960hz
#define FAST_SPEED	  724'638	// 1380Hz

enum SPEED : uint8_t
{
	SLOW = 9,
	MED = 16,
	FAST = 23
};

#define MEMSIZE 4096
#define ROMSIZE 3584
#define VREGSIZE 16
#define GFXSIZE 2048
#define STACKSIZE 16
#define KEYSIZE 16

typedef struct Chip8
{
	uint16_t opCode_;
	uint8_t mem_[MEMSIZE];
	uint8_t rom_[ROMSIZE];
	uint8_t vReg_[VREGSIZE];
	
	uint16_t regIndex_;
	uint16_t progCounter_;
	
	uint8_t gfx_[GFXSIZE];
	
	std::atomic<uint8_t> delayTimer_;
	std::atomic<uint8_t> soundTimer_;
	SPEED speed_;
	bool soundPlaying_;
	bool isDelay_;
	
	uint16_t stack_[STACKSIZE];
	uint16_t stackPointer_;
	
	uint8_t key_[KEYSIZE];
	bool drawFlag_;

	// flags for debugger
	bool inDebug_;
	bool dumpRegs_;
	bool printInst_;
	bool goNext_;
} Chip8;

typedef struct GSI GSI;

void initChip(Chip8 * chip);
void loadGame(Chip8 * chip, const char * path);
void executeCode(Chip8 * chip, GSI * gsi);