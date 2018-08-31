#ifndef GAME_H
#define GAME_H
#include "spritelist.h"

extern Uint8 numSprites; //current number of onscreen sprites
#define MAX_SPRITES 200
extern SPRITE_INFO sprites[MAX_SPRITES];
void loadLevel(Uint16 map[]);
void loadSpritePos(FIXED posArr[], int size);
void runLevel(void); //plays the level until all enemies are killed
#endif
