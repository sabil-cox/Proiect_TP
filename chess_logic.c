#include "chess_logic.h" 
#include <stdio.h>     
#include <string.h>    
#include <ctype.h>     
#include <stdlib.h>   

// Definirea variabilelor globale
char board[SIZE][SIZE];
int white_turn = 1; // Albul incepe intotdeauna

// --- Implementarea Functiilor ---

void initialize_board() {
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = ' ';
        }
    }
    for (int j = 0; j < SIZE; j++) board[1][j] = 'p';
    board[0][0] = board[0][7] = 't';
    board[0][1] = board[0][6] = 'c';
    board[0][2] = board[0][5] = 'n';
    board[0][3] = 'r';
    board[0][4] = 'k';
    for (int j = 0; j < SIZE; j++) board[6][j] = 'P';
    board[7][0] = board[7][7] = 'T';
    board[7][1] = board[7][6] = 'C';
    board[7][2] = board[7][5] = 'N';
    board[7][3] = 'R';
    board[7][4] = 'K';
    white_turn = 1;
}

// Functiile de validare specifice pieselor (is_valid_pawn_move, etc.)

int is_valid_pawn_move(int from_x, int from_y, int to_x, int to_y, int direction) {
    char target_piece = board[to_x][to_y];
    int start_row = (direction == -1) ? 6 : 1;

    // Miscare inainte
    if (from_y == to_y && target_piece == ' ') {
        if (to_x == from_x + direction) return 1; // Un pas inainte
        // Doi pasi inainte de pe pozitia de start
        if (from_x == start_row && to_x == from_x + 2 * direction && board[from_x + direction][from_y] == ' ') return 1;
    }
    // Captura pe diagonala
    else if (abs(from_y - to_y) == 1 && to_x == from_x + direction && target_piece != ' ') {
        // Pionul alb captureaza piesa neagra, pionul negru captureaza piesa alba
        if ((direction == -1 && islower(target_piece)) || (direction == 1 && isupper(target_piece))) {
            return 1;
        }
    }    
    return 0;
}

int is_valid_rook_move(int from_x, int from_y, int to_x, int to_y) {
    if (from_x != to_x && from_y != to_y) return 0; // Nu e miscare pe linie sau coloana
    // Verifica piese pe drum
    if (from_x == to_x) { // Miscare pe coloana
        int step = (to_y > from_y) ? 1 : -1;
        for (int y = from_y + step; y != to_y; y += step) {
            if (board[from_x][y] != ' ') return 0; // Cale blocata
        }
    } else { // Miscare pe linie (from_y == to_y)
        int step = (to_x > from_x) ? 1 : -1;
        for (int x = from_x + step; x != to_x; x += step) {
            if (board[x][from_y] != ' ') return 0; // Cale blocata
        }
    }
    return 1;
}

int is_valid_knight_move(int from_x, int from_y, int to_x, int to_y) {
    int dx = abs(from_x - to_x);
    int dy = abs(from_y - to_y);
    return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
}

int is_valid_bishop_move(int from_x, int from_y, int to_x, int to_y) {
    if (abs(from_x - to_x) != abs(from_y - to_y)) return 0; // Nu e miscare pe diagonala
    // Verifica piese pe drum
    int step_x = (to_x > from_x) ? 1 : -1;
    int step_y = (to_y > from_y) ? 1 : -1;
    int x = from_x + step_x;
    int y = from_y + step_y;
    while (x != to_x) { 
        if (board[x][y] != ' ') return 0; // Cale blocata
        x += step_x;
        y += step_y;
    }
    return 1;
}

int is_valid_queen_move(int from_x, int from_y, int to_x, int to_y) {
    return is_valid_rook_move(from_x, from_y, to_x, to_y) || is_valid_bishop_move(from_x, from_y, to_x, to_y);
}

int is_valid_king_move(int from_x, int from_y, int to_x, int to_y) {
    // Miscare normala de rege
    if (abs(from_x - to_x) <= 1 && abs(from_y - to_y) <= 1) {
        return 1;
    }
    return 0;
}

// Functie generala de validare a mutarii
int is_valid_move(int from_x, int from_y, int to_x, int to_y) {
    // Verificari initiale de baza (in limitele tablei, nu aceeasi pozitie)
    if (from_x < 0 || from_x >= SIZE || from_y < 0 || from_y >= SIZE ||
        to_x < 0 || to_x >= SIZE || to_y < 0 || to_y >= SIZE) {
        return 0;
    }
    if (from_x == to_x && from_y == to_y) return 0; // Nu se misca

    char piece = board[from_x][from_y];
    char dest_piece = board[to_x][to_y];

    if (piece == ' ') return 0; // Nu e nicio piesa la origine

    // Verifica daca piesa de la destinatie este de aceeasi culoare
    if (dest_piece != ' ') {
        if ((isupper(piece) && isupper(dest_piece)) || (islower(piece) && islower(dest_piece))) {
            return 0; // Nu poti captura propria piesa
        }
    }

    // Apeleaza functia de validare specifica piesei
    switch (piece) {
        case 'P': return is_valid_pawn_move(from_x, from_y, to_x, to_y, -1); // Alb se misca spre index mai mic
        case 'p': return is_valid_pawn_move(from_x, from_y, to_x, to_y, 1);  // Negru se misca spre index mai mare
        case 'T': case 't': return is_valid_rook_move(from_x, from_y, to_x, to_y);
        case 'C': case 'c': return is_valid_knight_move(from_x, from_y, to_x, to_y);
        case 'N': case 'n': return is_valid_bishop_move(from_x, from_y, to_x, to_y);
        case 'R': case 'r': return is_valid_queen_move(from_x, from_y, to_x, to_y);
        case 'K': case 'k': return is_valid_king_move(from_x, from_y, to_x, to_y);
        default: return 0; // Piesa necunoscuta
    }
}

// Functie care verifica daca un patrat este atacat de o culoare
int is_square_attacked(int target_x, int target_y, int attacker_is_white) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char piece = board[i][j];
            if (piece == ' ') continue;

            int piece_is_attacker_color = (attacker_is_white && isupper(piece)) || (!attacker_is_white && islower(piece));

            if (piece_is_attacker_color) {

                // Pentru pioni, atacul e diferit de miscare.
                if (piece == 'P' && attacker_is_white) { // Pion alb atacand
                    if ((i - 1 == target_x && abs(j - target_y) == 1)) return 1;
                } else if (piece == 'p' && !attacker_is_white) { // Pion negru atacand
                     if ((i + 1 == target_x && abs(j - target_y) == 1)) return 1;
                } else if ((piece != 'P' && piece != 'p') && is_valid_move(i, j, target_x, target_y)) {

                    if (is_valid_move(i, j, target_x, target_y)) return 1;
                }
            }
        }
    }
    return 0;
}


int is_in_check(int player_is_white) {
    int king_x = -1, king_y = -1;
    char king_char = player_is_white ? 'K' : 'k';

    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (board[r][c] == king_char) {
                king_x = r;
                king_y = c;
                break;
            }
        }
        if (king_x != -1) break;
    }

    if (king_x == -1) {
        return 1; // Consideram ca e in sah pentru a opri jocul sau a semnala o eroare grava.
    }
    return is_square_attacked(king_x, king_y, !player_is_white); // Atacat de oponent
}

int is_legal_move(int from_x, int from_y, int to_x, int to_y, int player_is_white) {

    char piece_to_move = board[from_x][from_y];
    char original_target_piece = board[to_x][to_y]; // Salveaza ce era la destinatie

    // Simuleaza mutarea
    board[to_x][to_y] = piece_to_move;
    board[from_x][from_y] = ' ';

    // Verifica daca regele jucatorului care A MUTAT este in sah ACUM
    int leaves_king_in_check = is_in_check(player_is_white);

    // Anuleaza simularea (restaureaza tabla la starea anterioara)
    board[from_x][from_y] = piece_to_move;
    board[to_x][to_y] = original_target_piece;

    return !leaves_king_in_check; // Mutarea e legala daca NU lasa regele in sah
}

int has_any_legal_move(int player_is_white) {
    for (int r_from = 0; r_from < SIZE; r_from++) {
        for (int c_from = 0; c_from < SIZE; c_from++) {
            char piece = board[r_from][c_from];
            if (piece == ' ') continue;

            // Verifica daca piesa apartine jucatorului curent
            if ((player_is_white && isupper(piece)) || (!player_is_white && islower(piece))) {
                // Incearca toate mutarile posibile pentru aceasta piesa
                for (int r_to = 0; r_to < SIZE; r_to++) {
                    for (int c_to = 0; c_to < SIZE; c_to++) {
                        if (is_valid_move(r_from, c_from, r_to, c_to)) {
                            if (is_legal_move(r_from, c_from, r_to, c_to, player_is_white)) {
                                return 1; // Am gasit cel putin o mutare legala
                            }
                        }
                    }
                }
            }
        }
    }
    return 0; // Nu s-a gasit nicio mutare legala
}

void perform_move_on_board(int from_x, int from_y, int to_x, int to_y) {

    board[to_x][to_y] = board[from_x][from_y];
    board[from_x][from_y] = ' ';

    char moved_piece = board[to_x][to_y];
    if (moved_piece == 'P' && to_x == 0) { // Pion alb ajunge pe randul 0 (al negrului)
        board[to_x][to_y] = 'R'; // Promoveaza la Regina Alba
    } else if (moved_piece == 'p' && to_x == SIZE - 1) { // Pion negru ajunge pe randul 7 (al albului)
        board[to_x][to_y] = 'r'; // Promoveaza la Regina Neagra
    }

}