#include <math.h>

#include "player.h"

#define PLAYER_GRAVITY 3000 // the force in which the player is pulled down
#define PLAYER_MAX_FALL_VELOCITY 4000 // max vertical speed caused by gravity
#define PLAYER_FALL_VELOCITY_WHEN_HUGGING_WALL 200

#define PLAYER_DASH_SPEED 5000 // the speed of the dash
#define PLAYER_DASH_DURATION 0.1 // duration of the dash

#define PLAYER_HORIZONTAL_FORCE 7000
#define PLAYER_MAX_HORIZONTAL_VELOCITY 1000

#define PLAYER_JUMP_FORCE 6500
#define PLAYER_JUMP_DURATION 0.3

#define PLAYER_WIDTH 60
#define PLAYER_HEIGHT 120

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static void gravity(Player *player) {
    float dt = GetFrameTime();
    float max = player->huggingWall ? PLAYER_FALL_VELOCITY_WHEN_HUGGING_WALL : PLAYER_MAX_FALL_VELOCITY;
    player->vel.y = MIN(max, player->vel.y + PLAYER_GRAVITY * dt);
}

static void dash(Player *player) {
    if(player->huggingWall) return;

    float dt = GetFrameTime();

    if(IsKeyPressed(KEY_C)) {
        player->dashing = true;
        player->vel.x = PLAYER_DASH_SPEED * player->dir;
    }

    if(player->dashing) {
        player->dashTime += dt;
        player->vel.y = 0;

        if(player->dashTime >= PLAYER_DASH_DURATION) {
            player->dashing = false;
            player->dashTime = 0;
            player->vel.x = 0;
        }
    }
}

static void movement(Player *player) {
    if(player->dashing) return;
    float dt = GetFrameTime();

    if(IsKeyDown(KEY_RIGHT)) {
        player->vel.x += PLAYER_HORIZONTAL_FORCE * dt;
        player->dir = PLAYER_DIR_RIGHT;
    } else if(IsKeyDown(KEY_LEFT)) {
        player->vel.x -= PLAYER_HORIZONTAL_FORCE * dt;
        player->dir = PLAYER_DIR_LEFT;
    } else if(player->vel.x != 0) {
        float mul = player->vel.x > 0 ? 1 : -1;
        player->vel.x += -mul * PLAYER_HORIZONTAL_FORCE * dt;

        if(fabs(player->vel.x) <= 100) {
            player->vel.x = 0;
        }
    }

    if(player->vel.x > 0) {
        player->vel.x = MIN(PLAYER_MAX_HORIZONTAL_VELOCITY, player->vel.x);
    } else {
        player->vel.x = MAX(-PLAYER_MAX_HORIZONTAL_VELOCITY, player->vel.x);
    }
}

static void jump(Player *player) {
    float dt = GetFrameTime();

    if(IsKeyPressed(KEY_Z) && player->isOnFloor) {
        player->jumping = true;
        player->jumpTime = 0;
    }

    if(player->jumping && (IsKeyReleased(KEY_Z) || player->jumpTime >= PLAYER_JUMP_DURATION)) {
        player->jumping = false;
    }

    if(player->jumping) {
        player->jumpTime += dt;
        player->vel.y -= PLAYER_JUMP_FORCE * dt;
    }
}

Collider *check_collision(Game *game) {
    Rectangle playerRec = {
        .x = game->player.pos.x,
        .y = game->player.pos.y,
        .width = PLAYER_WIDTH,
        .height = PLAYER_HEIGHT,
    };

    for(size_t i = 0; i < game->colliders.count; i++) {
        Collider *collider = &game->colliders.items[i];
        if(CheckCollisionRecs(playerRec, collider->rec)) {
            return collider;
        }
    }

    return NULL;
}

Collider *check_collision2(Game *game, Vector2 prevPos, Vector2 pos) {
    float diff = pos.x - prevPos.x;

    Rectangle playerRec = {
        .x = diff > 0 ? prevPos.x : pos.x,
        .y = pos.y,
        .width = fabs(diff) + PLAYER_WIDTH,
        .height = PLAYER_HEIGHT,
    };

    DrawRectangleRec(playerRec, GREEN);

    for(size_t i = 0; i < game->colliders.count; i++) {
        Collider *collider = &game->colliders.items[i];
        if(CheckCollisionRecs(playerRec, collider->rec)) {
            return collider;
        }
    }

    return NULL;
}

static void handle_horizontal_collision(Game *game) {
    float dt = GetFrameTime();
    Player *player = &game->player;

    Vector2 prevPos = player->pos;

    player->pos.x += player->vel.x * dt;

    Collider *collider = check_collision2(game, prevPos, player->pos);
    if(collider != NULL) {
        Rectangle colliderRec = collider->rec;

        if(!player->jumping && !player->isOnFloor) {
            player->huggingWall = true;
        }

        if(player->vel.x > 0) {
            player->vel.x = 0;
            player->pos.x = colliderRec.x - PLAYER_WIDTH;
        } else if(player->vel.x < 0) {
            player->vel.x = 0;
            player->pos.x = colliderRec.x + colliderRec.width;
        }
    } else if(player->huggingWall) {
        player->huggingWall = false;
    }
}

static void collisions(Game *game) {
    (void) game;
}

void player_update(Game *game) {
    float dt = GetFrameTime();

    Player *player = &game->player;

    gravity(player);
    dash(player);
    movement(player);
    jump(player);
    collisions(game);

    handle_horizontal_collision(game);

    player->pos.y += player->vel.y * dt;
    Collider *collider = check_collision(game);
    player->isOnFloor = false;

    if(collider != NULL) {
        Rectangle colliderRec = collider->rec;

        if(player->vel.y > 0) {
            player->vel.y = 0;
            player->pos.y = colliderRec.y - PLAYER_HEIGHT;
            player->isOnFloor = true;
        } else if(player->vel.y < 0) {
            player->vel.y = 0;
            player->pos.y = colliderRec.y + colliderRec.height;
            player->jumping = false;
        }

        if(colliderRec.y == 500) TraceLog(LOG_INFO, "Collision!");
    }

    DrawRectangle(player->pos.x, player->pos.y, PLAYER_WIDTH, PLAYER_HEIGHT, RED);
}
