#include "main.h"

scene SCENE = menu;

int main()
{
    InitWindow(screenWidth, screenHeight, "Minesweeper");
    InitAudioDevice();

    PrepMenu();
    PrepHighscores();
    PrepGame();

    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    // Glowna petla programu

    while (SCENE != quit && !WindowShouldClose())
    {
        BeginDrawing();

        // Rysowanie odpowiednich scen

        switch (SCENE)
        {
        case highscores:
            DrawHighscores();
            break;

        case gameLost:
            DrawGame();
            DrawGameLost();
            break;

        case gameWon:
            DrawGame();
            DrawGameWon();
            break;

        case game:
            DrawGame();
            break;

        case menu:
        default:
            DrawMenu();
            break;
        }

        EndDrawing();
    }

    // Czysczenie, zwalnianie pamieci itp
    
    CleanMenu();
    CleanGame();
    CleanHighscores();

    CloseAudioDevice();
    CloseWindow();

    return 0;
}