#include "sgl.h"
#include "game.h"
#include "assetrefs.h"
#include "spritelist.h"
#include "spritepos.h"

typedef struct levelInfo {
	Uint16* map;
	Uint16* spritepos;
	int spriteposSize;
	FIXED playerX;
	FIXED playerY;
	Uint8 background;
} LEVEL;

#define NUM_LEVELS 2
int level = 0;
LEVEL levels[] = {
	{map_map1, spritepos1, sizeof(spritepos1)/sizeof(Uint16), toFIXED(82), toFIXED(82), MODE_LINESCROLL},
	{map_map2, spritepos2, sizeof(spritepos2)/sizeof(Uint16), toFIXED(34), toFIXED(30), MODE_TILEMAP},
};

int playLevel(LEVEL l);

void ss_main(void) { //entry point for program
	slInitSystem(TV_320x224,tex_sprites, 1);
	while (1) {
		// loadLevel(map_map1);
		// loadSpritePos(spritepos1, sizeof(spritepos1)/sizeof(Uint16));
		// loadPlayerPos(toFIXED(82),toFIXED(82));
		// bgMode = MODE_LINESCROLL;
		// runLevel();
		// loadLevel(map_map2);
		// loadSpritePos(spritepos2, sizeof(spritepos2)/sizeof(Uint16));
		// loadPlayerPos(toFIXED(34),toFIXED(30));
		// bgMode = MODE_TILEMAP;
		// runLevel();
		if (playLevel(levels[level]))
			level++;
		else
			level = 0;
		if (level >= NUM_LEVELS)
			level = 0;
	}
}

int playLevel(LEVEL l) {
	loadLevel(l.map);
	loadSpritePos(l.spritepos, l.spriteposSize);
	loadPlayerPos(l.playerX, l.playerY);
	bgMode = l.background;
	return runLevel();
}
