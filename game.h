#ifndef GAME_H
#define GAME_H

extern Uint8 numSprites; //current number of onscreen sprites
void loadLevel(Uint16 map[]);
void loadSpritePos(FIXED posArr[], int size);
void runLevel(void); //plays the level until all enemies are killed
#endif
