#include "sgl.h"
#include "game.h"
#include "assetrefs.h"
#include "spritelist.h"
#include "spritepos1.h"

Uint16 spritepos1[] = {
	90, 90, TYPE_CIRCLE,
	90, 90, TYPE_PUSH
	
};

Uint16 spritepos2[] = {
	10, 10, TYPE_CIRCLE,
	10, 10, TYPE_CIRCLE,
	
	130, 10, TYPE_PUSH,
	130, 10, TYPE_PUSH,
	
	10, 130, TYPE_CIRCLE,
	10, 130, TYPE_CIRCLE,
	
	130, 130, TYPE_PUSH,
	130, 130, TYPE_PUSH
};
void ss_main(void) { //entry point for program
	slInitSystem(TV_320x224,tex_sprites, 1);
	while (1) {
		loadLevel(map_map1);
		loadSpritePos(spritepos1, sizeof(spritepos1)/sizeof(Uint16));
		runLevel();
		loadLevel(map_map2);
		loadSpritePos(spritepos2, sizeof(spritepos2)/sizeof(Uint16));
		runLevel();
	}
}

