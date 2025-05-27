#include <SDL2/SDL.h>
#include <stdio.h>
#include "chess_logic.h" // Header-ul tău pentru logica șahului
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h> // NECESAR PENTRU TEXT

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define TILE_SIZE (WINDOW_WIDTH / 8)

// Definirea stărilor jocului
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_GAMEOVER_CHECKMATE, // Ramane aici pana la inchiderea ferestrei
    GAME_STATE_GAMEOVER_STALEMATE  // Ramane aici pana la inchiderea ferestrei
} GameState;

GameState current_game_state = GAME_STATE_MENU;

// Structura pentru texturile pieselor
typedef struct {
    SDL_Texture* white_pawn;
    SDL_Texture* white_rook;
    SDL_Texture* white_knight;
    SDL_Texture* white_bishop;
    SDL_Texture* white_queen;
    SDL_Texture* white_king;
    SDL_Texture* black_pawn;
    SDL_Texture* black_rook;
    SDL_Texture* black_knight;
    SDL_Texture* black_bishop;
    SDL_Texture* black_queen;
    SDL_Texture* black_king;
} PieceTextures;

PieceTextures piece_textures;

// Variabile pentru font și culori text
TTF_Font* game_font = NULL;
SDL_Color text_color_black = {0, 0, 0, 255};       // Negru
SDL_Color text_color_white = {255, 255, 255, 255}; // Alb

// Dimensiuni și poziție buton Start
SDL_Rect start_button_rect;

// Variabile pentru drag and drop
int is_dragging = 0;
char dragged_piece_char = ' ';
int dragged_piece_original_row = -1;
int dragged_piece_original_col = -1;
SDL_Texture* dragged_piece_texture = NULL;
int current_mouse_x = 0;
int current_mouse_y = 0;


// --- Funcții de Încărcare și Eliberare Resurse ---
SDL_Texture* load_texture(const char* path, SDL_Renderer* renderer) {
    SDL_Texture* new_texture = NULL;
    SDL_Surface* loaded_surface = IMG_Load(path);
    if (loaded_surface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    } else {
        new_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
        if (new_texture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
        }
        SDL_FreeSurface(loaded_surface);
    }
    return new_texture;
}

void load_all_piece_textures(SDL_Renderer* renderer) {
    piece_textures.white_pawn   = load_texture("assets/pion_alb.png", renderer);
    piece_textures.white_rook   = load_texture("assets/tura_alb.png", renderer);
    piece_textures.white_knight = load_texture("assets/cal_alb.png", renderer);
    piece_textures.white_bishop = load_texture("assets/nebun_alb.png", renderer);
    piece_textures.white_queen  = load_texture("assets/regina_alb.png", renderer);
    piece_textures.white_king   = load_texture("assets/rege_alb.png", renderer);

    piece_textures.black_pawn   = load_texture("assets/pion_negru.png", renderer);
    piece_textures.black_rook   = load_texture("assets/tura_negru.png", renderer);
    piece_textures.black_knight = load_texture("assets/cal_negru.png", renderer);
    piece_textures.black_bishop = load_texture("assets/nebun_negru.png", renderer);
    piece_textures.black_queen  = load_texture("assets/regina_negru.png", renderer);
    piece_textures.black_king   = load_texture("assets/rege_negru.png", renderer);
}

void destroy_all_piece_textures() {
    SDL_DestroyTexture(piece_textures.white_pawn);
    SDL_DestroyTexture(piece_textures.white_rook);
    SDL_DestroyTexture(piece_textures.white_knight);
    SDL_DestroyTexture(piece_textures.white_bishop);
    SDL_DestroyTexture(piece_textures.white_queen);
    SDL_DestroyTexture(piece_textures.white_king);
    SDL_DestroyTexture(piece_textures.black_pawn);
    SDL_DestroyTexture(piece_textures.black_rook);
    SDL_DestroyTexture(piece_textures.black_knight);
    SDL_DestroyTexture(piece_textures.black_bishop);
    SDL_DestroyTexture(piece_textures.black_queen);
    SDL_DestroyTexture(piece_textures.black_king);
}


// --- Funcție Helper pentru Randare Text ---
void render_text(SDL_Renderer* renderer, const char* text, int x, int y, TTF_Font* font, SDL_Color color, int center_x) {
    if (!font) {
        // E important sa rezolvi problema cu fontul.
        // Daca fontul nu e incarcat, mesajele nu vor aparea.
        // printf("Font not loaded, cannot render text: %s\n", text); 
        return;
    }
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect dest_rect = {x, y, surface->w, surface->h};
    if (center_x) {
        dest_rect.x = x - surface->w / 2;
    }

    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


// --- Funcții de Desenare pentru Diferite Stări ---
void draw_menu_screen(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255); // Un albastru închis
    SDL_RenderClear(renderer);
    render_text(renderer, "Welcome to Chess!", WINDOW_WIDTH / 2, WINDOW_HEIGHT / 4, game_font, text_color_white, 1); // Centrat
    SDL_SetRenderDrawColor(renderer, 100, 180, 100, 255); // Verde pentru buton
    SDL_RenderFillRect(renderer, &start_button_rect);
    render_text(renderer, "Start Game", start_button_rect.x + start_button_rect.w / 2, start_button_rect.y + 15, game_font, text_color_black, 1); // Centrat pe buton
}

void draw_playing_screen(SDL_Renderer* renderer) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            SDL_Color tile_color;
            if ((row + col) % 2 == 0) {
                tile_color = (SDL_Color){238, 238, 210, 255};
            } else {
                tile_color = (SDL_Color){118, 150, 86, 255};
            }
            SDL_SetRenderDrawColor(renderer, tile_color.r, tile_color.g, tile_color.b, tile_color.a);
            SDL_Rect tile_rect = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_RenderFillRect(renderer, &tile_rect);

            if (is_dragging && row == dragged_piece_original_row && col == dragged_piece_original_col) {
                // Nu desena piesa aici, va fi la cursor
            } else {
                char piece_char = board[row][col];
                SDL_Texture* current_piece_texture = NULL;
                switch (piece_char) {
                    case 'P': current_piece_texture = piece_textures.white_pawn; break;
                    case 'T': current_piece_texture = piece_textures.white_rook; break;
                    case 'C': current_piece_texture = piece_textures.white_knight; break;
                    case 'N': current_piece_texture = piece_textures.white_bishop; break;
                    case 'R': current_piece_texture = piece_textures.white_queen; break;
                    case 'K': current_piece_texture = piece_textures.white_king; break;
                    case 'p': current_piece_texture = piece_textures.black_pawn; break;
                    case 't': current_piece_texture = piece_textures.black_rook; break;
                    case 'c': current_piece_texture = piece_textures.black_knight; break;
                    case 'n': current_piece_texture = piece_textures.black_bishop; break;
                    case 'r': current_piece_texture = piece_textures.black_queen; break;
                    case 'k': current_piece_texture = piece_textures.black_king; break;
                }
                if (current_piece_texture != NULL) {
                    SDL_RenderCopy(renderer, current_piece_texture, NULL, &tile_rect);
                }
            }
        }
    }

    if (is_dragging && dragged_piece_texture != NULL) {
        SDL_Rect dragged_rect = {
            current_mouse_x - TILE_SIZE / 2,
            current_mouse_y - TILE_SIZE / 2,
            TILE_SIZE,
            TILE_SIZE
        };
        SDL_RenderCopy(renderer, dragged_piece_texture, NULL, &dragged_rect);
    }
}

void draw_game_over_screen(SDL_Renderer* renderer, const char* message) {
    draw_playing_screen(renderer); // Arată tabla finală

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Activează blending pentru transparență
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180); // Negru, ~70% opacitate
    SDL_Rect overlay_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay_rect);

    render_text(renderer, message, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 30, game_font, text_color_white, 1); // Centrat
}


// --- Funcție de Resetare a Jocului ---
void reset_game_logic() {
    initialize_board(); // Reinitializează tabla din chess_logic.c
    white_turn = 1;    // Albul începe
    is_dragging = 0;   // Oprește orice drag activ
    dragged_piece_char = ' ';
    dragged_piece_original_row = -1;
    dragged_piece_original_col = -1;
    dragged_piece_texture = NULL;
}


// --- Funcția Principală ---
int main(int argc, char* argv[]) {
    (void)argc; // Suprimă avertismentul de parametru nefolosit
    (void)argv; // Suprimă avertismentul de parametru nefolosit

    // Inițializare SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Eroare la inițializarea SDL Video: %s\n", SDL_GetError());
        return 1;
    }

    // Inițializare SDL_image
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Inițializare SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Încarcă fontul
    // !!! ASIGURĂ-TE CĂ ACEASTĂ CALE ESTE CORECTĂ ȘI FIȘIERUL EXISTĂ ȘI ESTE VALID !!!
    game_font = TTF_OpenFont("assets/LiberationSans-Regular.ttf", 36); // Sau "assets/Roboto-Regular.ttf" etc.
    if (game_font == NULL) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        printf("Ensure 'assets/LiberationSans-Regular.ttf' (or your chosen font) exists and is a valid TTF file.\n");
        // Programul va continua, dar textul nu va fi afișat.
    }

    // Definirea geometriei butonului Start
    start_button_rect.w = 280;
    start_button_rect.h = 70;
    start_button_rect.x = WINDOW_WIDTH / 2 - start_button_rect.w / 2;
    start_button_rect.y = WINDOW_HEIGHT / 2 - start_button_rect.h / 2 + 30;

    // Crearea ferestrei
    SDL_Window* window = SDL_CreateWindow("Chess Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Eroare la crearea ferestrei: %s\n", SDL_GetError());
        if (game_font) TTF_CloseFont(game_font);
        TTF_Quit(); IMG_Quit(); SDL_Quit();
        return 1;
    }

    // Crearea renderer-ului
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Eroare la crearea renderer-ului: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        if (game_font) TTF_CloseFont(game_font);
        TTF_Quit(); IMG_Quit(); SDL_Quit();
        return 1;
    }

    load_all_piece_textures(renderer);

    SDL_Event event;
    int running = 1;
    const char* game_over_message_text = "";

    // Bucla principală a jocului
    while (running) {
        // Gestionarea evenimentelor
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0; // Singura modalitate de a ieși din starea GAME_OVER este închiderea ferestrei
            }

            // Gestionare evenimente specifică stării
            switch (current_game_state) {
                case GAME_STATE_MENU:
                    if (event.type == SDL_MOUSEBUTTONDOWN) {
                        if (event.button.button == SDL_BUTTON_LEFT) {
                            int mouse_x = event.button.x;
                            int mouse_y = event.button.y;
                            if (mouse_x >= start_button_rect.x && mouse_x <= start_button_rect.x + start_button_rect.w &&
                                mouse_y >= start_button_rect.y && mouse_y <= start_button_rect.y + start_button_rect.h) {
                                reset_game_logic();
                                current_game_state = GAME_STATE_PLAYING;
                            }
                        }
                    }
                    break;

                case GAME_STATE_PLAYING:
                    if (event.type == SDL_MOUSEBUTTONDOWN) { // START DRAG
                        if (event.button.button == SDL_BUTTON_LEFT && !is_dragging) {
                            int mouse_x_board = event.button.x / TILE_SIZE;
                            int mouse_y_board = event.button.y / TILE_SIZE;
                            if (mouse_y_board >= 0 && mouse_y_board < SIZE && mouse_x_board >= 0 && mouse_x_board < SIZE) {
                                char piece_at_click = board[mouse_y_board][mouse_x_board];
                                if (piece_at_click != ' ' && ((white_turn == 1 && isupper(piece_at_click)) || (white_turn == 0 && islower(piece_at_click)))) {
                                    is_dragging = 1;
                                    dragged_piece_char = piece_at_click;
                                    dragged_piece_original_row = mouse_y_board;
                                    dragged_piece_original_col = mouse_x_board;
                                    // Obține textura piesei
                                    switch (dragged_piece_char) {
                                        case 'P': dragged_piece_texture = piece_textures.white_pawn; break;
                                        case 'T': dragged_piece_texture = piece_textures.white_rook; break;
                                        case 'C': dragged_piece_texture = piece_textures.white_knight; break;
                                        case 'N': dragged_piece_texture = piece_textures.white_bishop; break;
                                        case 'R': dragged_piece_texture = piece_textures.white_queen; break;
                                        case 'K': dragged_piece_texture = piece_textures.white_king; break;
                                        case 'p': dragged_piece_texture = piece_textures.black_pawn; break;
                                        case 't': dragged_piece_texture = piece_textures.black_rook; break;
                                        case 'c': dragged_piece_texture = piece_textures.black_knight; break;
                                        case 'n': dragged_piece_texture = piece_textures.black_bishop; break;
                                        case 'r': dragged_piece_texture = piece_textures.black_queen; break;
                                        case 'k': dragged_piece_texture = piece_textures.black_king; break;
                                        default: dragged_piece_texture = NULL; break;
                                    }
                                    current_mouse_x = event.button.x;
                                    current_mouse_y = event.button.y;
                                }
                            }
                        }
                    } else if (event.type == SDL_MOUSEMOTION) { // DRAGGING
                        if (is_dragging) {
                            current_mouse_x = event.motion.x;
                            current_mouse_y = event.motion.y;
                        }
                    } else if (event.type == SDL_MOUSEBUTTONUP) { // END DRAG
                        if (event.button.button == SDL_BUTTON_LEFT && is_dragging) {
                            is_dragging = 0; // Oprește drag-ul
                            int to_col = event.button.x / TILE_SIZE;
                            int to_row = event.button.y / TILE_SIZE;

                            // Verifică dacă destinația e validă și legală
                            if (to_row >= 0 && to_row < SIZE && to_col >= 0 && to_col < SIZE) { // În interiorul tablei
                                if (dragged_piece_original_row != to_row || dragged_piece_original_col != to_col) { // Nu e același pătrat
                                    if (is_valid_move(dragged_piece_original_row, dragged_piece_original_col, to_row, to_col)) {
                                        if (is_legal_move(dragged_piece_original_row, dragged_piece_original_col, to_row, to_col, white_turn)) {
                                            // Mutare validă și legală
                                            perform_move_on_board(dragged_piece_original_row, dragged_piece_original_col, to_row, to_col);
                                            white_turn = !white_turn; // Schimbă tura

                                            // Verifică starea jocului pentru jucătorul curent (a cărui tură urmează)
                                            int current_player_is_in_check = is_in_check(white_turn);
                                            int current_player_has_legal_moves = has_any_legal_move(white_turn);

                                            if (current_player_is_in_check) {
                                                if (!current_player_has_legal_moves) { // Șah-mat
                                                    game_over_message_text = !white_turn ? "CHECKMATE! White wins!" : "CHECKMATE! Black wins!";
                                                    current_game_state = GAME_STATE_GAMEOVER_CHECKMATE;
                                                } else { // Doar șah
                                                    printf("GUI: %s is in CHECK!\n", white_turn ? "White" : "Black");
                                                }
                                            } else { // Nu e în șah
                                                if (!current_player_has_legal_moves) { // Pat
                                                    game_over_message_text = "STALEMATE! It's a draw.";
                                                    current_game_state = GAME_STATE_GAMEOVER_STALEMATE;
                                                }
                                            }
                                        } // else mutare ilegală (se va redesena piesa la locul ei)
                                    } // else mutare invalidă (se va redesena piesa la locul ei)
                                } // else drop pe același pătrat (se va redesena piesa la locul ei)
                            } // else drop în afara tablei (se va redesena piesa la locul ei)
                            dragged_piece_texture = NULL; // Resetează textura piesei târâte
                        }
                    }
                    break;

                case GAME_STATE_GAMEOVER_CHECKMATE:
                case GAME_STATE_GAMEOVER_STALEMATE:
                    // Nu se procesează alte evenimente în afara de SDL_QUIT (gestionat mai sus)
                    // Jocul așteaptă închiderea ferestrei de către utilizator.
                    break;
            }
        }

        // Logica de desenare
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fundal negru, va fi acoperit
        SDL_RenderClear(renderer);

        switch (current_game_state) {
            case GAME_STATE_MENU:
                draw_menu_screen(renderer);
                break;
            case GAME_STATE_PLAYING:
                draw_playing_screen(renderer);
                break;
            case GAME_STATE_GAMEOVER_CHECKMATE:
            case GAME_STATE_GAMEOVER_STALEMATE:
                draw_game_over_screen(renderer, game_over_message_text);
                break;
        }

        SDL_RenderPresent(renderer);
    }

    // Curățarea resurselor la ieșire
    if (game_font) TTF_CloseFont(game_font);
    destroy_all_piece_textures();
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}