#ifndef GAME
#define GAME
extern Uint8 numSprites; //current number of onscreen sprites
void loadLevel(Uint16 map[]);
void runLevel(void); //plays the level until all enemies are killed
#endif
