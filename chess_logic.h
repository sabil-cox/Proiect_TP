// chess_logic.h
#ifndef CHESS_LOGIC_H
#define CHESS_LOGIC_H

#include <ctype.h>  // Folosit pentru isupper, islower
#include <stdlib.h> // Folosit pentru abs
#define SIZE 8


// Declaratii externe pentru variabilele globale definite in chess_logic.c
extern char board[SIZE][SIZE];
extern int white_turn; // 1 pentru tura Albului, 0 pentru tura Negrului

// --- Prototipuri Functii ---

// Initializare
void initialize_board();

int is_valid_pawn_move(int from_x, int from_y, int to_x, int to_y, int direction);
int is_valid_rook_move(int from_x, int from_y, int to_x, int to_y);
int is_valid_knight_move(int from_x, int from_y, int to_x, int to_y);
int is_valid_bishop_move(int from_x, int from_y, int to_x, int to_y);
int is_valid_queen_move(int from_x, int from_y, int to_x, int to_y);
int is_valid_king_move(int from_x, int from_y, int to_x, int to_y);

// Functie generala de validare a mutarii (fara verificarea "lasa regele in sah")
int is_valid_move(int from_x, int from_y, int to_x, int to_y);

// Functii pentru detectie sah / pat / legalitate mutare
int is_square_attacked(int target_x, int target_y, int attacker_is_white);
int is_in_check(int player_is_white);
int is_legal_move(int from_x, int from_y, int to_x, int to_y, int player_is_white);
int has_any_legal_move(int player_is_white);

// Functie pentru a efectua mutarea pe tabla (fara input sau output in terminal)
// Aceasta functie va fi apelata din GUI dupa ce GUI-ul a determinat coordonatele.
void perform_move_on_board(int from_x, int from_y, int to_x, int to_y);



#endif // CHESS_LOGIC_H