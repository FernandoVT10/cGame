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

#define DEBUG_CCD 1

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

static Rectangle get_rec_from_collider(Collider coll) {
    return (Rectangle) {
        .x = coll.x,
        .y = coll.y,
        .width = coll.width,
        .height = coll.height,
    };
}

static Collider *get_collision(Colliders colliders, Player *player) {
    Rectangle playerRec = {
        .x = player->pos.x,
        .y = player->pos.y,
        .width = PLAYER_WIDTH,
        .height = PLAYER_HEIGHT,
    };

    for(size_t i = 0; i < colliders.count; i++) {
        Collider *coll = &colliders.items[i];
        Rectangle collRec = get_rec_from_collider(colliders.items[i]);

        if(CheckCollisionRecs(collRec, playerRec)) {
            return coll;
        }
    }

    return NULL;
}

static void collision_x_axis(Game *game) {
    float dt = GetFrameTime();
    Player *player = &game->player;
    player->pos.x += player->vel.x * dt;

    Collider *coll = get_collision(game->colliders, player);
    if(coll != NULL) {
        if(!player->jumping && !player->isOnFloor) {
            player->huggingWall = true;
        }

        if(player->vel.x > 0) {
            player->vel.x = 0;
            player->pos.x = coll->x - PLAYER_WIDTH;
        } else if(player->vel.x < 0) {
            player->vel.x = 0;
            player->pos.x = coll->x + coll->width;
        }
    } else if(player->huggingWall) {
        player->huggingWall = false;
    }
}

static void collision_y_axis(Game *game) {
    float dt = GetFrameTime();
    Player *player = &game->player;
    player->pos.y += player->vel.y * dt;
    player->isOnFloor = false;

    Collider *coll = get_collision(game->colliders, player);
    if(coll != NULL) {
        if(player->vel.y > 0) {
            player->vel.y = 0;
            player->pos.y = coll->y - PLAYER_HEIGHT;
            player->isOnFloor = true;
        } else if(player->vel.y < 0) {
            player->vel.y = 0;
            player->pos.y = coll->y + coll->height;
            player->jumping = false;
        }
    }
}

void player_update(Game *game) {
    Player *player = &game->player;

    gravity(player);
    dash(player);
    movement(player);
    jump(player);

    collision_x_axis(game);
    collision_y_axis(game);

    Rectangle rec = {player->pos.x, player->pos.y, PLAYER_WIDTH, PLAYER_HEIGHT};
    DrawRectangleLinesEx(rec, 2, RED);
}
