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

    Game game = {
        .camera = {
            .zoom = 1,
        },
        .player = {
            .dir = 1,
        },
    };

    da_append(&game.platforms, ((Rectangle){
        .x = 1200,
        .y = -120,
        .width = 80,
        .height = 850,
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


    da_append(&game.platforms, ((Rectangle){
        .x = 800,
        .y = 200,
        .width = 200,
        .height = 80,
    }));

    da_append(&game.platforms, ((Rectangle){
        .x = 600,
        .y = 500,
        .width = 10,
        .height = 220,
    }));

    for(size_t i = 0; i < game.platforms.count; i++) {
        da_append(&game.colliders, ((Collider){
            .rec = game.platforms.items[i],
        }));
    }

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        game.camera.target.x = 0;

        if(game.player.pos.y < 360) {
            game.camera.target.y = game.player.pos.y - 360;
        } else {
            game.camera.target.y = 0;
        }

        BeginMode2D(game.camera);
        player_update(&game);
        platforms_draw(game.platforms);
        EndMode2D();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
