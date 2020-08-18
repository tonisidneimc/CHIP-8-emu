#include <stdio.h>
#include <stdlib.h> //rand and srand
#include <string.h> //memset
#include <time.h> //time
#include "chip8.h"

#define START_ADDRESS 0x200
#define FONTSET_START_ADDRESS 0x50
#define WORD_SZ (sizeof(uint8_t))
#define FONTSET_SZ 80

CHIP8 emu = {
	.regs   = {0x00},
	.memory = {0x00},
	.stack  = {0x00},
	.video  = {0x00},
	.keypad = {0x00},

	.I      = 0x00,
	.PC     = 0x200,
	.SP     = 0x00,
	
	.opcode = 0x00,
	
	.delayTimer = 0x00,
	.soundTimer = 0x00
};

#define memory emu.memory
#define registers emu.regs
#define stack emu.stack
#define opcode emu.opcode

typedef void(*funct)();

static uint8_t fontset[] = {
	
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

funct table[] = {
	Table0, 
	OP_1nnn, OP_2nnn,
	OP_3xkk, OP_4xkk,
	OP_5xy0,
	OP_6xkk, OP_7xkk,
	Table8,
	OP_9xy0,
	OP_Annn, OP_Bnnn,
	OP_Cxkk, 
	OP_Dxyn,
	TableE,
	TableF
};

funct table0[] = {
	OP_00E0,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_00EE
};

funct table8[] = {
	OP_8xy0, OP_8xy1, 
	OP_8xy2, OP_8xy3, 
	OP_8xy4, OP_8xy5,
	OP_8xy6, OP_8xy7,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_8xyE
};

funct tableE[] = {
	OP_NULL, 
	OP_ExA1,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_Ex9E	
};

funct tableF[] = {
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_Fx07, OP_NULL, //7
	OP_NULL, OP_Fx0A, OP_NULL, //10
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL, 
	OP_Fx15, OP_NULL, OP_NULL, //21
	OP_Fx18, OP_NULL, OP_NULL, //24
	OP_NULL, OP_NULL, OP_NULL,
	OP_Fx1E, OP_NULL, OP_NULL, //30
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_Fx29, //41
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_Fx33, OP_NULL, OP_NULL, //51
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_Fx55, OP_NULL, //85
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_NULL,
	OP_NULL, OP_NULL, OP_Fx65 //101
};

static void Table0(){
	(*(table0[opcode & 0x000Fu]))();
}

static void Table8(){
	(*(table8[opcode & 0x000Fu]))();
}

static void TableE(){
	(*(tableE[opcode & 0x000Fu]))();
}

static void TableF(){
	(*(tableF[opcode & 0x00FFu]))();
}

static void OP_NULL(){}

static void OP_00E0(){
	//CLS : clear the screen
	memset(emu.video, 0, sizeof(emu.video));
}

static void OP_00EE(){
	//RET : return from a subroutine
	--emu.SP;
	emu.PC = stack[emu.SP];
}

static void OP_1nnn(){
	//JP NNN : Jump to address NNN
	
	//get the 12 less significant bits of opcode
	uint16_t address = opcode & 0x0FFFu;
	
	emu.PC = address;
}

static void OP_2nnn(){
	//CALL NNN: call subroutine starting at address NNN	
		
	uint16_t address = opcode & 0x0FFFu;
	
	//save current context at the top of the stack
	stack[emu.SP] = emu.PC;
	++emu.SP;
	//update emu.PC
	emu.PC = address;
}

static void OP_3xkk(){
	//SE Vx, NN : Skip the following instruction if the value of register VX equals NN
	
	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	
	if(registers[Vx] == byte){
		emu.PC += 2;
	}
}
            
static void OP_4xkk(){
	//SNE Vx, NN : Skip the following instruction if the value of register VX is not equal to NN
	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	
	if(registers[Vx] != byte){
		emu.PC += 2;
	}
}

static void OP_5xy0(){
	//SE Vx, Vy : Skip the following instruction if the value of register VX is equal to the value of register VY
	
	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	
	if(registers[Vx] == registers[Vy]){
		emu.PC += 2;
	}
}

static void OP_6xkk(){
	//LD Vx, NN : store number NN in register Vx
	
	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	
	registers[Vx] = byte;
}

static void OP_7xkk(){
	//ADD Vx, NN : Add the value NN to register Vx
	
	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] += byte;
}

static void OP_8xy0(){
	//LD Vx, Vy : store the value of register Vy in register Vx
	
	//get operands 
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	
	registers[Vx] = registers[Vy];
}

static void OP_8xy1(){
	//OR Vx, Vy : Set VX to VX OR VY
	
	//get operands 
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	
	registers[Vx] |= registers[Vy];
}

static void OP_8xy2(){
	//AND Vx, Vy : Set VX to VX AND VY
	
	//get operands 
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	
	registers[Vx] &= registers[Vy];
}

static void OP_8xy3(){
	//XOR Vx, Vy : Set VX to VX XOR VY
	
	//get operands 
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	
	registers[Vx] ^= registers[Vy];
}

static void OP_8xy4(){
	//ADD Vx, Vy : Add the value of register VY to register VX
	
	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	
	//sum two 8 bits values can overflow
	uint16_t sum = registers[Vx] + registers[Vy];
	
	//check overflow, set VF register if true
	registers[0xF] = (sum > 255U ? 1 : 0); //*********//
	
	//select only the lowest 8 bits of sum
	registers[Vx] = sum & 0xFFu;
}

static void OP_8xy5(){
	//SUB Vx, Vy : Subtract the value of register VY from register VX

	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	
	//check if not borrow
	registers[0xF] = (registers[Vx] > registers[Vy] ? 1 : 0);
	
	registers[Vx] -= registers[Vy];
}

static void OP_8xy6(){
	//SHR Vx : Store the value of register VY shifted right one bit in register VX
	
	//get operand	
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	//set carry out
	registers[0xF] = (registers[Vx] & 0x1u);
		
	registers[Vx] >>= 1;
}

static void OP_8xy7(){
	//SUBN Vx, Vy : Set register VX to the value of VY minus VX
	
	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	
	//check if not borrow
	registers[0xF] = (registers[Vy] > registers[Vx] ? 1 : 0);
	
	registers[Vx] = registers[Vy] - registers[Vx];
}

static void OP_8xyE(){
	//SHL Vx : Store the value of register VY shifted left one bit in register VX
	
	//get operand
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	
	//set carry out
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;
}

static void OP_9xy0(){
	//SNE Vx, Vy : Skip the following instruction if the value of register VX is not equal to the value of register VY
	
	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	uint8_t Vy = (opcode & 0x00F0u) >> 4;
	
	if(registers[Vx] != registers[Vy]){
		emu.PC += 2;
	}
}

static void OP_Annn(){
	//LD I, nnn : Store memory address NNN in register I
	
	//get operand
	uint16_t address = opcode & 0x0FFFu;
	
	emu.I = address;
}

static void OP_Bnnn(){
	//JP V0, NNN : Jump to address NNN + V0
		
	//get operand
	uint16_t address = opcode & 0x0FFFu;
	
	emu.PC = registers[0] + address;	
}

static void OP_Cxkk(){
	//RND Vx, NN : Set VX to a random number with a mask of NN
	
	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	
	registers[Vx] = (rand() % 256) & byte;
}

static void OP_Dxyn(){
	//DRW Vx, Vy, height : Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I
	//each sprite drawing has a variable height, and a width of 8 bits
	
	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;
	
	uint8_t xScreen = registers[Vx] % VIDEO_WIDTH;
	uint8_t yScreen = registers[Vy] % VIDEO_HEIGHT; 
	
	registers[0xF] = 0; //assume no collisions in the beginning 
	
	//traverse through the sprite
	for(unsigned int row = 0; row < height; ++row){
		uint8_t sprite = memory[emu.I + row];
					
		for(unsigned int col = 0; col < 8; ++col){
			//select each bit of the sprite
			uint8_t spritePixel = sprite & (0x80u >> col);
			
			if((yScreen + row) * VIDEO_WIDTH + (xScreen + col) > 2047) continue;
				
			//do a conversion from Screen coordinates(2D) to memory array(1D)				
			uint32_t* displayPixel = &(emu.video[(yScreen + row) * VIDEO_WIDTH + (xScreen + col)]);
			
			//pixel is on
			if(spritePixel){
				if(*displayPixel == 0xFFFFFFFF){
					//pixel collision					
					registers[0xF] = 1;
				}
				//spritePixel(OFF) ^ displayPixel(OFF) = displayPixel(OFF)
				//spritePixel(ON)  ^ displayPixel(OFF) = displayPixel(ON)
				//spritePixel(OFF) ^ displayPixel(ON) = displayPixel(ON)
				//spritePixel(ON)  ^ displayPixel(ON) = displayPixel(ON) 
				*displayPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

static void OP_Ex9E(){
	//SKP Vx : Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed

	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	
	//get key at register Vx
	uint8_t key = registers[Vx];
	
	if(emu.keypad[key]){
		emu.PC += 2;
	}
}

static void OP_ExA1(){
	//SKNP Vx : Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
	
	uint8_t Vx = (opcode & 0x0F00u) >> 8;
	
	//get key at register Vx
	uint8_t key = registers[Vx];

	if(!emu.keypad[key]){
		emu.PC += 2;
	}
}

static void OP_Fx07(){
	//LD Vx, DT : Store the current value of the delay timer in register VX
	
	//get operand
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	
	registers[Vx] = emu.delayTimer; 
}

static void OP_Fx0A(){
	//LD Vx, K : Wait for a keypress and store the result in register VX
	
	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	
	for(uint8_t key = 0; key < 16; ++key){
		if(emu.keypad[key] == 1){
			registers[Vx] = key;
			return;
		}
	}
	emu.PC -= 2;
}

static void OP_Fx15(){
	//LD DT, Vx : Set the delay timer to the value of register VX
	
	//get operand
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	
	emu.delayTimer = registers[Vx];
}

static void OP_Fx18(){
	//LD ST, Vx : Set the sound timer to the value of register VX
	
	//get operand
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	
	emu.soundTimer = registers[Vx];
}

static void OP_Fx1E(){
	//ADD I, Vx : Add the value stored in register VX to register I
	
	//get operand
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	
	emu.I += registers[Vx];
}

static void OP_Fx29(){
	//LD F, Vx : Set I to the memory address of the sprite data corresponding to the hexadecimal digit stored in register VX 
	
	//get operands	
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = registers[Vx];
	
	emu.I = FONTSET_START_ADDRESS + (5 * digit);
}

static void OP_Fx33(){
	//LD B, Vx : Store the binary-coded decimal equivalent of the value stored in register VX at addresses I, I+1, and I+2
	
	//get operands
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];
	
	memory[emu.I + 2] = value % 10;
	value /= 10;
	
	memory[emu.I + 1] = value % 10;
	value /= 10;
	
	memory[emu.I] = value % 10; 
}

static void OP_Fx55(){
	//LD [I], Vx : Store the values of registers V0 to VX inclusive in memory starting at address I
	
	//get operand
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	
	for(uint8_t i = 0; i <= Vx; ++i){
		
		memory[emu.I + i] = registers[i];
	}
}

static void OP_Fx65(){
	//LD Vx, [I] : Fill registers V0 to VX inclusive with the values stored in memory starting at address I  
	
	//get operand
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	
	for(uint8_t i = 0; i <= Vx; ++i){
	
		registers[i] = memory[emu.I + i];
	}
}

void initPlatform(){
	emu.PC = START_ADDRESS;
		
	for(unsigned int i = 0; i < FONTSET_SZ; ++i){
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}
	srand(time(NULL));
}

void cycle(){
	//a CHIP8 instruction is 16-bit length,
	//so it fetches 2 bytes from the memory
	opcode = (memory[emu.PC] << 8u) | memory[emu.PC + 1];
	
	//increment emu.PC to the next instruction address
	emu.PC += 2;
	
	//decode and execute instruction
	(*(table[(opcode & 0xF000u) >> 12u]))();
	
	if(emu.delayTimer > 0){
		--emu.delayTimer;
	}
	if(emu.soundTimer > 0){
		if(emu.soundTimer == 1){
			printf("BEEEP!\n");
		}
		--emu.soundTimer;
	}
}

uint32_t* gfx(){
	return emu.video;
}

uint8_t* keypd(){
	return emu.keypad;
}

void loadROM(const char* path){
	FILE* file = fopen(path, "rb");	
	
	if(!file){
		perror("Error");
		//fprintf(stderr, "Error: Could not open file '%s'.\n", path);
		exit(EXIT_FAILURE);
	}	
	
	//get the file size in bytes
	fseek(file, 0, SEEK_END);
	size_t sz = ftell(file);
	
	//move pointer to the beginning of the file
	rewind(file);
	
	char* buffer = memory + (START_ADDRESS * WORD_SZ);
	//load ROM's content to the CHIP-8 memory, starting at address 0x200
	size_t bytesRead = fread(buffer, WORD_SZ, sz, file);
	
	if(bytesRead != sz){	
		fprintf(stderr, "Error: Could not read the ROM file.\n");
		exit(EXIT_FAILURE);
	}	
	
	fclose(file);
}
