#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "chip8.h"

#ifdef _WIN32
    #include "libs/glut.h"
    #include <windows.h>
#elif __APPLE__
    #include <GLUT/glut.h>
#elif __unix__      
    #include <GL/glut.h>
#endif

GLfloat scale = 10;
int cycleDelay;

uint8_t* keypad;
uint32_t* video;

uint8_t	scrBuffer[VIDEO_HEIGHT][VIDEO_WIDTH][3] = {0x00};

void initGL(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDisable(GL_DEPTH_TEST);
}

void update(){
	//reshape and update texture
	glClear(GL_COLOR_BUFFER_BIT);	
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	GLfloat w = glutGet(GLUT_WINDOW_WIDTH);
	GLfloat h = glutGet(GLUT_WINDOW_HEIGHT);
	scale = w/h; //aspect ratio
	
	gluOrtho2D(0, VIDEO_WIDTH * scale, VIDEO_HEIGHT * scale, 0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnableClientState(GL_VERTEX_ARRAY);	
	
	for(GLint y = 0; y < 32; ++y){		
		for(GLint x = 0; x < 64; ++x){
			if(video[(y*64) + x] == 0){
				glColor3f(0.0f,0.0f,0.0f); //black
			}else{
				glColor3f(1.0f,1.0f,1.0f); //white		
			}
			GLfloat vertices[] = {
				x * scale, y * scale,
				x * scale, y * scale + scale,
				x * scale + scale, y * scale + scale,
				x * scale + scale, y * scale
			};
			glVertexPointer(2, GL_FLOAT, 0, vertices);
			glDrawArrays(GL_QUADS, 0, 4);
		}
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}

clock_t previousTime; //last cycle time
clock_t currentTime;

void display(){
	currentTime = clock();
	
	if(currentTime - previousTime > cycleDelay){
		previousTime = currentTime;
				
		cycle();

		if(drawF){
			update();		
			glFlush();
				
			glutSwapBuffers();
			drawF = 0;
		}
	}	
}

void keyDown(GLubyte key, GLint x, GLint y);
void keyUp(GLubyte key, GLint x, GLint y);

int main(int argc, char* argv[]){
	if(argc != 4){
		fprintf(stderr, "Usage: %s <scale> <delay> <ROM>\n", argv[0]);
		exit(EXIT_FAILURE);
	}		
	scale = atoi(argv[1]);
	cycleDelay = atoi(argv[2]);	

	const char* romFile = argv[3];
	initPlatform();
	loadROM(romFile);	
	
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(VIDEO_WIDTH * scale, VIDEO_HEIGHT * scale);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("CHIP8 Emulator");		
		
	initGL();
	
	video = gfx();
	keypad = keypd();
	
	glutDisplayFunc(display);
	glutIdleFunc(display);
	
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);	
	
	previousTime = clock();
	glutMainLoop();
	
	return 0;
}

void keyDown(GLubyte key, GLint x, GLint y){
	
	if(key == 27) exit(0); //ESC

	switch(key){
		case '1':
			keypad[0x1] = 1; break;
		case '2':
			keypad[0x2] = 1; break;
		case '3':
			keypad[0x3] = 1; break;		
		case '4':
			keypad[0xC] = 1; break;
		case 'q':
			keypad[0x4] = 1; break;
		case 'w':
			keypad[0x5] = 1; break;
		case 'e':
			keypad[0x6] = 1; break;
		case 'r':
			keypad[0xD] = 1; break;
		case 'a':
			keypad[0x7] = 1; break;
		case 's':
			keypad[0x8] = 1; break;
		case 'd':
			keypad[0x9] = 1; break;
		case 'f':
			keypad[0xE] = 1; break;
		case 'z':
			keypad[0xA] = 1; break;
		case 'x':
			keypad[0x0] = 1; break;
		case 'c':
			keypad[0xB] = 1; break;
		case 'v':
			keypad[0xF] = 1; break;
	}
}

void keyUp(GLubyte key, GLint x, GLint y){
	
	switch(key){
		case '1':
			keypad[0x1] = 0; break;
		case '2':
			keypad[0x2] = 0; break;
		case '3':
			keypad[0x3] = 0; break;		
		case '4':
			keypad[0xC] = 0; break;
		case 'q':
			keypad[0x4] = 0; break;
		case 'w':
			keypad[0x5] = 0; break;
		case 'e':
			keypad[0x6] = 0; break;
		case 'r':
			keypad[0xD] = 0; break;
		case 'a':
			keypad[0x7] = 0; break;
		case 's':
			keypad[0x8] = 0; break;
		case 'd':
			keypad[0x9] = 0; break;
		case 'f':
			keypad[0xE] = 0; break;
		case 'z':
			keypad[0xA] = 0; break;
		case 'x':
			keypad[0x0] = 0; break;
		case 'c':
			keypad[0xB] = 0; break;
		case 'v':
			keypad[0xF] = 0; break;
	}
}
