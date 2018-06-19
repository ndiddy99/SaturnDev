#include	"sgl.h"
#include	"sega_sys.h"

#include "structs.h"
#include "assetrefs.h"
#include "miscfuncs.c"

#define		RBG0_CEL_ADR		VDP2_VRAM_A0
#define		RBG0_MAP_ADR		( VDP2_VRAM_A0 + 0x10000 )
#define		RBG0_COL_ADR		( VDP2_COLRAM + 0x00200 )
#define		RBG0_PAR_ADR		( VDP2_VRAM_A1 + 0x1fe00 )

#define		NBG1_CEL_ADR		( VDP2_VRAM_B0)
#define		NBG1_MAP_ADR		( VDP2_VRAM_B0 + 0x10000 )
#define		NBG1_COL_ADR		( VDP2_COLRAM + 0x00400 )

#define		BACK_COL_ADR		( VDP2_VRAM_A1 + 0x1fffe )

#define NUM_SPRITES 200
SPRITE_INFO sprites[NUM_SPRITES];
const SPR_ATTR DEFAULT_ATTR = SPR_ATTRIBUTE(0,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
int numDispSprites = 1;

#define STATE_FALLING 0
#define STATE_RISING 1
int state = 0;

FIXED scale = toFIXED(1.0);
FIXED scaleSpeed = toFIXED(0.0);

FIXED screenX = toFIXED(0.0);
FIXED screenY = toFIXED(0.0);

//function prototypes
static void set_sprite(PICTURE *pcptr , Uint32 NbPicture, TEXTURE *txptr);
void handleInput(void);
void initSprites(void);
void initVDP2(void);
void updateBG(void);
void handleGroundCollision(FIXED x, FIXED y);
void writeBlock(Uint16 x, Uint16 y, Uint16 data);
void dispSprites(void);
void ss_main(void);

static void set_sprite(PICTURE *pcptr , Uint32 NbPicture, TEXTURE *txptr)
{
	for(; NbPicture-- > 0; pcptr++){
		txptr += pcptr->texno;
		slDMACopy((void *)pcptr->pcsrc,
			(void *)(SpriteVRAM + ((txptr->CGadr) << 3)),
			(Uint32)((txptr->Hsize * txptr->Vsize * 4) >> (pcptr->cmode)));
	}
}

void handleInput(void)
{
	Uint16 data = ~Smpc_Peripheral[0].data;
	if (data & PER_DGT_KR)
		screenX += toFIXED(2.0);
	else if (data & PER_DGT_KL)
		screenX -= toFIXED(2.0);
	if (data & PER_DGT_KU)
		screenY -= toFIXED(2.0);
	else if (data & PER_DGT_KD)
		screenY += toFIXED(2.0);
}

void initSprites(void)
{
	int i;
	for (i = 0; i < NUM_SPRITES; i++) {
		sprites[i].pos[X] = toFIXED(192.0); //start offscreen (160 + sprite width)
		sprites[i].pos[Y] = toFIXED(0.0);
		sprites[i].pos[Z] = toFIXED(169);
		sprites[i].pos[S] = toFIXED(1.0); 
		sprites[i].ang = DEGtoANG(0.0);
		sprites[i].attr = DEFAULT_ATTR;
		sprites[i].type = TYPE_NULL;
	}
}

void initVDP2(void)
{
	slColRAMMode(CRM16_2048);
	slBack1ColSet((void *)BACK_COL_ADR , 0);
	
	//init rotating bg 
	slRparaInitSet((void *)RBG0_PAR_ADR);
	slCharRbg0(COL_TYPE_256, CHAR_SIZE_2x2);
	slPageRbg0((void *)RBG0_CEL_ADR, 0, PNB_1WORD|CN_10BIT);
	slPlaneRA(PL_SIZE_1x1);
	sl1MapRA((void *)RBG0_MAP_ADR);
	slOverRA(2);
	Cel2VRAM(cel_map1, (void *)RBG0_CEL_ADR, 3 * 64 * 4);
	Map2VRAM(map_map1, (void *)RBG0_MAP_ADR, 64, 64, 1, 0);
	Pal2CRAM(pal_map1, (void *)RBG0_COL_ADR, 256);
	
	slDispCenterR(toFIXED(160.0) , toFIXED(112.0));
	slLookR(toFIXED(0.0) , toFIXED(0.0));
	
	//init face
	slCharNbg1(COL_TYPE_256, CHAR_SIZE_2x2);
	slPageNbg1((void *)NBG1_CEL_ADR, 0 , PNB_1WORD|CN_10BIT);
	slPlaneNbg1(PL_SIZE_1x1);
	slMapNbg1((void *)NBG1_MAP_ADR , (void *)NBG1_MAP_ADR , (void *)NBG1_MAP_ADR , (void *)NBG1_MAP_ADR);
	Cel2VRAM(cel_face, (void *)NBG1_CEL_ADR, 16 * 64 * 4);
	Map2VRAM(map_face, (void *)NBG1_MAP_ADR, 64, 64, 2, 0);
	Pal2CRAM(pal_face, (void *)NBG1_COL_ADR, 256);
	slScrPosNbg1(toFIXED(-160.0) + toFIXED(32.0), toFIXED(-116.0) + toFIXED(32.0));
	slPriorityNbg1(7);
	slColorCalc(CC_RATE | CC_TOP | NBG1ON);
	slColRateNbg1(0x08); 
	slColorCalcOn(NBG1ON);
	
	slScrAutoDisp(NBG0ON | NBG1ON | RBG0ON);	
}

void updateBG(void)
{
	if (state == STATE_FALLING) {
		scaleSpeed -= toFIXED(0.001);
		scale += scaleSpeed;
		if (scale <= toFIXED(0.2)) {
			state = STATE_RISING;
			scaleSpeed = toFIXED(0.05);
		//	slPrintFX((screenX >> 4), slLocate(0,1));
			//slPrintFX((screenY >> 4), slLocate(0,2));
			handleGroundCollision((screenX >> 4), (screenY >> 4)); //divide by 16- 16 px per tile
		}
	}
	else if (state == STATE_RISING) {
		scaleSpeed -= toFIXED(0.001);
		scale += scaleSpeed;
		if (scaleSpeed <= 0) {
			state = STATE_FALLING;
			scaleSpeed = 0;
		}
	}
	slLookR(screenX, screenY);
	slZoomR(scale, scale);
}

void handleGroundCollision(FIXED x, FIXED y) {
	#define BLOCK_THRESHOLD_LOW toFIXED(0.2)
	#define BLOCK_THRESHOLD_HIGH toFIXED(0.8)
	FIXED xDecimal = x & 0x0000ffff;
	FIXED yDecimal = y & 0x0000ffff;
	//todo: check for sprite collision here, make ground collision conditional upon sprite collision not happening
	if (xDecimal > BLOCK_THRESHOLD_HIGH || xDecimal < BLOCK_THRESHOLD_LOW) { //either less than .2 or greater than .8: trigger block to left and block
		writeBlock(fixedToUint16(x) - 1, fixedToUint16(y), 0x0000);
		writeBlock(fixedToUint16(x), fixedToUint16(y), 0x0000);
		
		if (yDecimal > BLOCK_THRESHOLD_HIGH || yDecimal < BLOCK_THRESHOLD_LOW) { //if y is in same threshold, trigger blocks to top as well
			writeBlock(fixedToUint16(x) - 1, fixedToUint16(y) - 1, 0x0000);
			writeBlock(fixedToUint16(x), fixedToUint16(y) - 1, 0x0000);
		}
	}
	else { 
		writeBlock(fixedToUint16(x), fixedToUint16(y), 0x0000); //otherwise, just trigger block
		if (yDecimal > BLOCK_THRESHOLD_HIGH || yDecimal < BLOCK_THRESHOLD_LOW)
			writeBlock(fixedToUint16(x) - 1, fixedToUint16(y), 0x0000);
	}
}

//when I have more block types, this should scan them to make sure they're breakable before breaking them
void writeBlock(Uint16 x, Uint16 y, Uint16 data) {
	if (MapRead(RBG0_MAP_ADR, x, y) == 0x0004)
		MapWrite(RBG0_MAP_ADR, x, y, 1, data);
}

void updateSprites(void)
{
	int i;
	FIXED dx, dy;
	for (i = 0; i < numDispSprites; i++) {
		switch(sprites[i].type) {
		case TYPE_NULL:
			break;
		case TYPE_CIRCLE:
			#define CIRCLE_SPEED toFIXED(1.0)
			if (slRandom() > toFIXED(0.5))
				sprites[i].ang += DEGtoANG(10);
			else
				sprites[i].ang -= DEGtoANG(10);
			dx = slMulFX(CIRCLE_SPEED, slSin(sprites[i].ang));
			dy = slMulFX(CIRCLE_SPEED, slCos(sprites[i].ang));
			sprites[i].pos[X] += dx;
			sprites[i].pos[Y] += dy;
			slPrint("Target X:", slLocate(0,0));
			slPrintFX(sprites[i].pos[X], slLocate(10,0));
			slPrint("Target Y:", slLocate(0,1));
			slPrintFX(sprites[i].pos[Y], slLocate(10,1));
			slPrint("Angle:", slLocate(0,2));
			slPrintFX(sprites[i].ang, slLocate(7,2));
			if (MapRead(RBG0_MAP_ADR, fixedToUint16(sprites[i].pos[X] >> 4), fixedToUint16(sprites[i].pos[Y] >> 4)) == 0x0000) {
				slPrint("zongo", slLocate(0,3));
				sprites[i].pos[X] -= dx;
				sprites[i].pos[Y] -= dy;
				sprites[i].ang += DEGtoANG(90);
			}
			else
				slPrint("dongo", slLocate(0,3));
			break;
		}
	}
}

void dispSprites(void)
{
	int i;
	FIXED spriteScale = slDivFX(scale, toFIXED(1.0)); //reciprocal
	FIXED spritePos[XYZS];
	for (i = 0; i < numDispSprites; i++) {
		spritePos[X] = slMulFX(sprites[i].pos[X] - screenX, spriteScale);
		spritePos[Y] = slMulFX(sprites[i].pos[Y] - screenY, spriteScale);
		slPrintFX(spritePos[X], slLocate(0,4));
		slPrintFX(spritePos[Y], slLocate(0,5));
		slPrintFX(spriteScale, slLocate(0,6));
		spritePos[S] = spriteScale;
		slDispSprite(spritePos, &sprites[i].attr, sprites[i].ang);
	}
}
 
void ss_main(void)
{
	slInitSystem(TV_320x224,tex_sprites,1);
	slTVOff();
	set_sprite(pic_sprites, 3, tex_sprites);
	initSprites();
	initVDP2();
	slTVOn();
	sprites[0].pos[X] = toFIXED(50.0);
	sprites[0].pos[Y] = toFIXED(50.0);
	sprites[0].type = TYPE_CIRCLE;
	while(1) {
		handleInput();
		updateBG();
		updateSprites();
		dispSprites();
		slSynch();
	} 
}

