#include "player.h"

#define PLAYER_GRAVITY 2000 // the force in which the player is pulled down
#define PLAYER_MAX_FALL_SPEED 1000 // max vertical speed caused by gravity
#define PLAYER_FALL_SPEED_WHEN_HUGGING_WALL 300
#define PLAYER_JUMP_DURATION 0.15 // for how long the player can press the jump button
#define PLAYER_JUMP_FORCE 8000 // the force of the jump
#define PLAYER_HORIZONTAL_SPEED 1000 // movement speed
#define PLAYER_DASH_SPEED 5000 // the speed of the dash
#define PLAYER_DASH_DURATION 0.1 // duration of the dash

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static void handle_jump(Player *player) {
    float dt = GetFrameTime();

    bool keyPressed = IsKeyDown(KEY_Z);

    if(player->huggingWall && keyPressed) {
        player->vel.x = -20000;
        return;
    }

    if(keyPressed && player->canJump) {
        player->jumping = true;
        player->canJump = false;
    }

    if(player->jumping && keyPressed) {
        player->jumpTime += dt;
        player->vel.y -= PLAYER_JUMP_FORCE * dt;

        if(player->jumpTime > PLAYER_JUMP_DURATION) {
            player->jumping = false;
        }
    } else if(player->jumping) {
        player->jumping = false;
    }
}

static void handle_horizontal_movement(Player *player) {
    if(player->isDashing) return;

    if(IsKeyDown(KEY_RIGHT)) {
        player->vel.x = PLAYER_HORIZONTAL_SPEED;
    } else if(IsKeyDown(KEY_LEFT)) {
        player->vel.x = -PLAYER_HORIZONTAL_SPEED;
    } else if(player->vel.x != 0) {
        player->vel.x = 0;
    }
}

static void handle_dash(Player *player) {
    float dt = GetFrameTime();

    if(IsKeyPressed(KEY_C)) {
        player->isDashing = true;
        // if the vel is 0 then dash to the right
        float dir = player->vel.x >= 0 ? 1 : -1;
        player->vel.x = PLAYER_DASH_SPEED * dir;
    }

    if(player->isDashing) {
        player->dashTime += dt;
        player->vel.y = 0;

        if(player->dashTime > PLAYER_DASH_DURATION) {
            player->isDashing = false;
            player->dashTime = 0;
            player->vel.x = 0;
        }
    }
}

Collider *check_collision(Game *game) {
    Rectangle playerRec = {
        .x = game->player.pos.x,
        .y = game->player.pos.y,
        .width = 40,
        .height = 80,
    };

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
    player->pos.x += player->vel.x * dt;

    Collider *collider = check_collision(game);
    if(collider != NULL) {
        Rectangle colliderRec = collider->rec;

        player->huggingWall = true;

        if(player->vel.x > 0) {
            player->vel.x = 0;
            player->pos.x = colliderRec.x - 40;
        } else {
            player->vel.x = 0;
            player->pos.x = colliderRec.x + colliderRec.width;
        }
    } else if(player->huggingWall) {
        player->huggingWall = false;
    }
}

void player_update(Game *game) {
    static int playerHeight = 80;
    float dt = GetFrameTime();

    Player *player = &game->player;

    handle_jump(player);
    handle_horizontal_movement(player);
    handle_dash(player);

    float max = player->huggingWall ? PLAYER_FALL_SPEED_WHEN_HUGGING_WALL : PLAYER_MAX_FALL_SPEED;
    player->vel.y = MIN(max, player->vel.y + PLAYER_GRAVITY * dt);

    handle_horizontal_collision(game);

    player->pos.y += player->vel.y * dt;
    Collider *collider = check_collision(game);
    if(collider != NULL) {
        Rectangle colliderRec = collider->rec;

        if(player->vel.y > 0) {
            player->vel.y = 0;
            player->pos.y = colliderRec.y - 80;
            player->canJump = true;
            player->jumpTime = 0;
        } else {
            player->vel.y = 0;
            player->pos.y = colliderRec.y + colliderRec.height;
        }
    }

    DrawRectangle(player->pos.x, player->pos.y, 40, playerHeight, RED);
}
