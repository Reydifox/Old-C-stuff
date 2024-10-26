// Author: Radovan Bittner
// Date: 11.3.2019

#include "chess.h"
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

enum players {
	White, Black, None
};

enum entity_type {
	Pawn, Rook, Knight, Bishop, Queen, King, Empty
};

int chessboard[L][L];

void print_chessboard() {
	system("cls");
	printf("\n\t\t\t\tBLACK\n\n");
	for (short row = 0; row < L; row++) {
		printf("\t\t\t%hd  ", L - row);
		for (short col = 0; col < L; col++) {
			printf("[%c]", chessboard[row][col]);
		}
		putchar('\n');
	}
	printf("\n\t\t\t    a  b  c  d  e  f  g  h \n\n");
	printf("\t\t\t\tWHITE\n");
}

void init_chessboard()
{
	for (short row = 0; row < L; row++) {
		for (short col = 0; col < L; col++) {
			switch (row)
			{
			case 0:
			{
				if (col == 0 || col == L - 1) chessboard[row][col] = B_ROOK;
				if (col == 1 || col == L - 2) chessboard[row][col] = B_KNIGHT;
				if (col == 2 || col == L - 3) chessboard[row][col] = B_BISHOP;
				if (col == 3) {
					chessboard[row][col] = B_QUEEN;
					chessboard[row][col + 1] = B_KING;
				}
				break;
			}
			case 1:		chessboard[row][col] = B_PAWN; break;
			case L - 2: chessboard[row][col] = W_PAWN; break;
			case L - 1:
			{
				if (col == 0 || col == L - 1) chessboard[row][col] = W_ROOK;
				if (col == 1 || col == L - 2) chessboard[row][col] = W_KNIGHT;
				if (col == 2 || col == L - 3) chessboard[row][col] = W_BISHOP;
				if (col == 3) {
					chessboard[row][col] = W_QUEEN;
					chessboard[row][col + 1] = W_KING;
				}
				break;
			}
			default: chessboard[row][col] = EMPTY_SQUARE;
			}
		}
	}
}

int get_enemy_id(int p_id) {
	if (p_id == WHITE_PLAYER_ID) {
		return BLACK_PLAYER_ID;
	}
	else if (p_id == BLACK_PLAYER_ID) {
		return WHITE_PLAYER_ID;
	}
	return -1;
}

int rook_move(int srow, int scol, int trow, int tcol, int p_id) {
	if (srow == trow || scol == tcol) {
		if (get_player_owner(trow, tcol) == None || get_player_owner(trow, tcol) == get_enemy_id(p_id)) {
			return true;
		}
	}
	return false;
}

int bishop_move(int srow, int scol, int trow, int tcol, int p_id) {
	if (scol != tcol && srow != trow) {
		if ((scol + srow) % 2 == 0) {
			if ((tcol + trow) % 2 == 0) {
				if (get_player_owner(trow, tcol) == None || get_player_owner(trow, tcol) == get_enemy_id(p_id)) {
					return true;
				}
			}
		}
		else {
			if ((tcol + trow) % 2 != 0) {
				if (get_player_owner(trow, tcol) == None || get_player_owner(trow, tcol) == get_enemy_id(p_id)) {
					return true;
				}
			}
		}
	}
	return false;
}

int knight_move(int srow, int scol, int trow, int tcol, int p_id) {
	if (srow + 2 == trow || srow - 2 == trow) {
		if (scol + 1 == tcol || scol - 1 == tcol) {
			if (get_player_owner(trow, tcol) == None || get_player_owner(trow, tcol) == get_enemy_id(p_id))
				return true;
		}
	}
	if (scol + 2 == tcol || scol - 2 == tcol) {
		if (srow + 1 == trow || srow - 1 == trow) {
			if (get_player_owner(trow, tcol) == None || get_player_owner(trow, tcol) == get_enemy_id(p_id))
				return true;
		}
	}
	return false;
}

int king_move(int srow, int scol, int trow, int tcol, int p_id) {
	if (scol + 1 == tcol || scol - 1 == tcol || scol == tcol) {
		if (srow + 1 == trow || srow - 1 == trow || srow == trow) {
			if (get_player_owner(trow, tcol) == None || get_player_owner(trow, tcol) == get_enemy_id(p_id))
				return true;
		}
	}
	else if (srow + 1 == trow || srow - 1 == trow || srow == trow) {
		if (scol + 1 == tcol || scol - 1 == tcol || scol == tcol) {
			if (get_player_owner(trow, tcol) == None || get_player_owner(trow, tcol) == get_enemy_id(p_id))
				return true;
		}
	}
	return false;
}

int check_move(enum entity_type p, int srow, int scol, int trow, int tcol, int p_id) {
	switch (p)
	{
	case Pawn: {
		if ((srow - trow == 1 && p_id == WHITE_PLAYER_ID) || (srow - trow == -1 && p_id == BLACK_PLAYER_ID)) {
			if (scol == tcol && get_player_owner(trow, tcol) == None) return true;
			else if (get_player_owner(trow, tcol) == get_enemy_id(p_id) && abs(srow - trow) == 1) {
				return true;
			}
		}
		break;
	}
	case Rook: {
		bool clear = true;
		int AB[2] = { tcol - scol, trow - srow };
		int foo = AB[0] == 0 ? 1 : 0;
		for (short i = AB[foo]; i != 0; AB[foo] > 0 ? i-- : i++) {
			int r = foo == 1 ? srow + i : trow;
			int c = foo == 1 ? tcol : scol + i;
			if (!rook_move(srow, scol, r, c, p_id)) {
				clear = false;
				break;
			}
		}
		return clear;
		break;
	}
	case Bishop: {
		if (scol == tcol || srow == trow) return false;

		bool clear = true;
		int AB[2] = { tcol - scol, trow - srow };
		int cfoo = AB[0] < 0 ? 1 : -1;
		int rfoo = AB[1] > 0 ? 1 : -1;
		for (short i = AB[0] * cfoo, j = AB[1] * rfoo; i != 0, j != 0; i++, j--) {
			if (!bishop_move(srow, scol, srow + (j * rfoo), scol + (i * cfoo), p_id)) {
				clear = false;
				break;
			}
		}
		return clear;
		break;
	}
	case Knight: return knight_move(srow, scol, trow, tcol, p_id); break;
	case Queen: {
		bool aclear = true;
		bool bclear = true;
		int AB[2] = { tcol - scol, trow - srow };
		int foo = AB[0] == 0 ? 1 : 0;
		for (short i = AB[foo]; i != 0; AB[foo] > 0 ? i-- : i++) {
			int r = foo == 1 ? srow + i : trow;
			int c = foo == 1 ? tcol : scol + i;
			if (!rook_move(srow, scol, r, c, p_id)) {
				aclear = false;
				break;
			}
		}

		if (scol == tcol || srow == trow) bclear = false;
		int cfoo = AB[0] < 0 ? 1 : -1;
		int rfoo = AB[1] > 0 ? 1 : -1;
		for (short i = AB[0] * cfoo, j = AB[1] * rfoo; i != 0, j != 0; i++, j--) {
			if (!bishop_move(srow, scol, srow + (j * rfoo), scol + (i * cfoo), p_id)) {
				bclear = false;
				break;
			}
		}
		return aclear | bclear;
		break;
	}
	case King: return king_move(srow, scol, trow, tcol, p_id);
	}
	return false;
}

enum entity_type get_entity(int row, int col, int p_id) {
	if (p_id == WHITE_PLAYER_ID) {
		switch (chessboard[row][col]) {
		case W_PAWN: return Pawn; break;
		case W_BISHOP: return Bishop; break;
		case W_ROOK: return Rook; break;
		case W_KNIGHT: return Knight; break;
		case W_QUEEN: return Queen; break;
		case W_KING: return King; break;
		}
	}
	else if (p_id == BLACK_PLAYER_ID) {
		switch (chessboard[row][col]) {
		case B_PAWN: return Pawn; break;
		case B_BISHOP: return Bishop; break;
		case B_ROOK: return Rook; break;
		case B_KNIGHT: return Knight; break;
		case B_QUEEN: return Queen; break;
		case B_KING: return King; break;
		}
	}
	return Empty;
}

// This function checks whether movement is correct: [srow,scol]-->[trow,tcol]
// In case the provided movement is correct, player will move.
//
// Input:
//      srow - current row
//      scol - current collum
//      trow - row of new move
//      tcol - collum of new move
//      p_id - player ID (check chess.h)
//
// Output:
//      Returns 1 - movement is correct.
//      Returns 0 - movement is incorrect.

int move(int srow, int scol, int trow, int tcol, int p_id)
{
	if (p_id == get_player_owner(srow, scol)) {
		if (check_move(get_entity(srow, scol, p_id), srow, scol, trow, tcol, p_id))
		{
			chessboard[trow][tcol] = chessboard[srow][scol];
			chessboard[srow][scol] = EMPTY_SQUARE;
		}
		else return MOVE_ERR_TURN;
	}
	else return MOVE_ERR_OWNER;

	return 0;
}

int random_move() {
	short sy = rand() % L;
	short ty = rand() % L;
	short sx = rand() % L;
	short tx = rand() % L;
	short move = 0;

	while (move == 0) {
		if (check_move(get_entity(sy, sx, BLACK_PLAYER_ID), sy, sx, ty, tx, BLACK_PLAYER_ID))
		{
			chessboard[ty][tx] = chessboard[sy][sx];
			chessboard[sy][sx] = EMPTY_SQUARE;
			move = 1;
		}
		sy = rand() % L;
		ty = rand() % L;
		sx = rand() % L;
		tx = rand() % L;
	}
	return 0;
}

bool is_char_lower(int c) {
	if (c >= 'a' && c <= 'z')
		return true;
	return false;
}

int has_player_king(int p_id) {
	if (p_id == WHITE_PLAYER_ID) {
		for (int r = 0; r < L; r++) {
			for (int c = 0; c < L; c++) {
				if (chessboard[r][c] == W_KING) {
					return true;
				}
			}
		}
	}
	if (p_id == BLACK_PLAYER_ID) {
		for (int r = 0; r < L; r++) {
			for (int c = 0; c < L; c++) {
				if (chessboard[r][c] == B_KING) {
					return true;
				}
			}
		}
	}
	return false;
}

enum Players get_player_owner(int row, int col) {		
	if (chessboard[row][col] != EMPTY_SQUARE) {
		if (is_char_lower(chessboard[row][col])) {
			return White;
		}
		else return Black;
	}
	return None;
}
