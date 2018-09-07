#include "sgl.h"
#include "sega_sys.h"

#include "assetrefs.h"
#include "framebuffer.h"

#define videoMem ((Uint16*)VDP2_VRAM_A0)
#define TEXTURE_WIDTH 128
#define TEXTURE_HEIGHT 128
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 56

static void initVDP2(void);
static void render(int time);

static Uint8 lut[(SCREEN_WIDTH*SCREEN_HEIGHT)*2];
static Uint16 screenBuf[SCREEN_WIDTH*SCREEN_HEIGHT];

static void initVDP2(void) {
	// slTVOff();
	slBitMapNbg1(COL_TYPE_32768, BM_512x256, (void *)VDP2_VRAM_A0);
	slScrPosNbg1(toFIXED(0.0), toFIXED(0.0));
	slZoomModeNbg1(ZOOM_1);
	slZoomNbg1(toFIXED(0.25), toFIXED(0.25));  
	slScrAutoDisp(NBG0ON | NBG1ON);
}

//code from http://www.iquilezles.org/www/articles/deform/deform.htm
void initFramebuffer(void) {
	int i,j;
	int k = 0;
	initVDP2();
	for (j=0; j < SCREEN_HEIGHT; j++) {
		for (i=0; i < SCREEN_WIDTH; i++) {
			FIXED x = toFIXED(-1) + slDivFX(toFIXED(SCREEN_WIDTH), slMulFX(toFIXED(2), (i << 16)));
			FIXED y = toFIXED(-1) + slDivFX(toFIXED(SCREEN_HEIGHT), slMulFX(toFIXED(2), (j << 16)));
			FIXED distance = slSquartFX(slMulFX(x,x) + slMulFX(y,y));
			ANGLE angle = slAtan(x,y);
			
			FIXED u = slDivFX(distance, slCos(angle));
			FIXED v = slDivFX(distance, slSin(angle));
			lut[k++] = ((slMulFX(toFIXED(128), u)) >> 16) & 127;
			lut[k++] = ((slMulFX(toFIXED(128), v)) >> 16) & 127;
		}
	}
	
}

static void render(int time) {
	int i,j;
	for (j = 0; j < SCREEN_HEIGHT; j++) {
		for (i = 0; i < SCREEN_WIDTH; i++) {
			int offset = (SCREEN_WIDTH)*j + i;
			int u = lut[(offset << 1)+0] + time;
			int v = lut[(offset << 1)+1] + time;
			videoMem[512*j+i]=checkerboard[128*(v&127)+(u&127)];
			// screenBuf[SCREEN_WIDTH*j + i] = checkerboard[128*(v&127)+(u&127)];
		}
	}
	// for (i = 0; i < TEXTURE_HEIGHT; i++) {
		// slDMACopy(((void *)checkerboard) + (i * TEXTURE_WIDTH * 2), ((void *)VDP2_VRAM_A0) + (512 * i * 2), TEXTURE_WIDTH*2);
		// slDMAWait();
	// }
	// slDMACopy((void *)checkerboard, (void *)VDP2_VRAM_A0, 128*2);
}

void runFramebuffer(void) {
	static int i = 0;
	render(i++);
}
