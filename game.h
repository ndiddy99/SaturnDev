#ifndef GAME_H
#define GAME_H
#include "spritelist.h"

extern Uint8 numSprites; //current number of onscreen sprites
#define MAX_SPRITES 200
extern SPRITE_INFO sprites[MAX_SPRITES];
extern int lives;
#define MODE_TILEMAP 0
#define MODE_LINESCROLL 1
extern Uint8 bgMode;
void loadLevel(Uint16 map[]);
void loadSpritePos(Uint16 posArr[], int size);
void loadPlayerPos(FIXED x, FIXED y);
void runLevel(void); //plays the level until all enemies are killed
#endif
