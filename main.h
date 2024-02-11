#pragma once
#ifndef MAIN
#define MAIN
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Stałe i pomocnicze typy danych
#define screenWidth 1000
#define screenHeight 1000

// Enum oznaczajacy obecny stan programu
typedef enum
{
	menu,
	highscores,
	game,
	gameWon,
	gameLost,
	quit
} scene;
extern scene SCENE;

// Pojedynczy rekord w tabeli rekordow
typedef struct record
{
	int minutes, seconds;
	char nick[11];
} record;
// Globalna tablica rekordow
extern record RECORDS[10];

// Funkcje przygotowania, rysowania i czyszczenia dla wszystkich scen
void PrepMenu();
void DrawMenu();
void CleanMenu();

void PrepGame();
void DrawGame();
void DrawGameLost();
void DrawGameWon();
void CleanGame();

void PrepHighscores();
void DrawHighscores();
void CleanHighscores();

#endif