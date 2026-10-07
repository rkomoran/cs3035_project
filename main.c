
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <stdlib.h>   // rand, srand
#include <time.h>     // time

int main(void)
{
    // Create the window
    InitWindow(600, 400, "Rock Paper Scissors");
    SetTargetFPS(60);

    // Seed the random number generator so the computer's choice differs each run
    srand(time(NULL));

    // Make the raygui text a bit bigger
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    // Choices: 0 = Rock, 1 = Paper, 2 = Scissors, -1 = nothing picked yet
    const char *names[3] = { "Rock", "Paper", "Scissors" };
    int playerChoice = -1;
    int computerChoice = -1;

    const char *result = "Pick a move!";
    int wins = 0;
    int losses = 0;
    int draws = 0;

    // Game loop: runs until the window is closed
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Title and score
        DrawText("Rock Paper Scissors", 150, 20, 30, DARKBLUE);
        DrawText(TextFormat("Wins: %d    Losses: %d    Draws: %d", wins, losses, draws), 130, 70, 20, DARKGRAY);

        // Show what each side picked
        if (playerChoice >= 0)
        {
            DrawText(TextFormat("You chose: %s", names[playerChoice]), 180, 130, 24, BLACK);
            DrawText(TextFormat("Computer chose: %s", names[computerChoice]), 180, 165, 24, BLACK);
        }

        // Show the result
        DrawText(result, 180, 220, 36, MAROON);

        // Three buttons, one for each choice
        for (int i = 0; i < 3; i++)
        {
            Rectangle button = { 30 + i * 190, 300, 160, 60 };

            // GuiButton returns true when the button is clicked
            if (GuiButton(button, names[i]))
            {
                playerChoice = i;
                computerChoice = rand() % 3;   // random number 0, 1 or 2

                // Work out who won
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