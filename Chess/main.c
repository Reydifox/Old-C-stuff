// Author: Radovan Bittner
// Date: 11.3.2019

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include "chess.h"

#define KEY_W				119
#define KEY_A				97
#define KEY_S				115
#define KEY_D				100
#define KEY_ENTER			13

#define MENU_PLAY			0
#define MENU_LEVEL			1
#define MENU_ENEMIES		2
#define MENU_EXIT			3

static char menu[3][12] =
{
	"> Play",
	"  Type: ",
	"  Exit"
};
static char menuType[2][4] =
{
	"PvP",
	"PvE"
};
bool ingame = false;
bool pvp = true;

int menu_position = 0;
int menu_position_type = 0;
int last_position = 0;

void gotoxy(int x, int y)
{
	COORD pos = { x, y };
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(output, pos);
}

void initMenu() {
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < sizeof(menu[i]); j++) {
			printf("%c", menu[i][j]);
		}
		printf("\n");
	}
	gotoxy(8, 1);
	printf("%s", menuType[menu_position_type]);

}

void menuSelecting() {
	menu[last_position][0] = ' ';
	menu[menu_position][0] = '>';
	last_position = menu_position;

	gotoxy(0, 0);
	initMenu();
}

void key_control(int key) {
	switch (key) {
	case KEY_W:  menu_position = menu_position > 0 ? menu_position - 1 : 2; break;
	case KEY_S:  menu_position = menu_position < 2 ? menu_position + 1 : 0; break;
	case KEY_D: {
		if (menu_position == MENU_LEVEL) {
			menu_position_type = menu_position_type < 1 ? menu_position_type + 1 : 0;
			pvp = menu_position_type == 0 ? true : false;
		}
		break;
	}
	case KEY_ENTER: {
		switch (menu_position) {
		case MENU_PLAY: ingame = true; break;
		case MENU_EXIT: exit(1);
		}
		break;
	}
	}
	menuSelecting();
}

bool parse_input(char* str, int p_id) {
	if (strlen(str) > 4) {
		printf("Bad input!\n");
		Sleep(1000);
		return false;
	}

	short o_row = -1, o_col = -1, n_row = 0, n_col = 0;
	for (short i = 0; i < 4; i++) {
		if ((str[i] >= 'a' && str[i] <= 'h') || (str[i] >= '1' && str[i] <= '8')) {
			if (str[i] >= 'a' && str[i] <= 'h') {
				if (o_col == -1) o_col = str[i] - 'a';
				else n_col = str[i] - 'a';
			}
			else {
				if (o_row == -1) o_row = str[i] - '1';
				else n_row = str[i] - '1';
			}
		}
		else {
			printf("Bad input!\n");
			Sleep(1000);
			return false;
		}
	}

	switch (move(L - o_row - 1, o_col, L - n_row - 1, n_col, p_id))
	{
	case MOVE_ERR_OWNER: {
		printf("This entity is owned by other player!");
		Sleep(1000);
		return false;
	}
	case MOVE_ERR_TURN: {
		printf("Invalid move!");
		Sleep(1000);
		return false;
	}
	default: return true;
	}
	return true;
}

int main() {
	bool white_turn = true;

	initMenu();
	while (!ingame)
		if (kbhit()) key_control(getch());

	srand(time(NULL));
	init_chessboard();
	while (ingame) {
		print_chessboard();

		if (!has_player_king(WHITE_PLAYER_ID) || !has_player_king(BLACK_PLAYER_ID))
			break;

		char pos[5];
		if (white_turn) {
			printf("\nWhite to move: ");
			gets(pos);
			if (parse_input(pos, WHITE_PLAYER_ID) && pvp) {
				white_turn = false;
			}
		}
		else if (!white_turn && pvp) {
			printf("\nBlack to move: ");
			gets(pos);
			if (parse_input(pos, BLACK_PLAYER_ID)) {
				white_turn = true;
			}
		}

		if (!pvp) random_move();
		system("cls");
	}
	if (!has_player_king(WHITE_PLAYER_ID)) printf("\nThe Black player won!\n");
	else printf("\nThe White player won!\n");

	system("pause");
	return 0;
}