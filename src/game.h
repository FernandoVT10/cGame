#ifndef GAME_H
#define GAME_H

#include <stddef.h>
#include "raylib.h"

typedef struct {
    Vector2 pos;
    Vector2 vel;

    bool isOnFloor;
    bool jumping;
    float jumpTime;
    bool wallJumping;

    bool isDashing;
    float dashTime;

    bool huggingWall;
} Player;

typedef struct {
    Rectangle rec;
} Collider;

typedef struct {
    Collider *items;
    size_t count;
    size_t capacity;
} Colliders;

typedef struct {
    Rectangle *items;
    size_t count;
    size_t capacity;
} Platforms;

typedef struct {
    Colliders colliders;
    Platforms platforms;
    Player player;
    Camera2D camera;
} Game;

#endif // GAME_H
