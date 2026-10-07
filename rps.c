#include <stdlib.h>   
#include <time.h>     
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main(void)
{
    srand(time(NULL));
    
    int screenWidth = 600;
    int screenHeight = 400;

    InitWindow(screenWidth, screenHeight, "raylib + raygui example - rock paper scissors");
    SetTargetFPS(60);

    int screenCenterX = GetScreenWidth() / 2;
    int screenCenterY = GetScreenHeight() / 2;

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    char * names[3] = { "Rock", "Paper", "Scissors" };
    int playerChoice = -1;
    int computerChoice = -1;

    char * result = "Pick a move!";

    int wins = 0;
    int losses = 0;
    int draws = 0;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        char * title = "Rock Paper Scissors";
        int titleX = screenCenterX - MeasureText(title, 30) / 2;

        DrawText(title, titleX, 20, 30, DARKBLUE);

        char * resultCounts = TextFormat("Wins: %d\tLosses: %d\tDraws: %d", wins, losses, draws);
        int resultCountsX = screenCenterX - MeasureText(resultCounts, 20) / 2;

        DrawText(resultCounts, resultCountsX, 70, 20, DARKGRAY);

        if (playerChoice >= 0)
        {
            DrawText(TextFormat("You chose: %s", names[playerChoice]), 40, 130, 24, BLACK);
            DrawText(TextFormat("Computer chose: %s", names[computerChoice]), 40, 165, 24, BLACK);
        }

        DrawText(result, 40, 220, 36, MAROON);

        for (int i = 0; i < 3; i++)
        {
            Rectangle button = { 30 + i * 190, 300, 160, 60 };

            if (GuiButton(button, names[i]))
            {
                playerChoice = i;
                computerChoice = rand() % 3;  

                if (playerChoice == computerChoice)
                {
                    result = "It's a draw!";
                    draws++;
                }
                else if ((playerChoice == 0 && computerChoice == 2) ||   // Rock beats Scissors
                         (playerChoice == 1 && computerChoice == 0) ||   // Paper beats Rock
                         (playerChoice == 2 && computerChoice == 1))     // Scissors beats Paper
                {
                    result = "You win!";
                    wins++;
                }
                else
                {
                    result = "You lose!";
                    losses++;
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}