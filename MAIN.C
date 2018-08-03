#include	"sgl.h"
#include "game.h"
#include "assetrefs.h"


void ss_main(void) { //entry point for program
	slInitSystem(TV_320x224,tex_sprites,1);
	while (1) {
		loadLevel(map_map1);
		runLevel();
		loadLevel(map_map2);
		runLevel();
	}
}
