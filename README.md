# Chip8C
This is, unsurprisingly, a Chip-8 emulator written entirely in C! It takes a path to any Chip-8 binary and interprets it, rendering the output display with SDL2.
## Features
* Loading and executing Chip-8 binaries
* Crt-esque display (pixels turned off fade out over time)
* Options to turn on quirks for the COSMAC VIP or the AMIGA
## Download

## Building Chip8C Yourself
### Requirements
You'll need ``gcc``,`make`, and the [SDL2 development libraries](https://www.libsdl.org/) installed.
#### Arch
```bash
sudo pacman -S sdl2-compat
```
#### Debian / Ubuntu
```bash
sudo apt install build-essential libsdl2-dev libsdl2-image-dev
```
#### MacOs (Via Homebrew)
```bash
brew install sdl2 sdl2_image
```
#### Fedora / Red Hat
```bash
sudo dnf groupinstall "Development Tools"
sudo dnf install SDL2-devel SDL2_image-devel
```
#### Windows
Ima be real for this one you're gonna use MSYS2 or write your own makefile <3

### Compilation
Run `make` from the root of the project directory. The binary will appear in the automatically generated `build` folder.

## Usage
To load and execute a binary with Chip8C, simply run your compiled chip8 executable with the first argument as a path to your target binary.
```
$./build/chip8 <path_to_binary>
```
COSMAC mode and AMIGA mode can be enabled by appending the arguments ``-c`` and ``-a`` respectively. Both may be enabled simultaneously, although I can't imagine a scenario where you'd need it :3
```
$./chip8 <path_to_binary> -c -a
```
The emulator will create a window in the middle of your screen; you can use your computer's keyboard to interact with it. Chip8 takes one key down/up input for each number 0-F in hexadecimal, which I have mapped as such:
```
(physical key -> emulated key, organized  by keyboard row/numeric order)
1,2,3   ->   1,2,3
Q,W,E   ->   4,5,6
A,S,D   ->   7,8,9
Z,C     ->     A,B
4,R,F,V -> C,D,E,F
X       ->       0
```