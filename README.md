# CHIP-8-emu
My first attempt with emulation. It was focused in the CHIP-8 Virtual Machine, a 8-bit CPU, originally developed in the 70's by Joseph Weisbecker.

![](./img/pong.png)

### Prerequisites

* installed [__OpenGL__](https://www.khronos.org/opengl/wiki/Getting_Started) Library.

### Building
```
~$ git clone https://github.com/tonisidneimc/CHIP-8-emu
~$ cd CHIP-8-emu
~$ make
```
### Running

> usage: **make run [scale] [delay] [ROM]**

The default (recommended) values for the scale and delay are 10 and 17, respectively.

### Example

```
~$ make run 10 10 tests/pong2.ch8
```

## CHIP-8 Virtual Machine specs

##### Memory Map

​	The original CHIP-8 machine has **4096** (0x1000) 8-bit memory locations, of which:

- **[0x000 - 0x1FF]** - reserved space for the CHIP-8 interpreter, or to store font data, in most modern CHIP-8 implementations.
- **[0x200 - 0xE9F]** - available memory space to be used by programs.
- **[0xEA0 - 0xEFF]** - reserved for the call stack, internal use and other variables.
- **[0xF00 - 0xFFF]** - reserved for display refresh.

##### Registers

​	CHIP-8 has 16 8-bit data registers (**V0 - VF**). The VF register is used as a flag for some instructions (**carry flag** for addition, **"no borrow"** flag for subtraction and **pixel collision** flag for the draw instruction). In addition to these 16 registers, the CHIP-8 has an address register, named **I**, that is 16-bits wide, and is used with several instructions that involve memory operations.

##### The stack

​	The stack is only used to store return address when subroutines are called. The original CHIP-8 machine uses 48 bytes for up to 12 levels of nesting. In this emulator, it uses 32 bytes for at most **16 levels of nesting**.

##### Timers

​	CHIP-8 has 2 timers. They both count down at 60Hz, until they reach 0.

- delay timer: is used for timing the events of games.
- sound timer: as the name says, it is used for sound effects.

##### Input

​	Input is done with a hex keyboard that maps **16 keys**, ranging from 0 to F. The '8', '4', '6' and '2' keys are typically used for directional input. Three opcodes are used to detect input, waiting for a key press, or not, and then storing it value in one of the data registers.

##### Graphics

​	Original CHIP-8 display is **monochrome** and has a resolution of **64x32** pixels. Each sprite drawing to the screen is 8 bits wide and may be from 1 to 15 pixels in height. Sprite pixels that are set flip the color of the corresponding screen pixel, while unset sprite pixels do nothing.

##### Opcodes

​	CHIP-8 has **35** opcodes, wich are all two bytes long and stored big-endian. The chip8 opcode table can be consulted at the web.