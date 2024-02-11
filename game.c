#include "main.h"
#define gridSize 10
const int tileSize = screenHeight / gridSize;

// Tekstury planszy
Texture2D flag, bomb, unrevealed;
Texture2D numbers[9];

// Pojedyncza komorka w grze
typedef struct Tile
{
	int x;
	int y;
	bool mine;
	bool revealed;
	bool flag;
	int touchingMines;
} Tile;
// Plansza
Tile GRID[gridSize][gridSize];

int tilesRevealed = 0;
int mines = (gridSize * gridSize) / 10;

float startTime, endTime;

// Zmienne zapobiegajace nieskonczonemu wstawianiu rekordow i resetowania czasu
bool gameStarted;
bool shouldInsert;

bool isValid(int x, int y) // Sprawdzenie czy indeksy pola sa poprawne (zawieraja sie w planszy)
{
	return (x >= 0 && x < gridSize && y >= 0 && y < gridSize);
}

int IsMine(int x, int y) // Sprawdzenie czy w polu jest mina (o ile pole jest poprawne)
{
	if (!isValid(x, y))
		return 0;
	return GRID[x][y].mine;
}

int CountTouchingMines(int x, int y) // Liczenie ile dotykajacych pol ma miny
{
	if (x < 0 || x >= gridSize || y < 0 || y >= gridSize)
		return 0;
	int ans = GRID[x][y].mine;
	for (int offsetX = -1; offsetX <= 1; offsetX++)
		for (int offsetY = -1; offsetY <= 1; offsetY++)
			if (offsetX != 0 || offsetY != 0)
				ans += IsMine(x + offsetX, y + offsetY);

	return ans;
}

void DrawTileTexture(int x, int y, Texture2D t) // Rysowanie pojedynczej tekstury
{
	DrawTextureEx(t, (Vector2){tileSize * x + (screenWidth - tileSize * gridSize), tileSize * y}, 0, (float)tileSize / (float)t.width, WHITE);
}

void DrawTile(Tile *tile) // Rysowanie pojedynczej kratki
{
	if (tile->revealed)
	{
		if (tile->mine)
			DrawTileTexture(tile->x, tile->y, bomb);
		else
			DrawTileTexture(tile->x, tile->y, numbers[tile->touchingMines]);
	}
	else if (tile->flag)
		DrawTileTexture(tile->x, tile->y, flag);
	else
		DrawTileTexture(tile->x, tile->y, unrevealed);
}

void FloodTiles(int x, int y); // Funkcja umozliwiajaca "rozlewanie sie" po odkryciu pustego pola

void RevealTile(Tile *tile) // Odkrycie pola na planszy
{
	if (tile->revealed)
		return;
	tile->revealed = true;

	if (tile->mine)
	{
		for (int i = 0; i < gridSize; i++)
			for (int j = 0; j < gridSize; j++)
				GRID[i][j].revealed = true;
		SCENE = gameLost;
		endTime = GetTime();
	}
	else
	{
		tilesRevealed++;
		if (tile->touchingMines == 0)
			FloodTiles(tile->x, tile->y);
	}
	if (tilesRevealed == gridSize * gridSize - mines)
	{
		for (int i = 0; i < gridSize; i++)
			for (int j = 0; j < gridSize; j++)
				GRID[i][j].revealed = true;
		SCENE = gameWon;
		endTime = GetTime();
		shouldInsert = true;
	}
}

void FloodTiles(int x, int y)
{
	for (int offsetX = -1; offsetX <= 1; offsetX++)
		for (int offsetY = -1; offsetY <= 1; offsetY++)
			if (offsetX != 0 || offsetY != 0)
				if (isValid(x + offsetX, y + offsetY))
					if (!GRID[x + offsetX][y + offsetY].revealed)
						RevealTile(&GRID[x + offsetX][y + offsetY]);
}

bool RecordComp(record *r1, record *r2) // Zwraca true jesli pierwszy rekord jest mniejszy od drugiego (w sensie czasu)
{
	if (r1 == r2)
		return true;
	if (r1->minutes < r2->minutes)
		return true;
	if (r2->minutes < r1->minutes)
		return false;
	return (r1->seconds < r2->seconds);
}

void InsertRecord(record *add, int index) // Wstawia rekord to tablicy rekordow na wskazana pozycje
{
	// Przesuwanie sufiksu
	for (int i = 9; i > index; i--)
		RECORDS[i] = RECORDS[i - 1];

	RECORDS[index] = *add;

	// Pisanie do pliku
	FILE *file;
	file = fopen("highscores.txt", "r+");
	for (int i = 0; i < 10; i++)
		fprintf(file, "%d %d %s\n", RECORDS[i].minutes, RECORDS[i].seconds, RECORDS[i].nick);
	fclose(file);
}

void AddRecord(record *add) // Dodawanie rekordu, jesli to konieczne
{
	// Szukanie pozycji dla nowego rekordu
	int pos = 0;
	while (!RecordComp(add, &RECORDS[pos]) && pos < 10)
		pos++;
	if (pos < 10)
		InsertRecord(add, pos);
}

void PrepGame() // Przygotowanie gry
{
	// Wczytywanie tekstur
	flag = LoadTexture("assets/img/flag.png");
	bomb = LoadTexture("assets/img/bomb.png");
	unrevealed = LoadTexture("assets/img/tile.png");

	char num[20];
	for (int i = 0; i < 9; i++)
	{
		sprintf(num, "assets/img/%d.png", i);
		numbers[i] = LoadTexture(num);
	}

	// Wypelnianie planszy pustymi polami
	tilesRevealed = 0;
	for (int i = 0; i < gridSize; i++)
		for (int j = 0; j < gridSize; j++)
			GRID[i][j] = (Tile){.x = i, .y = j, .revealed = false, .mine = false, .flag = false};

	// Losowe ustawianie min
	int q = mines;
	while (q)
	{
		int i = rand() % gridSize;
		int j = rand() % gridSize;

		if (!GRID[i][j].mine)
		{
			GRID[i][j].mine = true;
			q--;
		}
	}

	// Obliczanie liczby sasiadujacych min dla kazdego pola
	for (int i = 0; i < gridSize; i++)
		for (int j = 0; j < gridSize; j++)
			GRID[i][j].touchingMines = CountTouchingMines(i, j);

	gameStarted = false;
	shouldInsert = false;
}

Vector2 mousePos;

void DrawGame()
{
	// Jednorazowe wystartowanie zegara (nie w kazdej klatce!)
	if (SCENE == game && !gameStarted)
	{
		gameStarted = true;
		startTime = GetTime();
	}

	ClearBackground(LIGHTGRAY);

	// Rysowanie planszy
	for (int i = 0; i < gridSize; i++)
		for (int j = 0; j < gridSize; j++)
			DrawTile(&GRID[i][j]);

	// Klikniecie lewym i prawym klawiszem myszy
	bool leftClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT), rightClick = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
	if (leftClick || rightClick)
	{
		mousePos = GetMousePosition();
		int X = mousePos.x / tileSize, Y = mousePos.y / tileSize;
		if (isValid(X, Y))
		{
			// Lewy - odkrycie pola, prawy - flaga
			if (leftClick)
			{
				if (!GRID[X][Y].flag)
					RevealTile(&GRID[X][Y]);
			}
			else if (rightClick)
			{
				if (!GRID[X][Y].revealed)
					GRID[X][Y].flag = !GRID[X][Y].flag;
			}
		}
	}
}

void DrawGameLost() // Komunikat o przegranej
{
	// Wymiary okienka
	int w = 500, h = 300;
	const char *YouLostText = "You lost!", *RestartText = "Press R to restart";

	// Rysowanie okienka z ramkami na środku ekranu
	int posX = (screenWidth - w) / 2, posY = (screenHeight - h) / 2;
	DrawRectangle(posX, posY, w, h, Fade(LIGHTGRAY, 0.7));
	DrawRectangleLines(posX, posY, w, h, BLACK);

	// Tekst w okienku
	DrawText(YouLostText, posX + w / 2 - MeasureText(YouLostText, 75) / 2, posY + h * 0.25, 75, BLACK);
	DrawText(RestartText, posX + w / 2 - MeasureText(RestartText, 30) / 2, posY + h * 0.6, 30, BLACK);

	// Restart gry
	if (IsKeyPressed(KEY_R))
	{
		SCENE = menu;
		PrepGame();
	}
}

char textbox[11] = {};
int textboxLen = 0;

void DrawGameWon() // Komunikat o wygranej
{
	// Wymiary okienka
	int w = 500, h = 300;
	const char *YouWonText = "You won!";

	// Rysowanie okienka z ramkami na środku ekranu
	int posX = (screenWidth - w) / 2, posY = (screenHeight - h) / 2;
	DrawRectangle(posX, posY, w, h, Fade(LIGHTGRAY, 0.7));
	DrawRectangleLines(posX, posY, w, h, BLACK);

	// Tekst w okienku
	DrawText(YouWonText, posX + w / 2 - MeasureText(YouWonText, 75) / 2, posY + h * 0.1, 75, BLACK);

	// Obliczenie czasu gry
	int seconds = (int)(endTime - startTime) % 60, minutes = (int)(endTime - startTime) / 60;

	const char *TimeText = TextFormat("Your time: %02d:%02d", minutes, seconds);
	const char *TimeText2 = "Nickname (ENTER to confirm):";
	DrawText(TimeText, posX + w / 2 - MeasureText(TimeText, 30) / 2, posY + h * 0.5, 30, BLACK);
	DrawText(TimeText2, posX + w / 2 - MeasureText(TimeText2, 30) / 2, posY + h * 0.6, 30, BLACK);

	DrawText(textbox, posX + w / 2 - MeasureText(textbox, 30) / 2, posY + h * 0.8, 30, BLACK);

	// Pole na wpisanie nicku
	int key = GetCharPressed();
	bool letter = (key >= 32) && (key <= 125);
	if (letter || (IsKeyPressed(KEY_BACKSPACE)))
	{
		if (letter && textboxLen < 10)
			textbox[textboxLen++] = key;
		else if (IsKeyPressed(KEY_BACKSPACE) && textboxLen > 0)
			textbox[--textboxLen] = '\0';
	}

	// Zatwierdzenie nicku - ewentualne dodanie rekordu i restart gry
	if (IsKeyPressed(KEY_ENTER))
	{
		record new = {.minutes = minutes, .seconds = seconds};
		strcpy(new.nick, textbox);
		if (shouldInsert)
		{
			AddRecord(&new);
			shouldInsert = false;
		}
		SCENE = menu;
		PrepGame();
	}
}

void CleanGame()
{
	UnloadTexture(flag);
	UnloadTexture(bomb);
	UnloadTexture(unrevealed);
	for (int i = 0; i < 9; i++)
		UnloadTexture(numbers[i]);
}