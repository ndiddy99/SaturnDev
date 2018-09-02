#include "sgl.h"
#include "game.h"
#include "assetrefs.h"
#include "spritepos1.h"

FIXED spritepos1[] = {
	toFIXED(90.0), toFIXED(90.0),
	toFIXED(90.0), toFIXED(90.0),
	
};

FIXED spritepos2[] = {
	toFIXED(10.0), toFIXED(10.0),
	toFIXED(10.0), toFIXED(10.0),
	
	toFIXED(130), toFIXED(10),
	toFIXED(130), toFIXED(10),
	
	toFIXED(10), toFIXED(130),
	toFIXED(10), toFIXED(130),
	
	toFIXED(130), toFIXED(130),
	toFIXED(130), toFIXED(130),
};
void ss_main(void) { //entry point for program
	slInitSystem(TV_320x224,tex_sprites,1);
	while (1) {
		loadLevel(map_map1);
		loadSpritePos(spritepos1, sizeof(spritepos1)/sizeof(FIXED));
		runLevel();
		loadLevel(map_map2);
		loadSpritePos(spritepos2, sizeof(spritepos2)/sizeof(FIXED));
		runLevel();
	}
}

