#include<GL/glut.h>
#include<iostream>
#include <math.h>
#include<windows.h>
#include<vector>
#include<ctime>
#include<map>
using namespace std;

int score = 0;
int ptsz = 43;
bool lt = false;
bool rt = false;
bool down = false;
bool r = false;
int y;
float rd, gr, bl;

// This map will store the actual grid. how the blocks are layed out.
map<pair<int, int>, vector<float>> grid;

int ht[9] = { 50,50,50,50,50,50,50,50,50 }; //width: 50*9 = 450
int colCount = 8; //total colors

// RGB making the 8 colors like (red[2],green[2],blue[2]) will make one color. index, in this case 2, should be constant for all.
double red[8] = { (double)230 / 256,(double)60 / 256,(double)255 / 256,(double)0 / 256,(double)245 / 256,(double)145 / 256,(double)70 / 256,(double)240 / 256 };
double green[8] = { (double)25 / 256,(double)180 / 256,(double)225 / 256,(double)130 / 256,(double)130 / 256,(double)30 / 256,(double)240 / 256,(double)50 / 256 };
double blue[8] = { (double)75 / 256,(double)75 / 256,(double)25 / 256,(double)200 / 256,(double)48 / 256,(double)180 / 256,(double)240 / 256,(double)230 / 256 };


int tow, towi, col;
int blockNo = 0;
bool newBlock = true;
bool gameOver = false;

//Function Declaration
void checkSurroundings(int[], float, float, float, int, int, int);


//newblock
void drawCurrentBlock() {
	if (gameOver)//check if game is over
		return;

	if (newBlock) { //if new block is true
		cout << "Score = " << score << endl;
		cout << "drawing new block\n";
		blockNo++; // increment block number
		y = 600; // y coord where the new block will start to drop
		srand((unsigned)time(0) + blockNo);
		col = rand() % colCount;//choose a color index randomly
		towi = rand() % 9; // where to send the block from the x axis
		tow = (towi + 1) * 45;

		//taking colors from the global arrays of rgb.
		rd = red[col], gr = green[col], bl = blue[col];

		cout << tow << " " << y << endl;
		cout << rd << " " << gr << " " << bl << endl;
		
		// create the block
		glBegin(GL_POINTS);
		glColor3f(rd, gr, bl);
		glVertex2f(tow, y);
		glEnd();
		glFlush();
		newBlock = false;
	}
	else {
		//cout << "drawing current block\n";
		//cout << towi << " " << y << endl;
		//cout << rd << " " << gr << " " << bl << endl;
		tow = (towi + 1) * 45;
		glBegin(GL_POINTS);
		glColor3f(rd, gr, bl);
		glVertex2f(tow, y);
		glEnd();
		glFlush();
	}
}

void drawGrid() {
	//cout << "drawing grid\n";
	if (gameOver)
		return;
	glBegin(GL_POINTS);
	for (int i = 45; i < 450; i += 45) {
		for (int j = 50; j < 600; j += 45) {
			float rd1 = grid[{i, j}][0];
			float gr1 = grid[{i, j}][1];
			float bl1 = grid[{i, j}][2];
			glColor3f(rd1, gr1, bl1);
			glVertex2f(i, j);
		}
	}
	glEnd();
	glFlush();
}


// Used to delete the current matched block if they are side by side
void shiftDownByOne(int ht[], int towi, int tow, int y) {
	cout << endl << "shiftDownByOne" << endl;

	float color1[3];
	for (int i = y; i < ht[towi]; i += 45) {
		glReadPixels(tow, i + 45, 1.0, 1.0, GL_RGB, GL_FLOAT, color1);
		grid[{tow, i}] = { color1[0], color1[1], color1[2] };
		if (color1[0] == 0 && color1[1] == 0 && color1[2] == 0)
			break;
	}

	drawGrid();

	for (int i = y; i < ht[towi]; i += 45) {
		glReadPixels(tow, i, 1.0, 1.0, GL_RGB, GL_FLOAT, color1);
		if (color1[0] == 0 && color1[1] == 0 && color1[2] == 0)
			break;
		checkSurroundings(ht, color1[0], color1[1], color1[2], towi, tow, i);
	}
}

// Used to delete the current matched block if they are on top and bottom
void shiftDownByTwo(int ht[], int towi, int tow, int y) {
	cout << endl << "shiftDownByTwo" << endl;
	float color1[3];
	int count = 0;
	for (int i = y; i < ht[towi]; i += 45) {
		glReadPixels(tow, i + 2 * 45, 1.0, 1.0, GL_RGB, GL_FLOAT, color1);
		grid[{tow, i}] = { color1[0], color1[1], color1[2] };
		if (color1[0] == 0 && color1[1] == 0 && color1[2] == 0) {
			count++;
			if (count == 2)
				break;
		}
	}

	drawGrid();

	for (int i = y; i < ht[towi]; i += 45) {
		glReadPixels(tow, i, 1.0, 1.0, GL_RGB, GL_FLOAT, color1);
		if (color1[0] == 0 && color1[1] == 0 && color1[2] == 0)
			break;
		checkSurroundings(ht, color1[0], color1[1], color1[2], towi, tow, i);
	}
}

//checks if the color of the bottom block is the same as the current block
int checkBottom(int ht[], float rd, float gr, float bl, int towi, int tow, int y) {
	if (y > 51) {
		float color[3];
		glReadPixels(tow, y - 45, 1.0, 1.0, GL_RGB, GL_FLOAT, color);

		color[0] = (int)(color[0] * 100);
		color[1] = (int)(color[1] * 100);
		color[2] = (int)(color[2] * 100);

		if (color[0] == (int)(rd * 100) && color[1] == (int)(gr * 100) && color[2] == (int)(bl * 100)) {
			cout << "bottom matched\n";
			score++;
			shiftDownByTwo(ht, towi, tow, y - 45);
			ht[towi] -= 45;
			/*if (ht[towi] < 50)
				ht[towi] = 50;*/
			return 1;
		}
	}
	return 0;
}

//checks if the color of the Right block is the same as the current block
int checkRight(int ht[], float rd, float gr, float bl, int towi, int tow, int y) {
	if (towi > 9) {
		return 0;
	}
	float color[3];
	glReadPixels(tow + 45, y, 1.0, 1.0, GL_RGB, GL_FLOAT, color);

	color[0] = (int)(color[0] * 100);
	color[1] = (int)(color[1] * 100);
	color[2] = (int)(color[2] * 100);

	if (color[0] == (int)(rd * 100) && color[1] == (int)(gr * 100) && color[2] == (int)(bl * 100)) {
		score++;
		cout << "right matched\n";
		shiftDownByOne(ht, towi, tow, y);
		shiftDownByOne(ht, towi + 1, tow + 45, y);
		ht[towi + 1] -= 45;
		/*if (ht[towi + 1] < 50)
			ht[towi + 1] = 50;*/
		return 1;
	}
	return 0;
}

//checks if the color of the Left block is the same as the current block
int checkLeft(int ht[], float rd, float gr, float bl, int towi, int tow, int y) {
	if (towi < 0) {
		return 0;
	}
	float color[3];
	glReadPixels(tow - 45, y, 1.0, 1.0, GL_RGB, GL_FLOAT, color);

	color[0] = (int)(color[0] * 100);
	color[1] = (int)(color[1] * 100);
	color[2] = (int)(color[2] * 100);

	if (color[0] == (int)(rd * 100) && color[1] == (int)(gr * 100) && color[2] == (int)(bl * 100)) {
		cout << "left matched\n";
		score++;
		shiftDownByOne(ht, towi, tow, y);
		shiftDownByOne(ht, towi - 1, tow - 45, y);
		ht[towi - 1] -= 45;
		if (ht[towi - 1] < 50)
			ht[towi - 1] = 50;
		return 1;
	}
	return 0;
}

void checkSurroundings(int ht[], float rd, float gr, float bl, int towi, int tow, int y) {
	int bt = checkBottom(ht, rd, gr, bl, towi, tow, y);
	int rt = checkRight(ht, rd, gr, bl, towi, tow, y);
	int lt = checkLeft(ht, rd, gr, bl, towi, tow, y);

	if (bt || rt || lt) {
		score++;
		ht[towi] -= 45;
	}
}


// falling the block and slightly incrementing it as per block number
void drop() {
	if (y > ht[towi]) {
		y--;
		for (int i = 0; i < 10000 - blockNo * 100; i++)
			Sleep(0.9);
		glutPostRedisplay();
	}
}

// registers key input to global variables
void specialInput(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_DOWN:
		//do something here
		//cout << "from SpecialInput: down pressed\n";
		down = true;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		//do something here
		//cout << "from SpecialInput: left pressed\n";
		lt = true;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		//do something here
		//cout << "from SpecialInput: right pressed\n";
		rt = true;
		glutPostRedisplay();
		break;
	case GLUT_KEY_HOME:
		//cout << "from SpecialInput: home pressed\n";
		r = true;
		glutPostRedisplay();
		break;
	}
}

//reset game by pressing home
void restart() {
	for (int i = 45; i < 450; i += 45) {
		ht[i / 45 - 1] = 50;
		for (int j = 50; j < 600; j += 45) {
			grid[{i, j}] = { 0.0,0.0,0.0 };
		}
	}
}

void game() {
	glClear(GL_COLOR_BUFFER_BIT);
	drawGrid();
	drawCurrentBlock();

	//moves block right
	if (rt) {
		if (towi < 8 && ht[towi + 1] < y) {
			towi++;
			tow += 45;
		}
		rt = false;
	}

	//moves block left
	if (lt) {
		if (towi > 0 && ht[towi - 1] < y) {
			towi--;
			tow -= 45;
		}
		lt = false;
	}

	//moves block down
	if (down) {
		y -= 45;
		if (y < ht[towi]) {
			y = ht[towi];
		}
		down = false;
	}

	//check for home key and if pressed restart the game
	if (r) {
		cout << "\nDo you want to restart the game? [y/n]\n";
		char ans;
		cin >> ans;
		if (ans == 'y') {
			cout << "\n\nRestarting the game.\n";
			restart();
			newBlock = true;
			score = 0;
			blockNo = 0;
		}
		r = false;
	}

	if (y == ht[towi]) {
		cout << "\nprev tower height = " << ht[towi] << endl;
		cout << "block dropped\n";
		grid[{tow, ht[towi]}] = { rd,gr,bl }; // add block to grid
		drawGrid();
		ht[towi] += 45;
		checkSurroundings(ht, rd, gr, bl, towi, tow, y);
		cout << "new tower height = " << ht[towi] << endl << endl;
		newBlock = true;
		glutPostRedisplay();
	}

	//check if height of tower exeeds window pixel height and game is not over
	if (ht[towi] > 600 && !gameOver) {
		cout << "Game Over!\n";
		cout << "Score = " << score << endl;
		glBegin(GL_POINTS);
		
		//game is over so draw all the blocks white
		for (int i = 45; i < 450; i += 45) {
			for (int j = 50; j < 600; j += 45) {
				if (!(grid[{i, j}][0] == 0.0 && grid[{i, j}][1] == 0.0 && grid[{i, j}][2] == 0.0)) {
					glColor3f(1.0, 1.0, 1.0);
					glVertex2f(i, j);
				}
			}
		}
		glEnd();
		glFlush();
		gameOver = true; //mark gameOver true
	}
}

void myInit() {
	//initializing grid to black color string
	for (int i = 45; i < 450; i += 45) {
		for (int j = 50; j < 600; j += 45) {
			grid[{i, j}] = { 0.0,0.0,0.0 };
		}
	}

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glPointSize(ptsz);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 450, 0.0, 600);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(450, 600); // size of window
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Color Pop!"); // game title
	glutDisplayFunc(game);
	glutSpecialFunc(specialInput);
	glutIdleFunc(drop);
	myInit();
	glutMainLoop();
}