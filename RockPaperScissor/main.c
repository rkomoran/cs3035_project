// Rock Paper Scissors - raylib + raygui (C)
// Hands are drawn from shapes in code, so no image files are needed.
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

#define WIN_W      720
#define WIN_H      620
#define SHAKE_TIME 0.9f     // seconds of "Rock... Paper... Scissors!"
#define OUTLINE    3.0f     // hand outline thickness (in hand units)

typedef enum { ROCK = 0, PAPER = 1, SCISSORS = 2, NONE = 3 } Move;
typedef enum { DRAW, WIN, LOSE } Outcome;

static const char *MOVE_NAMES[] = { "Rock", "Paper", "Scissors", "?" };

static const Color SKIN      = { 255, 214, 170, 255 };
static const Color SKIN_LINE = { 190, 125,  80, 255 };

// Paper(1) beats Rock(0), Scissors(2) beats Paper(1), Rock(0) beats Scissors(2)
static Outcome Judge(Move player, Move cpu)
{
    if (player == cpu) return DRAW;
    return ((player - cpu + 3) % 3 == 1) ? WIN : LOSE;
}

// ---------------------------------------------------------------------------
// Hand drawing. A hand is described in "hand units" around an origin point,
// then scaled (s) and optionally mirrored left/right (f = +1 or -1).
// ---------------------------------------------------------------------------
typedef struct { float x, y, s, f; } Hand;

static Vector2 Pt(Hand h, float x, float y)
{
    return (Vector2){ h.x + x * h.f * h.s, h.y + y * h.s };
}

// A finger: thick line with round ends. pass 0 = outline, pass 1 = fill.
static void Limb(Hand h, float x1, float y1, float x2, float y2, float t, int pass)
{
    float thick = ((pass == 0) ? t + 2 * OUTLINE : t) * h.s;
    Color c = (pass == 0) ? SKIN_LINE : SKIN;
    Vector2 a = Pt(h, x1, y1), b = Pt(h, x2, y2);
    DrawLineEx(a, b, thick, c);
    DrawCircleV(a, thick / 2, c);
    DrawCircleV(b, thick / 2, c);
}

// A rounded block (palm, fist, wrist).
static void Block(Hand h, float x, float y, float w, float ht, float round, int pass)
{
    float g = (pass == 0) ? OUTLINE : 0;
    Vector2 p1 = Pt(h, x - g, y - g), p2 = Pt(h, x + w + g, y + ht + g);
    Rectangle r = { fminf(p1.x, p2.x), fminf(p1.y, p2.y), fabsf(p2.x - p1.x), fabsf(p2.y - p1.y) };
    DrawRectangleRounded(r, round, 12, (pass == 0) ? SKIN_LINE : SKIN);
}

static void Line(Hand h, float x1, float y1, float x2, float y2)
{
    DrawLineEx(Pt(h, x1, y1), Pt(h, x2, y2), 2.0f * h.s, SKIN_LINE);
}

static void DrawHand(Move m, float x, float y, float s, float f, Color sleeve, bool arm)
{
    Hand h = { x, y, s, f };

    // Pass 0 draws every outline, pass 1 draws every fill, so the parts merge
    // into one silhouette with no lines between them.
    for (int pass = 0; pass < 2; pass++)
    {
        if (arm) Block(h, -22, 25, 44, 70, 0.3f, pass);              // wrist

        if (m == PAPER)
        {
            Block(h, -38, -12, 76, 50, 0.3f, pass);                   // palm
            Limb(h, -28, -4, -28, -52, 17, pass);                     // fingers
            Limb(h,  -9.5f, -4, -9.5f, -76, 17, pass);
            Limb(h,   9.5f, -4,   9.5f, -70, 17, pass);
            Limb(h,  28, -4,  28, -50, 17, pass);
            Limb(h, -32, 26, -66, -2, 18, pass);                      // thumb
        }
        else if (m == SCISSORS)
        {
            Block(h, -44, -16, 88, 60, 0.35f, pass);                  // fist
            Limb(h, -33, -8, -33, -8, 20, pass);                      // curled fingers
            Limb(h,  33, -8,  33, -8, 20, pass);
            Limb(h, -12, -12, -32, -84, 19, pass);                    // index
            Limb(h,  12, -12,  32, -84, 19, pass);                    // middle
        }
        else // ROCK
        {
            Block(h, -44, -38, 88, 82, 0.35f, pass);                  // fist
        }
    }

    // Details drawn on top of the fills
    if (m == ROCK)
    {
        Line(h, -22, -32, -22, -2);
        Line(h,   0, -32,   0, -2);
        Line(h,  22, -32,  22, -2);
        Line(h, -38,  -2,  38, -2);
    }
    if (m == SCISSORS)
    {
        Line(h, -22, -8, -22, 12);
        Line(h,  22, -8,  22, 12);
    }

    // Thumb folded across the fist (own outline, so it stands out)
    if (m == ROCK || m == SCISSORS)
    {
        float ty = (m == ROCK) ? 22 : 26;
        Limb(h, -34, ty, 12, ty, 19, 0);
        Limb(h, -34, ty, 12, ty, 19, 1);
    }

    // Sleeve cuff
    if (arm)
    {
        Vector2 p1 = Pt(h, -38, 46), p2 = Pt(h, 38, 160);
        Rectangle r = { fminf(p1.x, p2.x), fminf(p1.y, p2.y), fabsf(p2.x - p1.x), fabsf(p2.y - p1.y) };
        DrawRectangleRounded((Rectangle){ r.x - 3, r.y - 3, r.width + 6, r.height + 6 }, 0.2f, 8, DARKGRAY);
        DrawRectangleRounded(r, 0.2f, 8, sleeve);
    }
}

// ---------------------------------------------------------------------------
// UI helpers
// ---------------------------------------------------------------------------
static void DrawCenteredText(const char *text, int cx, int y, int size, Color c)
{
    DrawText(text, cx - MeasureText(text, size) / 2, y, size, c);
}

static void DrawCard(Rectangle r, const char *title, Color accent)
{
    DrawRectangleRounded((Rectangle){ r.x + 4, r.y + 7, r.width, r.height }, 0.08f, 12, Fade(BLACK, 0.15f));
    DrawRectangleRounded((Rectangle){ r.x - 3, r.y - 3, r.width + 6, r.height + 6 }, 0.08f, 12, accent);
    DrawRectangleRounded(r, 0.08f, 12, (Color){ 250, 252, 255, 255 });
    DrawCenteredText(title, (int)(r.x + r.width / 2), (int)r.y + 12, 22, accent);
}

static void DrawScorePill(int x, const char *label, int value, Color c)
{
    DrawRectangleRounded((Rectangle){ (float)x, 58, 130, 32 }, 0.5f, 12, c);
    DrawCenteredText(TextFormat("%s: %d", label, value), x + 65, 63, 20, WHITE);
}

int main(void)
{
    InitWindow(WIN_W, WIN_H, "Rock Paper Scissors");
    SetTargetFPS(60);
    srand((unsigned)time(NULL));

    // raygui styling
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 3);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL,    ColorToInt((Color){ 255, 255, 255, 255 }));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED,   ColorToInt((Color){ 255, 243, 205, 255 }));
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED,   ColorToInt((Color){ 255, 224, 150, 255 }));
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL,  ColorToInt((Color){  70, 110, 200, 255 }));
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt((Color){ 230, 150,  40, 255 }));
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt((Color){ 200, 110,  20, 255 }));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL,    ColorToInt((Color){  40,  60, 120, 255 }));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED,   ColorToInt((Color){  40,  60, 120, 255 }));
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED,   ColorToInt((Color){  40,  60, 120, 255 }));

    const Color BLUE_ACCENT = {  60, 110, 220, 255 };
    const Color RED_ACCENT  = { 215,  70,  70, 255 };
    const char *COUNTDOWN[] = { "Rock...", "Paper...", "Scissors!" };

    Move playerMove = NONE, cpuMove = NONE;
    Outcome outcome = DRAW;
    int wins = 0, losses = 0, draws = 0;
    bool played = false, animating = false;
    float animT = 0.0f, revealT = 1.0f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // ---- update ----
        if (animating)
        {
            animT += dt;
            if (animT >= SHAKE_TIME)
            {
                animating = false;
                played = true;
                revealT = 0.0f;
                outcome = Judge(playerMove, cpuMove);
                if (outcome == WIN) wins++;
                else if (outcome == LOSE) losses++;
                else draws++;
            }
        }
        else if (revealT < 1.0f) revealT += dt;

        // ---- draw ----
        BeginDrawing();
        DrawRectangleGradientV(0, 0, WIN_W, WIN_H, (Color){ 196, 222, 255, 255 }, (Color){ 246, 249, 255, 255 });

        DrawCenteredText("ROCK  PAPER  SCISSORS", WIN_W / 2 + 2, 16, 34, Fade(BLACK, 0.2f));
        DrawCenteredText("ROCK  PAPER  SCISSORS", WIN_W / 2, 14, 34, (Color){ 30, 60, 140, 255 });

        DrawScorePill(151, "Wins",   wins,   (Color){  46, 160,  90, 255 });
        DrawScorePill(295, "Losses", losses, (Color){ 210,  70,  70, 255 });
        DrawScorePill(439, "Draws",  draws,  (Color){ 230, 150,  40, 255 });

        // Cards with hands
        Rectangle leftCard  = {  50, 105, 280, 250 };
        Rectangle rightCard = { 390, 105, 280, 250 };
        DrawCard(leftCard,  "You",      BLUE_ACCENT);
        DrawCard(rightCard, "Computer", RED_ACCENT);
        DrawCenteredText("VS", WIN_W / 2, 215, 32, GRAY);

        Move showP = ROCK, showC = ROCK;
        float bump = 0.0f, scale = 1.35f;
        if (animating)
        {
            // Both hands pump up and down three times as fists
            bump = fabsf(sinf(animT / SHAKE_TIME * 3.0f * PI)) * 22.0f;
        }
        else if (played)
        {
            showP = playerMove;
            showC = cpuMove;
            scale *= 1.0f + 0.2f * fmaxf(0.0f, 1.0f - revealT / 0.25f);   // little "pop"
        }

        BeginScissorMode((int)leftCard.x + 3, (int)leftCard.y + 40, (int)leftCard.width - 6, (int)leftCard.height - 43);
        DrawHand(showP, leftCard.x + leftCard.width / 2, leftCard.y + 160 - bump, scale, 1.0f, BLUE_ACCENT, true);
        EndScissorMode();

        BeginScissorMode((int)rightCard.x + 3, (int)rightCard.y + 40, (int)rightCard.width - 6, (int)rightCard.height - 43);
        DrawHand(showC, rightCard.x + rightCard.width / 2, rightCard.y + 160 - bump, scale, -1.0f, RED_ACCENT, true);
        EndScissorMode();

        // Result text
        const char *msg = "Pick a move below!";
        const char *sub = "";
        Color msgColor = DARKGRAY;
        if (animating)
        {
            int idx = (int)(animT / SHAKE_TIME * 3.0f);
            if (idx > 2) idx = 2;
            msg = COUNTDOWN[idx];
            msgColor = (Color){ 30, 60, 140, 255 };
        }
        else if (played)
        {
            if (outcome == WIN)
            {
                msg = "You win!"; msgColor = DARKGREEN;
                sub = TextFormat("%s beats %s", MOVE_NAMES[playerMove], MOVE_NAMES[cpuMove]);
            }
            else if (outcome == LOSE)
            {
                msg = "You lose!"; msgColor = RED;
                sub = TextFormat("%s beats %s", MOVE_NAMES[cpuMove], MOVE_NAMES[playerMove]);
            }
            else
            {
                msg = "It's a draw!"; msgColor = ORANGE;
                sub = TextFormat("Both picked %s", MOVE_NAMES[playerMove]);
            }
        }
        DrawCenteredText(msg, WIN_W / 2, 366, 38, msgColor);
        DrawCenteredText(sub, WIN_W / 2, 410, 22, GRAY);

        // Move buttons (raygui) with a small hand icon on each
        if (animating) GuiDisable();
        for (int i = 0; i < 3; i++)
        {
            Rectangle b = { 40.0f + i * 220.0f, 442, 200, 110 };
            if (GuiButton(b, "") && !animating)
            {
                playerMove = (Move)i;
                cpuMove = (Move)(rand() % 3);
                animating = true;
                animT = 0.0f;
                played = false;
            }
            DrawHand((Move)i, b.x + b.width / 2, b.y + 52, 0.4f, 1.0f, WHITE, false);
            DrawCenteredText(MOVE_NAMES[i], (int)(b.x + b.width / 2), (int)b.y + 80, 22, (Color){ 40, 60, 120, 255 });
        }
        GuiEnable();

        if (GuiButton((Rectangle){ 280, 566, 160, 38 }, "Reset Score"))
        {
            wins = losses = draws = 0;
            playerMove = cpuMove = NONE;
            played = false;
            animating = false;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}