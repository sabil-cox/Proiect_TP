#include <stdio.h>
#include <ctype.h>  // Folosit pentru isupper, islower
#include <stdlib.h> // Folosit pentru abs
#include <string.h> // Folosit pentru strcmp, fgets, strcspn


#define SIZE 8
#define MAX_INPUT_LEN 10 // Pentru bufferul fgets

char board[SIZE][SIZE];
int white_turn = 1; // 1 pentru tura Albului, 0 pentru tura Negrului


// Prototipuri Functii
// Necesare pentru ca functiile sa se poata apela intre ele.
int is_valid_move(int from_x, int from_y, int to_x, int to_y);
int is_valid_pawn_move(int from_x, int from_y, int to_x, int to_y, int direction);
int is_valid_rook_move(int from_x, int from_y, int to_x, int to_y);
int is_valid_knight_move(int from_x, int from_y, int to_x, int to_y);
int is_valid_bishop_move(int from_x, int from_y, int to_x, int to_y);
int is_valid_queen_move(int from_x, int from_y, int to_x, int to_y);
int is_valid_king_move(int from_x, int from_y, int to_x, int to_y);
int is_square_attacked(int target_x, int target_y, int attacker_is_white);

// Functii noi pentru detectie sah mat / pat
int is_in_check(int player_is_white);
int is_legal_move(int from_x, int from_y, int to_x, int to_y, int player_is_white);
int has_any_legal_move(int player_is_white);


// Initializare si Afisare Tabla
void initialize_board()
{
    printf("TABLA SAH:\n");
    printf("\n");

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = ' '; //pune locurile goale
        }
    }

    // Piese negre (lowercase) - randurile 0 si 1 (conform indexarii array-ului)
    for (int j = 0; j < SIZE; j++) board[1][j] = 'p'; // Pioni negri pe randul 1
    board[0][0] = board[0][7] = 't'; // Turnuri negre
    board[0][1] = board[0][6] = 'c'; // Cai negri
    board[0][2] = board[0][5] = 'n'; // Nebuni negri
    board[0][3] = 'r'; // Regina neagra
    board[0][4] = 'k'; // Rege negru

    // Piese albe (uppercase) - randurile 6 si 7 (conform indexarii array-ului)
    for (int j = 0; j < SIZE; j++) board[6][j] = 'P'; // Pioni albi pe randul 6
    board[7][0] = board[7][7] = 'T'; // Turnuri albe
    board[7][1] = board[7][6] = 'C'; // Cai albi
    board[7][2] = board[7][5] = 'N'; // Nebuni albi
    board[7][3] = 'R'; // Regina alba
    board[7][4] = 'K'; // Rege alb

    white_turn = 1; // Albul incepe intotdeauna
}

void print_board() {
    printf("\n");
    printf("      a   b   c   d   e   f   g   h  \n");
    printf("    +---+---+---+---+---+---+---+---+\n");
    for (int i = 0; i < SIZE; i++)
    {
        printf("  %d |", 8 - i);
        for (int j = 0; j < SIZE; j++) {
            printf(" %c |", board[i][j]);
        }
        printf(" %d\n", 8 - i);
        printf("    +---+---+---+---+---+---+---+---+\n");
    }
    printf("      a   b   c   d   e   f   g   h  \n");
    printf("\n");
}

// Validare Mutari Specifice Pieselor
// Pionii (is_valid_pawn_move) se pot mișca doar înainte și capturează pe diagonală.
int is_valid_pawn_move(int from_x, int from_y, int to_x, int to_y, int direction) {
    char target_piece = board[to_x][to_y];
    int start_row = (direction == -1) ? 6 : 1;

    if (from_y == to_y && target_piece == ' ') {
        if (to_x == from_x + direction) return 1;
        if (from_x == start_row && to_x == from_x + 2 * direction && board[from_x + direction][from_y] == ' ') return 1;
    }
    else if (abs(from_y - to_y) == 1 && to_x == from_x + direction && target_piece != ' ') {
        if ((direction == -1 && islower(target_piece)) || (direction == 1 && isupper(target_piece))) {
            return 1;
        }
    }
    return 0;
}

//Turnurile (is_valid_rook_move) se mișcă pe linii sau coloane.
int is_valid_rook_move(int from_x, int from_y, int to_x, int to_y) {
    if (from_x != to_x && from_y != to_y) return 0;
    if (from_x == to_x) {
        int step = (to_y > from_y) ? 1 : -1;
        for (int y = from_y + step; y != to_y; y += step) if (board[from_x][y] != ' ') return 0;
    } else {
        int step = (to_x > from_x) ? 1 : -1;
        for (int x = from_x + step; x != to_x; x += step) if (board[x][from_y] != ' ') return 0;
    }
    return 1;
}
//Caii (is_valid_knight_move) se mișcă în formă de „L”.
int is_valid_knight_move(int from_x, int from_y, int to_x, int to_y) {
    int dx = abs(from_x - to_x);
    int dy = abs(from_y - to_y);
    return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
}
//Nebunii (is_valid_bishop_move) se mișcă pe diagonale.
int is_valid_bishop_move(int from_x, int from_y, int to_x, int to_y) {
    if (abs(from_x - to_x) != abs(from_y - to_y)) return 0;
    int step_x = (to_x > from_x) ? 1 : -1;
    int step_y = (to_y > from_y) ? 1 : -1;
    int x = from_x + step_x;
    int y = from_y + step_y;
    while (x != to_x) {
        if (board[x][y] != ' ') return 0;
        x += step_x;
        y += step_y;
    }
    return 1;
}
//Regina (is_valid_queen_move) combină mișcările turnului și ale nebunului.
int is_valid_queen_move(int from_x, int from_y, int to_x, int to_y) {
    return is_valid_rook_move(from_x, from_y, to_x, to_y) || is_valid_bishop_move(from_x, from_y, to_x, to_y);
}
//Regii (is_valid_king_move) se mișcă doar un pas într-o direcție.
int is_valid_king_move(int from_x, int from_y, int to_x, int to_y) {
    return abs(from_x - to_x) <= 1 && abs(from_y - to_y) <= 1;
}



// Functie care verifica daca un patrat este atacat de o culoare
int is_square_attacked(int target_x, int target_y, int attacker_is_white) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char piece = board[i][j];
            if (piece == ' ') continue;

            // Verifica daca piesa apartine atacatorului specificat (folosind int 0/1)
            int piece_is_attacker = (attacker_is_white && isupper(piece)) || (!attacker_is_white && islower(piece));

            if (piece_is_attacker) {
                if (is_valid_move(i, j, target_x, target_y)) {
                    return 1; // Da, este atacat
                }
            }
        }
    }
    return 0; // Nu, patratul nu este atacat de acea culoare
}


// Functie generala de validare a mutarii (fara verificarea "lasa regele in sah")
int is_valid_move(int from_x, int from_y, int to_x, int to_y) {
    if (from_x < 0 || from_x >= SIZE || from_y < 0 || from_y >= SIZE ||
        to_x < 0 || to_x >= SIZE || to_y < 0 || to_y >= SIZE) {
        return 0;
    }

    char piece = board[from_x][from_y];
    char dest_piece = board[to_x][to_y];

    if (piece == ' ') return 0;
    if (from_x == to_x && from_y == to_y) return 0;

    if (dest_piece != ' ' && ((isupper(piece) && isupper(dest_piece)) || (islower(piece) && islower(dest_piece)))) {
        return 0;
    }

    switch (piece) {
        case 'P': return is_valid_pawn_move(from_x, from_y, to_x, to_y, -1);
        case 'p': return is_valid_pawn_move(from_x, from_y, to_x, to_y, 1);
        case 'T': case 't': return is_valid_rook_move(from_x, from_y, to_x, to_y);
        case 'C': case 'c': return is_valid_knight_move(from_x, from_y, to_x, to_y);
        case 'N': case 'n': return is_valid_bishop_move(from_x, from_y, to_x, to_y);
        case 'R': case 'r': return is_valid_queen_move(from_x, from_y, to_x, to_y);
        case 'K': case 'k': return is_valid_king_move(from_x, from_y, to_x, to_y);
        default: return 0;
    }
}


// Verifica daca regele jucatorului specificat este in sah (returneaza 1 daca da, 0 daca nu)
int is_in_check(int player_is_white) {
    int king_x = -1, king_y = -1;
    char king_char = player_is_white ? 'K' : 'k';

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == king_char) {
                king_x = i;
                king_y = j;
                break;
            }
        }
        if (king_x != -1) break;
    }

    if (king_x == -1) {
        printf("EROARE: Regele %s nu a fost gasit pe tabla!\n", player_is_white ? "Alb" : "Negru");
        return 0; // Regele nu e pe tabla (eroare), deci nu e in sah tehnic
    }

    return is_square_attacked(king_x, king_y, !player_is_white);
}

// Verifica daca o anumita mutare este "legala" (returneaza 1 daca da, 0 daca nu)
int is_legal_move(int from_x, int from_y, int to_x, int to_y, int player_is_white) {
    char piece_to_move = board[from_x][from_y];
    char original_target_piece = board[to_x][to_y];
    int is_legal_flag; 

    // 1. Simuleaza mutarea
    board[to_x][to_y] = piece_to_move;
    board[from_x][from_y] = ' ';

    // 2. Verifica daca regele jucatorului care A MUTAT este in sah ACUM
    int leaves_king_in_check = is_in_check(player_is_white); // Rezultatul este 0 sau 1

    // 3. Anuleaza simularea (restaureaza tabla)
    board[from_x][from_y] = piece_to_move;
    board[to_x][to_y] = original_target_piece;

    // Mutarea este legala daca NU lasa regele in sah (!0 inseamna 1, !1 inseamna 0)
    is_legal_flag = !leaves_king_in_check;

    return is_legal_flag;
}

// Verifica daca jucatorul specificat are *macar o* mutare legala (returneaza 1 daca da, 0 daca nu)
int has_any_legal_move(int player_is_white) {
    for (int from_x = 0; from_x < SIZE; from_x++) {
        for (int from_y = 0; from_y < SIZE; from_y++) {
            char piece = board[from_x][from_y];
            if (piece == ' ') continue;

            // Verifica daca piesa apartine jucatorului curent
            int piece_belongs_to_player = (player_is_white && isupper(piece)) || (!player_is_white && islower(piece));

            if (piece_belongs_to_player) {
                for (int to_x = 0; to_x < SIZE; to_x++) {
                    for (int to_y = 0; to_y < SIZE; to_y++) {
                        // Pas 1: Verifica daca mutarea este valida conform regulilor piesei
                        if (is_valid_move(from_x, from_y, to_x, to_y)) {
                            // Pas 2: Verifica daca mutarea este legala (nu lasa regele in sah)
                            if (is_legal_move(from_x, from_y, to_x, to_y, player_is_white)) {
                                return 1; // Am gasit cel putin o mutare legala
                            }
                        }
                    }
                }
            }
        }
    }
    // Daca am ajuns aici, nu am gasit nicio mutare legala
    return 0;
}



// Citire si Efectuare Mutare 
int read_move(int *from_x, int *from_y, int *to_x, int *to_y) {
    char input_buffer[MAX_INPUT_LEN];
    char from_col_char, from_row_char, to_col_char, to_row_char;

    printf("\n[%s] Introduceti mutarea (ex: e2 e4) sau 'q' pentru a iesi: ", white_turn ? "Alb" : "Negru");

    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
        printf("Eroare la citirea inputului.\n");
        return 0;
    }
    input_buffer[strcspn(input_buffer, "\n")] = 0;

    if (strcmp(input_buffer, "q") == 0) return -1;

    if (sscanf(input_buffer, "%c%c%*[ ]%c%c", &from_col_char, &from_row_char, &to_col_char, &to_row_char) != 4) {
        printf("Format invalid! Introduceti mutarea corect (ex: e2 e4 sau e2e4).\n");
        return 0;
    }

    if (from_col_char < 'a' || from_col_char > 'h' || to_col_char < 'a' || to_col_char > 'h' ||
        from_row_char < '1' || from_row_char > '8' || to_row_char < '1' || to_row_char > '8') {
        printf("Coordonate invalide! Folositi a-h si 1-8 (ex: e2 e4).\n");
        return 0;
    }

    *from_x = 8 - (from_row_char - '0');
    *from_y = from_col_char - 'a';
    *to_x   = 8 - (to_row_char - '0');
    *to_y   = to_col_char - 'a';

    if (*from_x < 0 || *from_x >= SIZE || *from_y < 0 || *from_y >= SIZE ||
        *to_x < 0 || *to_x >= SIZE || *to_y < 0 || *to_y >= SIZE) {
         printf("Eroare interna la conversia coordonatelor.\n");
         return 0;
    }

    return 1;
}

int make_move() {
    int from_x, from_y, to_x, to_y;
    int read_status = read_move(&from_x, &from_y, &to_x, &to_y);

    if (read_status <= 0) return read_status;

    char piece_to_move = board[from_x][from_y];

    if (piece_to_move == ' ') {
        printf("Mutare invalida! Nu exista piesa la %c%c.\n", from_y + 'a', 8 - from_x + '0');
        return 0;
    }
    if ((white_turn && islower(piece_to_move)) || (!white_turn && isupper(piece_to_move))) {
        printf("Mutare invalida! Este randul jucatorului %s.\n", white_turn ? "Alb" : "Negru");
        return 0;
    }

    if (!is_valid_move(from_x, from_y, to_x, to_y)) {
         printf("Mutare invalida conform regulilor piesei %c! Incercati din nou.\n", piece_to_move);
         return 0;
    }

    if (!is_legal_move(from_x, from_y, to_x, to_y, white_turn)) {
         printf("Mutare ilegala! Regele %s ar fi/ramane in sah.\n", white_turn ? "Alb" : "Negru");
         return 0;
    }

    char captured_piece = board[to_x][to_y];
    board[to_x][to_y] = piece_to_move;
    board[from_x][from_y] = ' ';

    printf("Mutare valida: %c%c -> %c%c\n", from_y + 'a', 8 - from_x + '0', to_y + 'a', 8 - to_x + '0');
    if (captured_piece != ' ') {
         printf("Piesa capturata: %c\n", captured_piece);
    }

    white_turn = !white_turn;
    return 1;
}

// Functia Principala 
int main(void)
{
    initialize_board();
    int game_over = 0; 

    while(!game_over) // Continua cat timp game_over este 0
    {
        print_board();

        // Verifica starea JUCATORULUI CURENT (a carui tura urmeaza)
        int current_player_in_check = is_in_check(white_turn); // Returneaza 0 sau 1
        int current_player_can_move = has_any_legal_move(white_turn); // Returneaza 0 sau 1

        if (current_player_in_check) { // Daca este 1 (adevarat)
            if (!current_player_can_move) { // Daca este 0 (fals)
                printf("\n!!! SAH MAT !!!\n");
                printf("Jucatorul %s a castigat!\n", !white_turn ? "Alb" : "Negru");
                game_over = 1; // Seteaza la 1 (adevarat)
                continue;
            } else {
                printf("[%s] sunteti in SAH!\n", white_turn ? "Alb" : "Negru");
            }
        } else { // Nu este in sah (current_player_in_check este 0)
            if (!current_player_can_move) { // Daca este 0 (fals)
                printf("\n!!! PAT (Stalemate) !!!\n");
                printf("Remiza.\n");
                game_over = 1; // Seteaza la 1 (adevarat)
                continue;
            }
        }

        // Daca jocul nu s-a terminat, cere mutarea
        int move_result;
        do {
            move_result = make_move();
            if (move_result == -1) {
                printf("\nIesire din joc.\n");
                game_over = 1; // Seteaza la 1 (adevarat)
                break;
            }
        } while (move_result == 0);

    }

    printf("\nStarea finala a tablei:\n");
    print_board();

    return 0;
}