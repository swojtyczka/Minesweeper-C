#include "main.h"

FILE *records;
record RECORDS[10] = {0};

void PrepHighscores()
{
	// Wczytanie rekordow, utworzenie domyslnego pliku w przypadku braku
	records = fopen("highscores.txt", "r");
	if (records != NULL)
	{
		char buff[11];
		for (int i = 0; i < 10; i++)
		{
			int m, s;
			if (fscanf(records, "%d %d %s\n", &m, &s, buff) != 3)
				exit(1);
			RECORDS[i] = (record){.minutes = m, .seconds = s};
			strcpy(RECORDS[i].nick, buff);
		}
	}
	else
	{
		records = fopen("highscores.txt", "w");
		for (int i = 0; i < 10; i++)
		{
			fprintf(records, "999 999 none\n");
			RECORDS[i] = (record){.minutes = 999, .seconds = 999, .nick = "none"};
		}
	}
}

void DrawHighscores()
{
	if (IsKeyPressed(KEY_ESCAPE))
		SCENE = menu;

	ClearBackground(GRAY);

	DrawText("Press ESC to go back to menu", (screenWidth - MeasureText("Press ESC to go back to menu", 25)) * 0.5, screenHeight * 0.05, 25, BLACK);
	DrawText("HIGHSCORES", (screenWidth - MeasureText("HIGHSCORES", 100)) * 0.5, screenHeight * 0.1, 100, BLACK);

	// Wypisywanie rekordow
	for (int i = 0; i < 10; i++)
	{
		char rekordzik[100];
		sprintf(rekordzik, "%02d:%02d - %s", RECORDS[i].minutes, RECORDS[i].seconds, RECORDS[i].nick);
		DrawText(rekordzik, (screenWidth - MeasureText(rekordzik, 30)) * 0.5, screenHeight * 0.2 + 60 * (i + 1), 30, BLACK);
	}
}

void CleanHighscores()
{
	fclose(records);
}