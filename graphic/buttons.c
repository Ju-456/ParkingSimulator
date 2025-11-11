#include "window.h"

void draw_buttons_direction(Texture2D PC) {
    Rectangle srcUp = {129, 320, 60, 60};
    Rectangle srcDown = {192, 320, 60, 60};

    Rectangle srcLeft = {256, 320, 60, 60};
    Rectangle srcRight = {325, 320, 60, 60};

    Vector2 posUp = {440, 660};
    Vector2 posDown = {440, 720};

    Vector2 posLeft = {posDown.x - 60, posDown.y};
    Vector2 posRight = {posDown.x + 65, posDown.y};

    DrawTextureRec(PC, srcUp, posUp, WHITE);
    DrawTextureRec(PC, srcDown, posDown, WHITE);
    DrawTextureRec(PC, srcLeft, posLeft, WHITE);
    DrawTextureRec(PC, srcRight, posRight, WHITE);
}

void draw_floor_arrows(Texture2D PC, Rectangle srcArrow, Rectangle prev, Rectangle next, int floor, bool enabled) {
    Vector2 origin = {srcArrow.width / 2.0f, srcArrow.height / 2.0f};

    bool canUp = enabled && (floor < MAX_FLOOR);
    bool canDown = enabled && (floor > 0);

    Color upTint = canUp ? WHITE : Fade(GRAY, 0.5f);
    Color downTint = canDown ? WHITE : Fade(GRAY, 0.5f);

    DrawTexturePro(PC, srcArrow, prev, origin, 0.0f, upTint);
    DrawTexturePro(PC, srcArrow, next, origin, 180.0f, downTint);
}

void draw_return_arrow(Texture2D PC, Rectangle srcReturn, Rectangle destReturn, bool enabled) {

    Color returnColor = enabled ? brightBlue : Fade(GRAY, 0.5f);
    DrawTexturePro(PC, srcReturn, destReturn, origin1, 0, returnColor);
}

void draw_ticket_pay_buttons(Font font, bool enabled) {
    bool canUse = (currentFloor == 0) && enabled;
    Color btnTint = disabled_tint(WHITE, canUse);
    Color textTint1 = disabled_tint(parkingBlue, canUse);
    Color textTint2 = disabled_tint(parkingRed, canUse);

    DrawTextureRec(PC, srcMode, (Vector2){btnTicket.x, btnTicket.y}, btnTint);
    DrawTextEx(font, "Ticket", (Vector2){btnTicket.x + 32, btnTicket.y + 18}, 18, 1, textTint1);

    DrawTextureRec(PC, srcMode, (Vector2){btnPay.x, btnPay.y}, btnTint);
    DrawTextEx(font, "  Pay", (Vector2){btnPay.x + 35, btnPay.y + 18}, 18, 1, textTint2);
}

void handle_station_buttons_click(Vector2 mouse, bool enabled) {
    if (!enabled)
        return;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (currentFloor != 0) {
            return;
        }

        if (CheckCollisionPointRec(mouse, btnTicket)) {
            if (ticket == 0) {
                ticket = 1;
                entranceTriggerTime = GetTime();
            }
        }

        if (CheckCollisionPointRec(mouse, btnPay)) {
            if (ticket == 0) {
                fflush(stdout);
            } else if (payment == 0) {
                payment = 1;
                exitTriggerTime = GetTime();
            }
        }
    }
}