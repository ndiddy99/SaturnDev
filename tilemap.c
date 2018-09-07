#include "sgl.h"
#include "sega_sys.h"

#include "tilemap.h"
#include "assetrefs.h"

#define		NBG1_CEL_ADR		VDP2_VRAM_A0
#define		NBG1_MAP_ADR		( VDP2_VRAM_A1 + 0x2000 )
#define		NBG1_COL_ADR		( VDP2_COLRAM + 0x00200 )

#define		NBG2_CEL_ADR		( VDP2_VRAM_B0 )
#define		NBG2_MAP_ADR		( VDP2_VRAM_A1 + 0x2000)
#define		NBG2_COL_ADR		( VDP2_COLRAM + 0x00400 )

static FIXED bg1X = toFIXED(0.0);
static FIXED bg1Y = toFIXED(0.0);
static FIXED bg2X = toFIXED(0.0);
static FIXED bg2Y = toFIXED(0.0);

void initTilemap(void) {
	//init road
	slCharNbg1(COL_TYPE_256, CHAR_SIZE_2x2);
	slPageNbg1((void *)NBG1_CEL_ADR, 0 , PNB_1WORD|CN_10BIT);
	slPlaneNbg1(PL_SIZE_1x1);
	slMapNbg1((void *)NBG1_MAP_ADR , (void *)NBG1_MAP_ADR , (void *)NBG1_MAP_ADR , (void *)NBG1_MAP_ADR);
	Cel2VRAM(cel_road, (void *)NBG1_CEL_ADR, 83 * 64 * 4);
	Map2VRAM(map_road, (void *)NBG1_MAP_ADR, 64, 64, 1, 0); 
	Pal2CRAM(pal_road, (void *)NBG1_COL_ADR, 256);
	slScrPosNbg1(toFIXED(0), toFIXED(0));
	
	//init clouds
	slCharNbg2(COL_TYPE_256, CHAR_SIZE_2x2);
	slPageNbg2((void *)NBG2_CEL_ADR, 0 , PNB_1WORD|CN_10BIT);
	slPlaneNbg2(PL_SIZE_1x1);
	slMapNbg2((void *)NBG2_MAP_ADR , (void *)NBG2_MAP_ADR , (void *)NBG2_MAP_ADR , (void *)NBG2_MAP_ADR);
	Cel2VRAM(cel_cloud, (void *)NBG2_CEL_ADR, 11 * 64 * 4);
	Map2VRAM(map_cloud, (void *)NBG2_MAP_ADR, 64, 64, 2, 0); 
	Pal2CRAM(pal_cloud, (void *)NBG2_COL_ADR, 256);
	slScrPosNbg2(toFIXED(0), toFIXED(0));
	slPriorityNbg2(4);
	slColRateNbg2(0x05);
	
}

void updateTilemap(void) {
	//road movement
	bg1X += toFIXED(0.5);
	slScrPosNbg1(bg1X, bg1Y);
	//cloud movement
	bg2X += toFIXED(1.0);
	bg2Y += toFIXED(1.0);
	slScrPosNbg2(bg2X, bg2Y);
}
