// Easy-Shooter: a 2D shooter game made with Easy-Console

// Copyright (C) 2019 Sidali Mostefaoui

// For details see: README.md
// For LICENSE see: LICENSE.md


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "easy_console.h"

#define CONSOLE_WIDTH  120
#define CONSOLE_HEIGHT 40

#define PLAYER_BOX_W 15
#define PLAYER_BOX_H 2
#define PLAYER_SPEED 120
#define PLAYER_COOLDOWN 0.2f
#define PLAYER_LIVES_MAX 5

#define ENEMY_BOX_W 6
#define ENEMY_BOX_H 2
#define ENEMY_SPEED 30
#define ENEMY_COUNTDOWN_MAX 1.0f;

#define BULLET_SPEED 50


typedef struct PlayerInfo{
    float xPos;
    float yPos;
    float cooldown;
    int lives;
    int score;
}PlayerInfo;

typedef struct BulletInfo{
    float xPos;
    float yPos;
    int direction;
}BulletInfo;

typedef struct BulletListNode{
    BulletInfo info;
    struct BulletListNode* next;
    struct BulletListNode* prev;
}BulletListNode;
typedef BulletListNode* BulletList;

typedef struct EnemyInfo{
    float xPos;
    float yPos;
    int direction;
}EnemyInfo;

typedef struct EnemyListNode{
    EnemyInfo info;
    struct EnemyListNode* next;
    struct EnemyListNode* prev;
}EnemyListNode;
typedef EnemyListNode* EnemyList;


void enemy_spawn(EnemyList* enemyList, float xPos, float yPos, int direction) {
    EnemyListNode* enemy = malloc(1*sizeof(EnemyListNode));
    if(enemy == NULL) exit(1);

    enemy->info.xPos      = xPos;
    enemy->info.yPos      = yPos;
    enemy->info.direction = direction;

    enemy->prev = NULL;
    enemy->next = *enemyList;
    if(enemy->next != NULL) enemy->next->prev = enemy;
    *enemyList = enemy;
}

void enemy_destroy(EnemyList* enemyList, EnemyListNode* enemy) {
    if (enemy->next != NULL) enemy->next->prev = enemy->prev;
    if (enemy->prev != NULL) enemy->prev->next = enemy->next;
    if (*enemyList == enemy) *enemyList = enemy->next;
    free(enemy);
}

// collision detection between player & enemy
bool enemy_hit_player(EnemyInfo enemy, PlayerInfo player) {
    bool y_comp = (enemy.yPos + ENEMY_BOX_H >= player.yPos &&
                    enemy.yPos + ENEMY_BOX_H <= player.yPos + PLAYER_BOX_H);
    bool x_comp = (enemy.xPos + ENEMY_BOX_W >= player.xPos &&
                    enemy.xPos + ENEMY_BOX_W <= player.xPos + PLAYER_BOX_W) ||
                    (enemy.xPos >= player.xPos && enemy.xPos <= player.xPos + PLAYER_BOX_W);
    if(y_comp && x_comp) return true;
    return false;
}

void bullet_spawn(BulletList* bulletList, float xPos, float yPos, int direction) {
    BulletListNode* bullet = malloc(1*sizeof(BulletListNode));
    if(bullet == NULL) exit(1);

    bullet->info.xPos      = xPos;
    bullet->info.yPos      = yPos;
    bullet->info.direction = direction;

    bullet->prev = NULL;
    bullet->next = *bulletList;
    if(bullet->next != NULL) bullet->next->prev = bullet;
    *bulletList = bullet;
}

void bullet_destroy(BulletList* bulletList, BulletListNode* bullet) {
    if(bullet->next != NULL) bullet->next->prev = bullet->prev;
    if(bullet->prev != NULL) bullet->prev->next = bullet->next;
    if(*bulletList == bullet) *bulletList = bullet->next;
    free(bullet);
}

// collision detection between bullet & enemy
bool bullet_hit_enemy(BulletInfo bullet, EnemyInfo enemy) {
    bool y_comp = bullet.yPos >= enemy.yPos && bullet.yPos <= enemy.yPos + ENEMY_BOX_H;
    bool x_comp = (bullet.xPos >= enemy.xPos && bullet.xPos <= enemy.xPos + ENEMY_BOX_W) ||
                    (bullet.xPos + 1 >= enemy.xPos && bullet.xPos + 1 <= enemy.xPos + ENEMY_BOX_W);
    if(y_comp && x_comp) return true;
    return false;
}


int main() {
    Console console = console_init(CONSOLE_WIDTH, CONSOLE_HEIGHT);


    // show main screen
    while(!key_is_pressed(VK_SPACE) && !key_is_pressed(VK_ESCAPE)) {
        console_fill(console, BACKGROUND_BLACK | FOREGROUND_BLACK);

        char menu_text1[] = "EASY SHOOTER - Sidali Mostefaoui - 2019";
        console_draw_string(console, (CONSOLE_WIDTH - strlen(menu_text1))/2, CONSOLE_HEIGHT/2 - 1,
                            menu_text1, FOREGROUND_WHITE | BACKGROUND_BLACK);

        char menu_text2[] = "PRESS \"Q\" TO MOVE LEFT, \"D\" TO MOVE RIGHT AND \"SPACE\" TO SHOOT";
        console_draw_string(console, (CONSOLE_WIDTH - strlen(menu_text2))/2, CONSOLE_HEIGHT/2,
                            menu_text2, FOREGROUND_WHITE | BACKGROUND_BLACK);

        char menu_text3[] = "PRESS \"SPACE\" TO START OR \"ESCAPE\" TO QUIT";
        console_draw_string(console, (CONSOLE_WIDTH - strlen(menu_text3))/2, CONSOLE_HEIGHT/2 + 1,
                            menu_text3, FOREGROUND_WHITE | BACKGROUND_BLACK);

        console_set_title("EASY-SHOOTER");
        console_update(console);
    }


    PlayerInfo player = {(CONSOLE_WIDTH - PLAYER_BOX_W)/2.0f, CONSOLE_HEIGHT - PLAYER_BOX_H - 1, 0,
                         PLAYER_LIVES_MAX, 0};
    BulletList bulletList = NULL;
    EnemyList  enemyList = NULL;
    float enemy_countdown = ENEMY_COUNTDOWN_MAX;
    srand((unsigned int)time(NULL)); // random number generator initialisation(used to spawn enemies)


    // game loop
    clock_t t1 = clock();
    while(!key_is_pressed(VK_ESCAPE)) {
        /* calculates time difference between frames to keep
            mouvement independant from framerate */
        clock_t t2 = clock();
        float delta = (float)(t2 - t1) / CLOCKS_PER_SEC;
        t1 = t2;

        if(player.lives == 0)break;

        // spawn enemies and update enemy coundown
        if(enemy_countdown <= 0) {
            float xPos = rand() % (CONSOLE_WIDTH - ENEMY_BOX_W - 1);
            if(xPos == 0) xPos = 2;
            enemy_spawn(&enemyList, xPos, 0, 1);
            enemy_countdown = ENEMY_COUNTDOWN_MAX;
        }else enemy_countdown -= delta;

        // update shooting countdown
        if(player.cooldown < 0) player.cooldown = 0;
        else player.cooldown -= delta;

        // player shooting
        if(key_is_pressed(VK_SPACE) && player.cooldown == 0) {
            bullet_spawn(&bulletList, player.xPos + (PLAYER_BOX_W / 2.0f), player.yPos - 1, -1);
            player.cooldown = PLAYER_COOLDOWN;
        }

        // player mouvement
        if(key_is_pressed('Q')) player.xPos -= PLAYER_SPEED * delta;
        if(key_is_pressed('D')) player.xPos += PLAYER_SPEED * delta;
        if(player.xPos < 0) player.xPos = 0;
        if(player.xPos > CONSOLE_WIDTH - PLAYER_BOX_W) player.xPos = CONSOLE_WIDTH - PLAYER_BOX_W;

        // destroy enemys and remove life from player on collision
        for(EnemyListNode* enemy = enemyList; enemy != NULL; enemy = enemy->next)
            if(enemy_hit_player(enemy->info, player)) {
                enemy_destroy(&enemyList, enemy);
                player.lives -= 1;
                break;
            }

        // destroy ennemies and bullets on collision
        for(BulletListNode* bullet = bulletList; bullet != NULL; bullet = bullet->next)
            for(EnemyListNode* enemy = enemyList; enemy != NULL; enemy = enemy->next)
                if(bullet_hit_enemy(bullet->info, enemy->info)) {
                    bullet_destroy(&bulletList, bullet);
                    enemy_destroy(&enemyList, enemy);
                    player.score += 1;
                    break;
                }

        // update bullets
        for(BulletListNode* bullet = bulletList; bullet != NULL; bullet = bullet->next) {
            if(bullet->info.yPos <= 0) bullet_destroy(&bulletList, bullet);
            else bullet->info.yPos += bullet->info.direction * BULLET_SPEED * delta;
        }

        // update enemies
        for(EnemyListNode* enemy = enemyList; enemy != NULL; enemy = enemy->next) {
            if(enemy->info.yPos >= CONSOLE_HEIGHT - ENEMY_BOX_H){
                enemy_destroy(&enemyList, enemy);
                player.lives -= 1;
            }else enemy->info.yPos += enemy->info.direction * ENEMY_SPEED * delta;
        }

        console_fill(console, BACKGROUND_BLACK | FOREGROUND_BLACK);

        // draw player lives
        for(int i = 2; i < 2 + (player.lives * 3); i++)
            for(int j = i; j < i + 3; j++)
                console_draw_char(console, j, 1, CHAR_RECT, FOREGROUND_RED_LIGHT);

        // draw player
        for(int i = (int)player.xPos; i < (int)player.xPos + PLAYER_BOX_W; i++)
            for(int j = (int)player.yPos; j < (int)player.yPos + PLAYER_BOX_H; j++)
                console_draw_char(console, i, j, CHAR_RECT, FOREGROUND_WHITE);

        // draw enemies
        for(EnemyListNode* enemy = enemyList; enemy != NULL; enemy = enemy->next)
            for(int i = (int)enemy->info.xPos; i < (int)enemy->info.xPos + ENEMY_BOX_W; i++)
                for(int j = (int)enemy->info.yPos; j < (int)enemy->info.yPos + ENEMY_BOX_H; j++)
                    console_draw_char(console, i, j, CHAR_RECT, FOREGROUND_WHITE);

        // draw bullets
        for(BulletListNode* node = bulletList; node != NULL; node = node->next)
            console_draw_char(console, (int)node->info.xPos, (int)node->info.yPos,
                    CHAR_RECT, FOREGROUND_WHITE);

        // draw player score
        char score_str[20];
        snprintf(score_str, 20, "SCORE: %i", player.score);
        console_draw_string(console, CONSOLE_WIDTH - strlen(score_str) - 1, 1,
                            score_str, FOREGROUND_WHITE | BACKGROUND_BLACK);

        // set title & update
        char title[30];
        snprintf(title, 30, "EASY-SHOOTER FPS = %2.2f", 1.0f/delta);
        console_set_title(title);
        console_update(console);
    }


    // show game over screen
    while(!key_is_pressed(VK_ESCAPE)) {
        console_fill(console, BACKGROUND_BLACK | FOREGROUND_BLACK);

        char gameover_text1[] = "GAME OVER!";
        console_draw_string(console, (CONSOLE_WIDTH - strlen(gameover_text1))/2, CONSOLE_HEIGHT/2 - 1,
                            gameover_text1, FOREGROUND_WHITE | BACKGROUND_BLACK);

        char gameover_text2[20];
        snprintf(gameover_text2, 20, "YOUR SCORE IS %i", player.score);
        console_draw_string(console, (CONSOLE_WIDTH - strlen(gameover_text2))/2, CONSOLE_HEIGHT/2,
                            gameover_text2, FOREGROUND_WHITE | BACKGROUND_BLACK);

        char gameover_text3[] = "PRESS \"ESCAPE\" TO QUIT";
        console_draw_string(console, (CONSOLE_WIDTH - strlen(gameover_text3))/2, CONSOLE_HEIGHT/2 + 1,
                            gameover_text3, FOREGROUND_WHITE | BACKGROUND_BLACK);

        console_set_title("EASY-SHOOTER");
        console_update(console);
    }


    return EXIT_SUCCESS;
}
