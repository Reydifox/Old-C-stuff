// Author: Radovan Bittner
// Date: 11.3.2019

#ifndef CHESS_H
#define CHESS_H

#define EMPTY_SQUARE ' '//0 

#define B_PAWN		'P'  //-1
#define B_ROOK		'V'  //-2
#define B_KNIGHT	'J' //-3 
#define B_BISHOP	'S' //-4 
#define B_QUEEN		'D'  //-5 
#define B_KING		'K'   //-6 

#define W_PAWN		'p' //1
#define W_ROOK		'v' //2
#define W_KNIGHT	'j' //3
#define W_BISHOP	's' //4
#define W_QUEEN		'd' //5
#define W_KING		'k' //6

#define L 8 // size of the board
#define WHITE_PLAYER_ID 0 
#define BLACK_PLAYER_ID 1 

#define MOVE_ERR_OWNER	1
#define MOVE_ERR_TURN	2

extern int chessboard[L][L];

void print_chessboard(); 
void init_chessboard(); 
int get_enemy_id(int p_id);
int rook_move(int srow, int scol, int trow, int tcol, int p_id);
int bishop_move(int srow, int scol, int trow, int tcol, int p_id);
int knight_move(int srow, int scol, int trow, int tcol, int p_id);
int king_move(int srow, int scol, int trow, int tcol, int p_id);
int move(int srow, int scol, int trow, int tcol, int p_id);
int random_move();

int has_player_king(int p_id);
enum Players get_player_owner(int row, int col);

#endif // CHESS_H
