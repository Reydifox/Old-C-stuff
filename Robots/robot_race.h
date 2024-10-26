// Author: Radovan Bittner
// Date: 25.3.2019

#ifndef ROBOT_RACE_H
#define ROBOT_RACE_H

#define _CRT_SECURE_NO_DEPRECATE
#define _USE_MATH_DEFINES

#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN() system("cls")
#define SLEEP(sleepMs) Sleep(sleepMs)
#else
#include <unistd.h>
#define CLEAR_SCREEN() system("clear")
#define SLEEP(sleepMs) usleep(sleepMs * 1000)
#endif

#define CLOCK				50

#define MAX_ROBOTS			5
#define MAX_ENTITIES		80
#define MAX_OBSTACLES		100 

#define OBSTACLE_MODEL		176
#define OBSTACLE_DISTANCE	3
#define MAX_OBSTACLE_WIDTH	20
#define MIN_OBSTACLE_WIDTH	2
#define MAX_OBSTACLE_HEIGHT 8
#define MIN_OBSTACLE_HEIGHT 4

#define MAX_TRACK_HEIGHT	30 
#define MAX_TRACK_WIDTH		80
#define WIDTH_BORDER		10

#define ROBOT_SPEED			1 // default speed (one step)
#define ROBOT_DIRECTION_FW	0
#define ROBOT_DIRECTION_LT	-1
#define ROBOT_DIRECTION_RT	1

#define NO_COLLISION		-2
#define MAP_COLLISION		-1
#define BOOSTER_COLLISION	-5
#define STOPPER_COLLISION	-6

#define ENT_ENABLED // enables bonus features or comment this line to disable
#define ENTITY_BOOSTER_MDL	193
#define ENTITY_STOPPER_MDL	'x'

typedef enum {
	booster,
	stopper
} ENT_TYPE;

typedef struct {
	int x;
	int y;
	ENT_TYPE type;
} Entity;

typedef struct {
	int x;
	int y;
	int width;
	int height;
} Obstacle;

typedef struct {
	int width;
	int height;
	int obstacles;
	int entities;
	Obstacle o_arr[MAX_OBSTACLES];
	Entity e_arr[MAX_ENTITIES];
} Track;

typedef struct {
	int id;
	int x;
	int y;
	int speed;
	int direction;
} Robot;

static char level[MAX_TRACK_HEIGHT][MAX_TRACK_WIDTH - WIDTH_BORDER];

void putObstacleInMap(Obstacle* obs);
void putEntityInMap(Entity* ent);
void putRobotsInMap(Robot robots[], int n);
void initTrack(Track* track);
void initRobots(Robot robots[], int n, Track* track); 
void print(Robot robots[], int n, Track* track); 
void updateRobotPosition(Robot* robot); 
void updateRobotDirection(Robot* robot, Track* track, int obstacle_id); 
int isCollision(Robot* robot, Track* track); 
int isFinished(Robot robots[], int n);
int isMoving(Robot robots[], int n);

#endif // ROBOT_RACE_H
