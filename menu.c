#include "main.h"

// Dzwieki i tekstury przyciskow
Sound button_press;
Texture2D playBtn, highscoresBtn, quitBtn, background;
float frameHeight;

// Przycisk
typedef struct Button
{
	Texture2D texture;
	Rectangle sourceRec;
	Rectangle boundsRec;
	enum btnState
	{
		NORMAL,
		HOVERED,
		PRESSED
	} state;
	bool action;
	scene clickAction;
} Button;

Vector2 mousePoint;

Button buttons[3];

void PrepMenu()
{
	// Trzy stale przyciski na ekranie glownym
	playBtn = LoadTexture("assets/img/play.png");
	highscoresBtn = LoadTexture("assets/img/highscores.png");
	quitBtn = LoadTexture("assets/img/quit.png");
	background = LoadTexture("assets/img/bg.png");
	frameHeight = (float)playBtn.height / 3;

	buttons[0] = (Button){
		.texture = playBtn,
		.sourceRec = (Rectangle){0, 0, (float)playBtn.width, frameHeight},
		.boundsRec = (Rectangle){screenWidth / 2.0f - playBtn.width / 2.0f, screenHeight / 2.0f - playBtn.height / 3 / 2.0f, (float)playBtn.width, frameHeight},
		.state = 0,
		.action = false,
		.clickAction = game};

	buttons[1] = (Button){
		.texture = highscoresBtn,
		.sourceRec = (Rectangle){0, 0, (float)highscoresBtn.width, frameHeight},
		.boundsRec = (Rectangle){screenWidth / 2.0f - highscoresBtn.width / 2.0f, buttons[0].boundsRec.y + 150, (float)highscoresBtn.width, frameHeight},
		.clickAction = highscores,
		.state = 0,
		.action = false};

	buttons[2] = (Button){
		.texture = quitBtn,
		.sourceRec = (Rectangle){0, 0, (float)quitBtn.width, frameHeight},
		.boundsRec = (Rectangle){screenWidth / 2.0f - quitBtn.width / 2.0f, buttons[1].boundsRec.y + 150, (float)quitBtn.width, frameHeight},
		.clickAction = quit,
		.state = 0,
		.action = false};

	button_press = LoadSound("assets/snd/button_press.mp3");

	mousePoint = (Vector2){0, 0};
}

void DrawMenu()
{
	ClearBackground(DARKGRAY);
	// Tlo
	DrawTextureEx(background, (Vector2){0, 0}, 0, 1, WHITE);

	// Tytul
	DrawText("Minesweeper", (screenWidth - MeasureText("Minesweeper", 120)) * 0.5, screenHeight * 0.25, 120, WHITE);

	mousePoint = GetMousePosition();

	for (int i = 0; i < 3; i++)
	{
		// Akcje najechania na i wcisniecia przyciskow
		Button *me = &buttons[i];
		if (CheckCollisionPointRec(mousePoint, me->boundsRec))
		{
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
				me->state = 2;
			else
				me->state = 1;

			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
				me->action = true;
		}
		else
			me->state = 0;

		if (me->action)
		{
			PlaySound(button_press);
			SCENE = me->clickAction;
			me->action = false;
		}

		// Jedna tekstura zawiera 3 obrazki przyciskow, brana jest zawsze odpowiednia 1/3 tekstury
		me->sourceRec.y = me->state * frameHeight;

		DrawTextureRec(me->texture, me->sourceRec, (Vector2){me->boundsRec.x, me->boundsRec.y}, WHITE);
	}
}

void CleanMenu()
{
	for (int i = 0; i < 3; i++)
		UnloadTexture(buttons[i].texture);

	UnloadTexture(background);

	UnloadSound(button_press);
}