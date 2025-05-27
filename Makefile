# Compilatorul C
CC = gcc

# Flag-uri pentru compilator:
# -g: Adaugă simboluri de debug (util pentru gdb)
# -Wall -Wextra -pedantic: Activează majoritatea avertismentelor utile
# -std=c11: Specifică standardul C11
# $(shell sdl2-config --cflags): Obține flag-urile de include pentru SDL2
# $(shell pkg-config SDL2_ttf --cflags): Obține flag-urile de include pentru SDL2_ttf
# -I.: Include directorul curent (pentru chess_logic.h)
CFLAGS = -g -Wall -Wextra -pedantic -std=c11 $(shell sdl2-config --cflags) $(shell pkg-config SDL2_ttf --cflags) -I.

# Flag-uri pentru linker:
# $(shell sdl2-config --libs): Obține bibliotecile de link-uit pentru SDL2 (-lSDL2, -lSDL2main etc.)
# -lSDL2_image: Link-uiește biblioteca SDL2_image
# $(shell pkg-config SDL2_ttf --libs): Link-uiește biblioteca SDL2_ttf
# -lm: Link-uiește biblioteca matematică (nu este necesar pentru stdlib.h sau ctype.h)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image $(shell pkg-config SDL2_ttf --libs)

# Fișierele sursă .c
# Presupunem ca fisierul tau principal se numeste sah_grafic.c (sau main.c daca l-ai redenumit)
# Schimba "sah_grafic.c" cu numele corect al fisierului tau principal daca e diferit.
SRCS = sah_grafic.c chess_logic.c

# Fișierele obiect .o (generate automat din .c)
OBJS = $(SRCS:.c=.o)

# Numele executabilului final
TARGET = chess_game

# Comanda implicită când rulezi 'make' (prima regulă din fișier)
all: $(TARGET)

# Regula pentru a crea executabilul final ($(TARGET))
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Regulă generică pentru a crea un fișier obiect (.o) dintr-un fișier sursă (.c)
# Depinde de fișierul .c corespunzător ȘI de chess_logic.h
%.o: %.c chess_logic.h
	$(CC) $(CFLAGS) -c $< -o $@

# Daca fisierul tau principal nu include chess_logic.h direct,
# dar chess_logic.c il include, atunci regula de mai sus e ok.
# Daca ai un fisier .c care NU include chess_logic.h, dar include alte headere locale,
# ai putea avea nevoie de reguli mai specifice sau sa listezi toate headerele relevante
# ca dependente in regula generica, ex: %.o: %.c chess_logic.h alt_header.h

# Regula pentru 'make clean'
# Șterge fișierele obiect și executabilul
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets: ținte care nu reprezintă fișiere reale
.PHONY: all clean

# Verificare daca sdl2-config si pkg-config sunt disponibile (optional, dar util)
# Acest bloc va afisa un mesaj si va esua daca comenzile nu sunt gasite
# Poti comenta acest bloc daca iti cauzeaza probleme pe sistemul tau specific
#ifndef SDL2_CONFIG_CFLAGS
#    SDL2_CONFIG_CFLAGS = $(shell sdl2-config --cflags 2>/dev/null)
#    ifeq ($(SDL2_CONFIG_CFLAGS),)
#        $(error "sdl2-config --cflags failed. Make sure SDL2 development libraries are installed and sdl2-config is in your PATH.")
#    endif
#endif
#
#ifndef SDL2_CONFIG_LIBS
#    SDL2_CONFIG_LIBS = $(shell sdl2-config --libs 2>/dev/null)
#    ifeq ($(SDL2_CONFIG_LIBS),)
#        $(error "sdl2-config --libs failed. Make sure SDL2 development libraries are installed and sdl2-config is in your PATH.")
#    endif
#endif
#
#ifndef PKG_CONFIG_SDL2_TTF_CFLAGS
#    PKG_CONFIG_SDL2_TTF_CFLAGS = $(shell pkg-config SDL2_ttf --cflags 2>/dev/null)
#    ifeq ($(PKG_CONFIG_SDL2_TTF_CFLAGS),)
#        $(error "pkg-config SDL2_ttf --cflags failed. Make sure SDL2_ttf development libraries and pkg-config are installed.")
#    endif
#endif
#
#ifndef PKG_CONFIG_SDL2_TTF_LIBS
#    PKG_CONFIG_SDL2_TTF_LIBS = $(shell pkg-config SDL2_ttf --libs 2>/dev/null)
#    ifeq ($(PKG_CONFIG_SDL2_TTF_LIBS),)
#        $(error "pkg-config SDL2_ttf --libs failed. Make sure SDL2_ttf development libraries and pkg-config are installed.")
#    endif
#endif