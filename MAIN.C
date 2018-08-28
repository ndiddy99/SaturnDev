// #include "sgl.h"
// #include "game.h"
// #include "assetrefs.h"
// #include "spritepos1.h"

// #define		RBG0_CEL_ADR		VDP2_VRAM_A0
// #define		RBG0_MAP_ADR		VDP2_VRAM_B0
// #define		RBG0_COL_ADR		( VDP2_COLRAM + 0x00200 )
// #define		RBG0_PAR_ADR		( VDP2_VRAM_A1 + 0x1fe00 )

// #define		BACK_COL_ADR		( VDP2_VRAM_A1 + 0x1fffe 

// FIXED spritepos1[] = {
	// toFIXED(90.0), toFIXED(90.0),
	// toFIXED(90.0), toFIXED(90.0),
	
// };

// FIXED spritepos2[] = {
	// toFIXED(10.0), toFIXED(10.0),
	// toFIXED(10.0), toFIXED(10.0),
	
	// toFIXED(130), toFIXED(10),
	// toFIXED(130), toFIXED(10),
	
	// toFIXED(10), toFIXED(130),
	// toFIXED(10), toFIXED(130),
	
	// toFIXED(130), toFIXED(130),
	// toFIXED(130), toFIXED(130),
// };
// void ss_main(void) { //entry point for program
	// ANGLE ang = 0;

	// slInitSystem(TV_320x224,tex_sprites,1);
	// slTVOff();
	// slRparaInitSet((void *)RBG0_PAR_ADR);
	// slCharRbg0(COL_TYPE_256, CHAR_SIZE_2x2);
	// slPageRbg0((void *)RBG0_CEL_ADR, 0, PNB_1WORD|CN_10BIT);
	// slPlaneRA(PL_SIZE_1x1);
	// sl1MapRA((void *)RBG0_MAP_ADR);
	// slOverRA(2);
	// Cel2VRAM(cel_map1, (void *)RBG0_CEL_ADR, 3 * 64 * 4);
	// Pal2CRAM(pal_map1, (void *)RBG0_COL_ADR, 256);
	// Map2VRAM(map_map1, (void *)RBG0_MAP_ADR, 64, 64, 1, 0);
	// slDispCenterR(toFIXED(160.0) , toFIXED(112.0));
	// slLookR(toFIXED(0.0) , toFIXED(0.0));
	// slScrAutoDisp(RBG0ON);
	// slTVOn();
	// while (1) {
		// ang += DEGtoANG(1);
		// slZrotR(ang);
		// slSynch();
		// // loadLevel(map_map1);
		// // loadSpritePos(spritepos1, sizeof(spritepos1)/sizeof(FIXED));
		// // runLevel();
		// // loadLevel(map_map2);
		// // loadSpritePos(spritepos2, sizeof(spritepos2)/sizeof(FIXED));
		// // runLevel();
	// }
// }

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
		// loadLevel(map_map2);
		// loadSpritePos(spritepos2, sizeof(spritepos2)/sizeof(FIXED));
		// runLevel();
	}
}

