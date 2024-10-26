// Author: Radovan Bittner
// Date: 25.2.2019

#define _CRT_NO_SECURE_DEPRECATE
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <windows.h>

#define WIDTH				80
#define HEIGHT				20

#define KEY_W				119
#define KEY_A				97
#define KEY_S				115
#define KEY_D				100
#define KEY_ENTER			13

#define MENU_PLAY			0
#define MENU_LEVEL			1
#define MENU_ENEMIES		2
#define MENU_EXIT			3

#define PLAYER_STEP			5
#define PLAYER_MODEL		'O'
#define ENEMY_STEP			2
#define ENEMY_MODEL			'*'
#define MAX_ENEMIES			30
#define CATCH_DISTANCE		3

#define CLOCK				500
#define MATH_PI				3.14159265
#define TO_RADIANS(a)		(a*MATH_PI/180)
#define NEW_ANGLE(a)		(rand() % (360/a) * a)

float enemy[MAX_ENEMIES][2] = { 0 };
float player[2] = { 0 };

static char menu[4][12] =
{
	"> Play",
	"  Arena: ",
	"  Enemies: ",
	"  Exit"
};
static char menuMap[3][17] =
{
	"Large  (74x20)",
	"Medium (60x20)",
	"Small  (50x20)"
};
static char menuEnemies[4][17] =
{
	"1",
	"5",
	"15",
	"30"
};

bool ingame = false;
bool inmenu = false;
bool initialized = false;

int WIDTH_BORDER = 3;
int maxEnemies = 1;

int menu_position = 0;
int menu_position_map = 0;
int menu_position_enemy = 0;
int last_position = 0;

void gotoxy(int x, int y)
{
	COORD pos = { x, y };
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(output, pos);
}

void initMenu() {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < sizeof(menu[i]); j++) {
			printf("%c", menu[i][j]);
		}
		printf("\n");
	}
	gotoxy(sizeof(menu[menu_position]) + 1, 1);
	printf("%s", menuMap[menu_position_map]);

	gotoxy(sizeof(menu[menu_position]) + 1, 2);
	printf("%s", menuEnemies[menu_position_enemy]);
}

void menuSelecting() {
	menu[last_position][0] = ' ';
	menu[menu_position][0] = '>';
	last_position = menu_position;

	if (inmenu) system("cls");
	else gotoxy(0, 0);
	initMenu();
}

void key_control(int key) {
	if (!ingame) {
		switch (key) {
		case KEY_W: {
			menu_position = menu_position > 0 ? menu_position - 1 : 3;
			menuSelecting();
			break;
		}
		case KEY_S: {
			menu_position = menu_position < 3 ? menu_position + 1 : 0;
			menuSelecting();
			break;
		}
		case KEY_D: {
			switch (menu_position) {
			case MENU_LEVEL: {
				gotoxy(sizeof(menu[menu_position]) + 1, menu_position);
				menu_position_map = menu_position_map < 2 ? menu_position_map + 1 : 0;
				printf("%s", menuMap[menu_position_map]);

				if (menu_position_map == 0) WIDTH_BORDER = 3;
				else if (menu_position_map == 1) WIDTH_BORDER = 10;
				else if (menu_position_map == 2) WIDTH_BORDER = 15;
				break;
			}
			case MENU_ENEMIES: {
				gotoxy(sizeof(menu[menu_position]) + 1, menu_position);
				menu_position_enemy = menu_position_enemy < 3 ? menu_position_enemy + 1 : 0;
				printf("%s  ", menuEnemies[menu_position_enemy]);

				if (menu_position_enemy == 0) maxEnemies = 1;
				else if (menu_position_enemy == 1) maxEnemies = 5;
				else if (menu_position_enemy == 2) maxEnemies = 15;
				else if (menu_position_enemy == 3) maxEnemies = 30;
				break;
			}
			}
			break;
		}
		case KEY_ENTER: {
			switch (menu_position) {
			case MENU_PLAY: ingame = true;
				break;
			case MENU_EXIT: exit(1);
			}
			break;
		}
		}
	}
}

void initMap() {
	// horizontal
	gotoxy(WIDTH_BORDER, 0);
	for (int i = 0; i < WIDTH - WIDTH_BORDER * 2; i++) {
		putchar('-');
	}
	gotoxy(WIDTH_BORDER, HEIGHT - 1);
	for (int i = 0; i < WIDTH - WIDTH_BORDER * 2; i++) {
		putchar('-');
	}

	// vertical
	for (int i = 1; i <= HEIGHT - 2; i++) {
		gotoxy(WIDTH_BORDER, i);
		putchar('|');
		gotoxy(WIDTH - WIDTH_BORDER - 1, i);
		putchar('|');
	}
}

float initEntity(char _enemy, int id) {
	float arr[3] = { 0 };
	arr[0] = rand() % ((WIDTH - WIDTH_BORDER * 2) - (WIDTH_BORDER + 1) + 1) + (WIDTH_BORDER + 1);
	arr[1] = rand() % ((HEIGHT - 2) - 1) + 1;

	gotoxy(arr[0], arr[1]);
	if (_enemy == ENEMY_MODEL && id != -1) {
		enemy[id][0] = arr[0];
		enemy[id][1] = arr[1];
		putchar(ENEMY_MODEL);
	}
	else {
		if (distance(arr, enemy) > (WIDTH - WIDTH_BORDER * 2) / 2) { // difference must be at least the half of the map
			player[0] = arr[0];
			player[1] = arr[1];
			putchar(PLAYER_MODEL);
		}
		else initEntity(PLAYER_MODEL, -1);
	}
}

float distance(float vec1[], float vec2[]) {
	float u[2] = { vec2[0] - vec1[0], vec2[1] - vec1[1] };
	return sqrt(pow(u[0], 2) + pow(u[1], 2));
}

float get_new_x(float old_x, int alpha, float dist) {
	return old_x + dist * cos(TO_RADIANS(alpha));
}

float get_new_y(float old_y, int alpha, float dist) {
	return old_y + dist * sin(TO_RADIANS(alpha));
}

int get_angle(int c) {
	switch (c) {
	case KEY_W: return 270;
		break;
	case KEY_A: return 180;
		break;
	case KEY_S: return 90;
		break;
	case KEY_D: return 0;
		break;
	default: return -1;
	}
}

int get_time(int delta) {
	float time = 0;
	time = (float)CLOCK / 1000 * delta;
	return round(time);
}

void show_time(int seconds) {
	if (seconds >= 60) {
		int min = round(seconds / 60);
		int sec = seconds % 60;
		printf("%dm %ds", min, sec);
	}
	else {
		printf("%ds", seconds);
	}
}

float fit_in_map_x(float x) {
	float res = x;
	if (round(x) >= WIDTH - WIDTH_BORDER - 2) {
		res = WIDTH_BORDER + 1 + x - (WIDTH - WIDTH_BORDER - 2);
	}
	else if (round(x) <= WIDTH_BORDER) {
		res = WIDTH - WIDTH_BORDER - 2 - (WIDTH_BORDER - x);
	}
	if (round(x) == WIDTH_BORDER) {
		res = WIDTH - 1 - WIDTH_BORDER * 2;
	}
	return res;
}

float fit_in_map_y(float y) {
	float res = y;
	if (round(y) >= HEIGHT - 1) {
		res = y - (HEIGHT - 2);
	}
	else if (round(y) <= 0) {
		res = HEIGHT - 2;
	}
	return res;
}

void updateMap(float old_cords[], float cords[], char model) {
	gotoxy(round(old_cords[0]), round(old_cords[1]));
	putchar(' ');

	gotoxy(round(cords[0]), round(cords[1]));
	putchar(model);
}

void stepEnemy(float old_cords[], int a, int id) {
	float x = fit_in_map_x(get_new_x(enemy[id][0], a, ENEMY_STEP));
	float y = fit_in_map_y(get_new_y(enemy[id][1], a, ENEMY_STEP));
	float xy[2] = { x, y };
	float maxXY[2] = { x, y };

	if (distance(xy, player) < CATCH_DISTANCE * 5) {
		for (int i = 0; i < 12; i++) { // 360/30
			x = fit_in_map_x(get_new_x(enemy[id][0], i * 30, ENEMY_STEP));
			y = fit_in_map_y(get_new_y(enemy[id][1], i * 30, ENEMY_STEP));
			xy[0] = x;
			xy[1] = y;

			if (distance(xy, player) > CATCH_DISTANCE * 5) {
				break;
			}
			if (distance(xy, player) > distance(maxXY, player)) {
				maxXY[0] = x;
				maxXY[1] = y;
			}
			if (i == 11) { // last iteration
				x = maxXY[0];
				y = maxXY[1];
			}
		}
	}
	else if (x < WIDTH_BORDER + CATCH_DISTANCE) {
		x = fit_in_map_x(get_new_x(enemy[id][0], 0, ENEMY_STEP));
		y = fit_in_map_y(get_new_y(enemy[id][1], 0, ENEMY_STEP));
	}
	else if (x > (WIDTH - 1 - WIDTH_BORDER * 2) - CATCH_DISTANCE) {
		x = fit_in_map_x(get_new_x(enemy[id][0], 180, ENEMY_STEP));
		y = fit_in_map_y(get_new_y(enemy[id][1], 180, ENEMY_STEP));
	}
	else if (y < CATCH_DISTANCE + 1) {
		x = fit_in_map_x(get_new_x(enemy[id][0], 90, ENEMY_STEP));
		y = fit_in_map_y(get_new_y(enemy[id][1], 90, ENEMY_STEP));
	}
	else if (y > HEIGHT - 2 - CATCH_DISTANCE) {
		x = fit_in_map_x(get_new_x(enemy[id][0], 270, ENEMY_STEP));
		y = fit_in_map_y(get_new_y(enemy[id][1], 270, ENEMY_STEP));
	}

	enemy[id][0] = x;
	enemy[id][1] = y;
	updateMap(old_cords, enemy[id], ENEMY_MODEL);

}

void printInfo(int a, int delta, int enemies) {
	gotoxy(0, HEIGHT + 1);
	printf("Arena size: %dx%d, Game time: ", WIDTH - WIDTH_BORDER * 2, HEIGHT);
	show_time(get_time(delta));

	gotoxy(0, HEIGHT + 2);
	printf("[%c] Player: %.2f, %.2f", PLAYER_MODEL, player[0], player[1]);

	gotoxy(0, HEIGHT + 3);
	if (MAX_ENEMIES > 1) {
		printf("[%c] Enemies: %d/%d  ", ENEMY_MODEL, enemies, maxEnemies);
	}
	else {
		printf("[%c] Enemy: %.2f, %.2f, %d   ", ENEMY_MODEL, enemy[0][0], enemy[0][1], a);
	}
}

int main() {
	initMenu();
	int a = 0, delta = 0, totalEnemies = 0;
	bool catched[MAX_ENEMIES] = { false };

	srand(time(NULL));
	while (1) {
		if (!initialized && ingame) {
			system("cls");
			initMap();

			for (int i = 0; i < maxEnemies; i++) {
				initEntity(ENEMY_MODEL, i);
			}
			initEntity(PLAYER_MODEL, -1);
			totalEnemies = maxEnemies;
			initialized = true;
		}
		else if (initialized && ingame) {
			float tmp_P[2] = { player[0], player[1] };

			for (int i = 0; i < maxEnemies; i++) {
				if (catched[i] == false) {
					float tmp_E[2] = { enemy[i][0], enemy[i][1] };
					if (distance(tmp_E, tmp_P) > CATCH_DISTANCE) {
						stepEnemy(tmp_E, a, i);
						a = NEW_ANGLE(30);
					}
					else {
						catched[i] = true;
						gotoxy(tmp_E[0], tmp_E[1]);
						putchar(' ');
						totalEnemies--;
					}
				}
			}
			if (totalEnemies > 0) {
				if (kbhit()) {
					float ang = get_angle(getch());
					if (ang != -1) {
						player[0] = fit_in_map_x(get_new_x(player[0], ang, PLAYER_STEP));
						player[1] = fit_in_map_y(get_new_y(player[1], ang, PLAYER_STEP));
						updateMap(tmp_P, player, PLAYER_MODEL);
					}
				}
			}
			else {
				gotoxy(WIDTH / 2 - 10, HEIGHT + 2);
				printf("The player caught all the monsters!\n");
				break;
			}

			printInfo(a, delta++, totalEnemies);
			Sleep(CLOCK);
		}
		else {
			if (kbhit()) {
				key_control(getch());
			}
			Sleep(100);
		}
	}
	system("pause");
	return 0;
}