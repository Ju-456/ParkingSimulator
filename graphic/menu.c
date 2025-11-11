#include "window.h"

void panel() {
    DrawTexture(panel_menu, 0, 560, WHITE);
}

void init_manual_panel_menu() {
    int buttonWidth = srcMode.width;
    int buttonHeight = srcMode.height;
    int PosY = 720;

    btnRandom = (Rectangle){200, PosY, buttonWidth, buttonHeight};
    btnManual = (Rectangle){335, PosY, buttonWidth, buttonHeight};
    btnHardManual = (Rectangle){470, PosY, buttonWidth, buttonHeight};

    btnTicket = (Rectangle){780 - buttonWidth, 580, buttonWidth, buttonHeight};
    btnPay = (Rectangle){780 - buttonWidth, 650, buttonWidth, buttonHeight};

    btnReturn = (Rectangle){120, PosY, 60, 60};
}

void manual_panel_menu(Font font) {
    DrawTextureRec(PC, srcMode, (Vector2){btnRandom.x, btnRandom.y}, WHITE);
    DrawTextEx(font, "Random", (Vector2){btnRandom.x + 28, btnRandom.y + 18}, 18, 1, parkingBlue);

    DrawTextureRec(PC, srcMode, (Vector2){btnManual.x, btnManual.y}, WHITE);
    DrawTextEx(font, "Manual", (Vector2){btnManual.x + 32, btnManual.y + 18}, 18, 1, parkingGreen);

    DrawTextureRec(PC, srcMode, (Vector2){btnHardManual.x, btnHardManual.y}, WHITE);
    DrawTextEx(font, "     Hard", (Vector2){btnHardManual.x + 8, btnHardManual.y + 18}, 18, 1, parkingRed);
}