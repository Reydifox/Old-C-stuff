// Author: Radovan Bittner
// Date: 18.2.2019

#define _CRT_SECURE_NO_DEPRECATE 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <math.h>

#define TRACK_LENGTH			400 // meters
#define TRAP_DISTANCE			50 // meters
#define TRAP_COUNT				7 // (TRACK_LENGTH / TRAP_DISTANCE) - 1	
#define WIDTH					74 // console width
#define CLOCK_TIME				100 // ms
#define TOLERANCE

#define SCREEN_RATIO(x)			(10+round(x*(WIDTH-10)/TRACK_LENGTH)) // int meters
#define CMps_to_KMph(cm)		(cm * 0.036f)

int get_speed(int v_old)
{
	int speed = 0;
	if (v_old == 0)
	{
		speed = rand() % (450 - 350 + 1) + 350;
	}
	else {
		speed = v_old + (rand() % 25) - 5;
	}
	return speed;
}

void gotoxy(int x, int y)
{
	COORD pos = { x, y };
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(output, pos);
}

// distance v cm
int GetLastCollision(int distance)
{
	float dist = distance / 100;
	for (int i = TRAP_COUNT; i > 0; i--) {
		if (dist >= TRAP_DISTANCE * i) {
			return SCREEN_RATIO(TRAP_DISTANCE * i);
		}
	}
	return 0;
}

void initPlayer(int pos) {
	gotoxy(0, pos);
	printf("Runner%d >>(-", pos + 1);
	for (int i = 1; i <= TRAP_COUNT; i++) {
		gotoxy(SCREEN_RATIO(TRAP_DISTANCE * i), pos);
		putchar('|');

		gotoxy(SCREEN_RATIO(TRAP_DISTANCE * i) - 1, 2);
		printf("%dm", TRAP_DISTANCE * i);
	}
	gotoxy(SCREEN_RATIO(TRACK_LENGTH) + 2, pos);
	printf("-)");
}

void AddInMap(int runner[], int line, int collision[]) {
	gotoxy(SCREEN_RATIO(round(runner[1] / 100)), line);
	printf("--");

#ifdef TOLERANCE
	if (!((int)round(runner[1] / 100) % TRAP_DISTANCE)) {
#else
	if (!(runner[1] % (TRAP_DISTANCE * 100))) {
#endif
		if ((int)round(runner[1] / 100) != TRACK_LENGTH) {
			gotoxy(SCREEN_RATIO(round(runner[1] / 100)), line);
			runner[0] = get_speed(0);
			if (!line) collision[0]++;
			else collision[1]++;
			putchar('x');
		}
	}
	else if (GetLastCollision(runner[1]) != 0) {
		gotoxy(GetLastCollision(runner[1]), line);
		putchar('|');
	}
	}

void showProgress(int b1[], int b2[], int time) {
	gotoxy(0, 4);
	printf("Runner1: %d cm/s (%.2f km/h), %.2f m", b1[0], CMps_to_KMph(b1[0]), (float)(b1[1]) / 100);

	gotoxy(0, 5);
	printf("Runner2: %d cm/s (%.2f km/h), %.2f m", b2[0], CMps_to_KMph(b2[0]), (float)(b2[1]) / 100);

	gotoxy(0, 6);
	printf("Time %d s", time);
}

void printStats(float speed[], int collision[], int iterations) {
	float v1 = speed[0] / iterations; // average velocity
	float v2 = speed[1] / iterations;
	printf("\n---Statistics---\n");
	printf("Runner1: \n\taverage velocity: %.2f cm/s (%.2f km/h)\n\thits: %d\n", v1, CMps_to_KMph(v1), collision[0]);
	printf("Runner2: \n\taverage velocity: %.2f cm/s (%.2f km/h)\n\thits: %d\n", v2, CMps_to_KMph(v2), collision[1]);
}

void printTrophy() {
	printf("  .__.\n");
	printf(" (|  |)\n");
	printf("  (#1)\n");
	printf("  _)(_\n");
}

int main()
{
	int bezec1[2] = { 0, 0 }; // velocity cm/s, distance cm
	int bezec2[2] = { 0, 0 };

	// statistics
	float rychlost[2] = { 0, 0 };
	int naraz[2] = { 0, 0 }, i = 0;

	// draw rows
	initPlayer(0);
	initPlayer(1);

	srand(time(NULL));
	while (bezec1[1] < TRACK_LENGTH * 100 && bezec2[1] < TRACK_LENGTH * 100) // checks whether runner is in track
	{
		// generate speed
		bezec1[0] = get_speed(bezec1[0]);
		bezec2[0] = get_speed(bezec2[0]);

		// add current speed into track lenght
		bezec1[1] += bezec1[0];
		bezec2[1] += bezec2[0];

		// statistics
		rychlost[0] += bezec1[0];
		rychlost[1] += bezec2[0];

		// draw map
		AddInMap(bezec1, 0, naraz);
		AddInMap(bezec2, 1, naraz);

		showProgress(bezec1, bezec2, i);
		Sleep(CLOCK_TIME);
		i++;
	}

	// check winner
	if (bezec1[1] >= TRACK_LENGTH * 100 || bezec2[1] >= TRACK_LENGTH * 100)
	{
		if (bezec1[1] > bezec2[1])
		{
			printf("\n\n\nRunner1 won!\n");
		}
		else {
			printf("\n\n\nRunner2 won!\n");
		}
		printTrophy();
		printStats(rychlost, naraz, i);
	}

	system("pause");
	return 0;
}