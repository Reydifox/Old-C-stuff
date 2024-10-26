// Author: Radovan Bittner
// Date: 25.3.2019

#include <stdio.h>
#include "robot_race.h"

void putObstacleInMap(Obstacle* obs) {
	for (int i = 0; i < obs->height; i++) {
		for (int j = 0; j < obs->width; j++) {
			level[obs->y + i][obs->x + j] = OBSTACLE_MODEL;
		}
	}
}

void putEntityInMap(Entity* ent) {
	switch (ent->type) {
	case booster: level[ent->y][ent->x] = ENTITY_BOOSTER_MDL; break;
	case stopper: level[ent->y][ent->x] = ENTITY_STOPPER_MDL;
	}
}

void putRobotsInMap(Robot robots[], int n) {
	for (int i = 0; i < n; i++) {
		level[0][robots[i].x] = robots[i].id + '0';
	}
}

// This function randomly generates the obstacles on the map
// Width, Height of the map and as well as number of obstacles are defines by macros
void initTrack(Track* track) {
	// init empty map
	for (int i = 0; i < MAX_TRACK_HEIGHT; i++) {
		for (int j = 0; j < MAX_TRACK_WIDTH - WIDTH_BORDER; j++) {
			level[i][j] = ' ';
		}
	}

	// init track
	track->width = MAX_TRACK_WIDTH;
	track->height = MAX_TRACK_HEIGHT;

	for (int i = 0, j = 0, k = 0; i < track->height; i++) {
		if (!(i % OBSTACLE_DISTANCE) && j < MAX_OBSTACLES && i > 0) {
			Obstacle tmp;
			tmp.x = rand() % (track->width - WIDTH_BORDER + 1 - WIDTH_BORDER) + WIDTH_BORDER;
			tmp.y = i;
			tmp.width = rand() % (MAX_OBSTACLE_WIDTH + 1 - MIN_OBSTACLE_WIDTH) + MIN_OBSTACLE_WIDTH;
			tmp.height = rand() % (MAX_OBSTACLE_HEIGHT + 1 - MIN_OBSTACLE_HEIGHT) + MIN_OBSTACLE_HEIGHT;

			track->o_arr[j++] = tmp;
			track->obstacles = j;
			putObstacleInMap(&tmp);
		}
#if defined ENT_ENABLED
		if (!(i % MAX_TRACK_HEIGHT / MAX_ENTITIES) && k < MAX_ENTITIES && i > 0) {
			Entity ent;
			ent.x = rand() % (track->width - WIDTH_BORDER + 1 - WIDTH_BORDER) + WIDTH_BORDER;
			ent.y = i;
			ent.type = rand() % sizeof(ENT_TYPE);
			;
			track->e_arr[k++] = ent;
			track->entities = k;
			putEntityInMap(&ent);
		}
#endif
	}
}

// This function will place the robots on the map towards to the finish line
void initRobots(Robot robots[], int n, Track* track) {
	for (int i = 0; i < n; i++) {
		robots[i].id = i;
		robots[i].y = 0;
		robots[i].x = WIDTH_BORDER + 10 * (i + 1);
		robots[i].direction = ROBOT_DIRECTION_FW;
		robots[i].speed = ROBOT_SPEED;
	}
	putRobotsInMap(robots, n);
}

// This function will render the level with track and robots
void print(Robot robots[], int n, Track* track)
{
	for (int i = 0; i < MAX_TRACK_HEIGHT; i++) {
		for (int j = 0; j < MAX_TRACK_WIDTH; j++) {
			if (j < WIDTH_BORDER) putchar(' ');
			if (j == WIDTH_BORDER || j == MAX_TRACK_WIDTH - WIDTH_BORDER) putchar(179);
			else if (j > WIDTH_BORDER && j < MAX_TRACK_WIDTH - WIDTH_BORDER) {
				putchar(level[i][j]);
			}
		}
		putchar('\n');
	}
}

void updateRobotPosition(Robot* robot)
{
	level[robot->y][robot->x] = '.';
	switch (robot->direction) {
	case ROBOT_DIRECTION_FW: robot->y = robot->y + robot->speed; break;
	case ROBOT_DIRECTION_LT: robot->x = robot->x - robot->speed; break;
	case ROBOT_DIRECTION_RT:  robot->x = robot->x + robot->speed; break;
	}
	level[robot->y][robot->x] = robot->id + '0';
}

void updateRobotDirection(Robot* robot, Track* track, int obstacle_id)
{
	switch (obstacle_id) {
	case MAP_COLLISION: {
		if (robot->direction == ROBOT_DIRECTION_LT)
			robot->direction = ROBOT_DIRECTION_RT;
		else if (robot->direction == ROBOT_DIRECTION_RT)
			robot->direction = ROBOT_DIRECTION_LT;
		break;
	}
	case BOOSTER_COLLISION: break;
	case STOPPER_COLLISION: break;
	case NO_COLLISION: {
		switch (robot->direction) {
		case ROBOT_DIRECTION_LT: {
			Robot tmp;
			tmp.direction = ROBOT_DIRECTION_FW;
			tmp.speed = robot->speed;
			tmp.x = robot->x;
			tmp.y = robot->y;
			if (isCollision(&tmp, track) == NO_COLLISION) {
				robot->direction = ROBOT_DIRECTION_FW;
			}
			break;
		}
		case ROBOT_DIRECTION_RT: {
			Robot tmp;
			tmp.direction = ROBOT_DIRECTION_FW;
			tmp.speed = robot->speed;
			tmp.x = robot->x;
			tmp.y = robot->y;
			if (isCollision(&tmp, track) == NO_COLLISION) {
				robot->direction = ROBOT_DIRECTION_FW;
			}
			break;
		}
		}
		break;
	}
	default: {
		switch (robot->direction) {
		case ROBOT_DIRECTION_FW: {
			Robot tmp;
			tmp.x = robot->x;
			tmp.y = robot->y;
			tmp.speed = robot->speed;

			int cmp = track->o_arr[obstacle_id].x;
			if (cmp + (track->o_arr[obstacle_id].width / 2) > robot->x) {
				tmp.direction = ROBOT_DIRECTION_LT;
				if (isCollision(&tmp, track) == NO_COLLISION) {
					robot->direction = ROBOT_DIRECTION_LT;
				}
				else robot->direction = ROBOT_DIRECTION_RT;
			}
			else {
				tmp.direction = ROBOT_DIRECTION_RT;
				if (isCollision(&tmp, track) == NO_COLLISION) {
					robot->direction = ROBOT_DIRECTION_RT;
				}
				else robot->direction = ROBOT_DIRECTION_LT;
			}
			break;
		}
		case ROBOT_DIRECTION_LT: {
			Robot tmp;
			tmp.speed = robot->speed;
			tmp.x = robot->x;
			tmp.y = robot->y;

			tmp.direction = ROBOT_DIRECTION_RT;
			if (isCollision(&tmp, track) == NO_COLLISION) {
				robot->direction = ROBOT_DIRECTION_RT;
				break;
			}

			tmp.direction = ROBOT_DIRECTION_FW;
			if (isCollision(&tmp, track) == NO_COLLISION) {
				robot->direction = ROBOT_DIRECTION_FW;
			}
			break;
		}
		case ROBOT_DIRECTION_RT: {
			Robot tmp;
			tmp.speed = robot->speed;
			tmp.x = robot->x;
			tmp.y = robot->y;

			tmp.direction = ROBOT_DIRECTION_LT;
			if (isCollision(&tmp, track) == NO_COLLISION) {
				robot->direction = ROBOT_DIRECTION_LT;
				break;
			}

			tmp.direction = ROBOT_DIRECTION_FW;
			if (isCollision(&tmp, track) == NO_COLLISION) {
				robot->direction = ROBOT_DIRECTION_FW;
			}
			break;
		}
		}
	}
	}
}

// This function checks, whether robot can move with the current direction and speed without collision of obstacle or map border
// Return:
//      * -2: no collision (valid movement)
//      * -1: collision with the border of map
//      * 0...n: collision with the n-th obstacle
int isCollision(Robot* robot, Track* track)
{
	switch (robot->direction) {
	case ROBOT_DIRECTION_FW: {
		for (int i = 0; i < track->obstacles; i++) {
			for (int x = 1; x <= robot->speed; x++) {
				if (robot->y + x == track->o_arr[i].y) {
					for (int j = 0; j < track->o_arr[i].width; j++) {
						if (robot->x == track->o_arr[i].x + j) {
							return i;
						}
					}
				}
			}
		}
		break;
	}
	case ROBOT_DIRECTION_RT: {
		for (int i = 0; i < track->obstacles; i++) {
			for (int x = 1; x <= robot->speed; x++) {
				if (robot->x + x == track->width - WIDTH_BORDER) return MAP_COLLISION;
				for (int j = 0; j < track->o_arr[i].height; j++) {
					for (int l = 0; l < track->o_arr[i].width; l++) {
						if (robot->y == track->o_arr[i].y + j && robot->x + x == track->o_arr[i].x + l) {
							return i;
						}
					}
				}
			}
		}
		break;
	}
	case  ROBOT_DIRECTION_LT: {
		for (int i = 0; i < track->obstacles; i++) {
			for (int x = 1; x <= robot->speed; x++) {
				if (robot->x - x == WIDTH_BORDER) return MAP_COLLISION;
				for (int j = 0; j < track->o_arr[i].height; j++) {
					for (int l = 0; l < track->o_arr[i].width; l++) {
						if (robot->y == track->o_arr[i].y + j && robot->x - x == track->o_arr[i].x + l) {
							return i;
						}
					}
				}
			}
		}
		break;
	}
	}

#if defined ENT_ENABLED
	switch (robot->direction) {
	case ROBOT_DIRECTION_FW: {
		for (int i = 0; i < track->entities; i++) {
			for (int x = 1; x <= robot->speed; x++) {
				if (robot->y + x == track->e_arr[i].y) {
					if (robot->x == track->e_arr[i].x && track->e_arr[i].type == booster)
						return BOOSTER_COLLISION;
					else if (robot->x == track->e_arr[i].x && track->e_arr[i].type == stopper)
						return STOPPER_COLLISION;
				}
			}
		}
		break;
	}
	case ROBOT_DIRECTION_RT: {
		for (int i = 0; i < track->entities; i++) {
			for (int x = 1; x <= robot->speed; x++) {
				if (robot->x + x == track->e_arr[i].x && track->e_arr[i].type == booster)
					return BOOSTER_COLLISION;
				else if (robot->x + x == track->e_arr[i].x && track->e_arr[i].type == stopper)
					return STOPPER_COLLISION;
				break;
			}
		}
		break;
	}
	case ROBOT_DIRECTION_LT: {
		for (int i = 0; i < track->entities; i++) {
			for (int x = 1; x <= robot->speed; x++) {
				if (robot->x - x == track->e_arr[i].x && track->e_arr[i].type == booster)
					return BOOSTER_COLLISION;
				else if (robot->x - x == track->e_arr[i].x && track->e_arr[i].type == stopper)
					return STOPPER_COLLISION;
				break;
			}
		}
		break;
	}
	}
#endif
	return NO_COLLISION;
}

int isFinished(Robot robots[], int n) {
	for (int i = 0; i < n; i++) {
		if (robots[i].y >= MAX_TRACK_HEIGHT - 1) {
			return i;
		}
	}
	return -1;
}

int isMoving(Robot robots[], int n) {
	int count = 0;
	for (int i = 0; i < n; i++) {
		if (robots[i].speed == 0) {
			count++;
		}
	}
	if (count == n) return 0;
	return 1;
}