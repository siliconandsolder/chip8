# Chip8
### A rudimentary Chip8 emulator, written in (mostly) C!

## Overview
First things first: I'd like to thank [Geoff Nagy](https://gitlab.com/geoff.nagy) for developing [SIGIL](https://gitlab.com/geoff-nagy/sigil), the graphics library that I used for this project.

Much like other Chip8 emulators on GitHub, this was made purely as an experiment in computer emulation. I have included a few games in the Games folder, but this emulator should (theoretically) work with any Chip8 game out there. **Note: this emulator does not support SuperChip games (yet).**

## Usage
This project was built with Visual Studio 2017, and as such will only run on Windows. Currently, this project can only be built in x64. Hopefully, it will have support for x86 soon. Besides that, this project can be compiled like any other Visual Studio project.

To run the emulator, simply type:
```
chip8.exe <path_to_game> <--slow/--med/--fast>
```
The `<path_to_game>` parameter is required; chip8.exe will close immediately if no path is provided. The second parameter, speed, is optional. If no speed is specified, chip8.exe defaults to `--med`.

For clarification, here is what the speed flags represent:

Flag | Speed in Hertz
---- | --------------
--slow | 600hz
--med | 1000hz
--flag | 1500hz

## Debug
My Chip8 emulator comes with its own debugger! While not a complete disassembler, it does allow you to step through each OpCode as it's read.

These are the key binding:

Key | Effect
----|-------
B | Enters debug mode, pausing the game.
G | Exits debug mode.
N | Steps to the next instruction while in debug mode.
P | Prints the most recently read OpCode, as well as the PC value, to the console.
L | Stops printing each OpCode to the screen.
O | Dumps the registers, stack, and index to the console.
K | Stops printing the registers, stack, and index to the console.

While this is a little tedious, I haven't managed to get toggle-keys working yet. Hopefully that can be resolved soon.

## Conclusion
This is my first adventure in emulation, so it's a huge learning-curve for me. I'm proud that my emulator is finally working, and I hope you enjoy it!
