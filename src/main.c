#include "raylib.h"
#include "game.h"
#include "player.h"
#include "utils.h"

void platforms_draw(Platforms platforms) {
    for(size_t i = 0; i < platforms.count; i++) {
        Rectangle platform = platforms.items[i];
        DrawRectangleRec(platform, BLUE);
    }
}

int main(void) {
    InitWindow(1280, 720, "C Game");
    SetTargetFPS(60);

    Game game = {0};

    da_append(&game.platforms, ((Rectangle){
        .x = 1200,
        .y = 220,
        .width = 80,
        .height = 500,
    }));

    da_append(&game.platforms, ((Rectangle){
        .x = 0,
        .y = 680,
        .width = 1200,
        .height = 40,
    }));

    da_append(&game.platforms, ((Rectangle){
        .x = 350,
        .y = 450,
        .width = 200,
        .height = 80,
    }));

    for(size_t i = 0; i < game.platforms.count; i++) {
        da_append(&game.colliders, ((Collider){
            .rec = game.platforms.items[i],
        }));
    }

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        player_update(&game);
        platforms_draw(game.platforms);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
