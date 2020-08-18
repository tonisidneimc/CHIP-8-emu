#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define VIDEO_HEIGHT 32
#define VIDEO_WIDTH 64

typedef struct{
	
	uint8_t regs[20]; //V0 - VF registers
	uint8_t memory[4096]; //4K memory
	uint16_t stack[16];
	uint32_t video[64 * 32]; //2K gfx
	
	uint16_t I; //memory index
	uint16_t PC; //program counter
	uint8_t SP; //stack pointer
	
	uint8_t keypad[16];
	
	uint16_t opcode;
	
	uint8_t delayTimer;
	uint8_t soundTimer;

}CHIP8;

static void OP_00E0();	//CLS	
static void OP_00EE();  //RET
static void OP_1nnn();  //JP NNN
static void OP_2nnn();  //CALL NNN
static void OP_3xkk();  //SE  Vx, NN
static void OP_4xkk();  //SNE Vx, NN
static void OP_5xy0();  //SE  Vx, Vy
static void OP_6xkk();  //LD  Vx, NN
static void OP_7xkk();  //ADD Vx, NN
static void OP_8xy0();  //LD  Vx, Vy
static void OP_8xy1();  //OR  Vx, Vy
static void OP_8xy2();  //AND Vx, Vy
static void OP_8xy3();  //XOR Vx, Vy
static void OP_8xy4();  //ADD Vx, Vy
static void OP_8xy5();  //SUB Vx, Vy
static void OP_8xy6();  //SHR Vx
static void OP_8xy7();  //SUBN Vx, Vy
static void OP_8xyE();  //SHL Vx
static void OP_9xy0();  //SNE Vx, Vy
static void OP_Annn();  //LD I, NNN
static void OP_Bnnn();  //JP V0, NNN
static void OP_Cxkk();  //RND Vx, NN
static void OP_Dxyn();  //DRW Vx, Vy
static void OP_Ex9E();  //SKP Vx
static void OP_ExA1();  //SKNP Vx
static void OP_Fx07();  //LD Vx, DT
static void OP_Fx0A();  //LD Vx, K
static void OP_Fx15();  //LD DT, Vx
static void OP_Fx18();  //LD ST, Vx
static void OP_Fx1E();  //ADD I, Vx
static void OP_Fx29();  //LD F, Vx
static void OP_Fx33();  //LD B, Vx
static void OP_Fx55();  //LD [I], Vx
static void OP_Fx65();	//LD Vx, [I]

static void Table0();
static void Table8();
static void TableE();
static void TableF();
static void OP_NULL(); 

void initPlatform();
void loadROM(const char*);
void cycle();

uint8_t* keypd();
uint32_t* gfx();

#endif
