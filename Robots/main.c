// Author: Radovan Bittner
// Date: 25.3.2019

#include <stdio.h>
#include <time.h>
#include "robot_race.h"

int main(int argc, char* argv[])
{
	srand(time(NULL));
	int n = 5;// atoi(argv[1]);

	// 1. Init level
	Track track;
	Robot robots[MAX_ROBOTS];

	initTrack(&track);
	initRobots(robots, n, &track);

	print(robots, n, &track);
	
	// 2. Race
	do {
		system("cls");
		for (int i = 0; i < n; i++) {
			int col = isCollision(&robots[i], &track);
			switch (col) {
			case BOOSTER_COLLISION: robots[i].speed = robots[i].speed + 1; break;
			case STOPPER_COLLISION: robots[i].speed = 0; break;
			}

			updateRobotDirection(&robots[i], &track, col);
			updateRobotPosition(&robots[i]);
		}
		print(robots, n, &track);
		Sleep(CLOCK);
	} while (isFinished(robots, n) == -1 && isMoving(robots, n));

	if (isFinished(robots, n) != -1) {
		printf("Robot %d is winner!\n", isFinished(robots, n));
	}
	else printf("Nobody won, the robots got stuck!\n");
	system("pause");
	return 0;
}
