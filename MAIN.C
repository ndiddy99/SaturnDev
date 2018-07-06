/*
To-Dos:
Make enemies killable by crushing and falling (done)
Draw block textures that more closely match Bound High's (done for now)
Make a level progression system (I'm thinking making a level function that is called for each level from the main function)
Add score/lives
Animate fake Chalvo
Add level begin/end animations

*/

#include	"sgl.h"
#include	"sega_sys.h"

#include "structs.h"
#include "assetrefs.h"
#include "miscfuncs.c"

#define		RBG0_CEL_ADR		VDP2_VRAM_A0
#define		RBG0_MAP_ADR		( VDP2_VRAM_A0 + 0x10000 )
#define		RBG0_COL_ADR		( VDP2_COLRAM + 0x00200 )
#define		RBG0_PAR_ADR		( VDP2_VRAM_A0 + 0x1fe00 )

#define		NBG1_CEL_ADR		( VDP2_VRAM_B0)
#define		NBG1_MAP_ADR		( VDP2_VRAM_A1)
#define		NBG1_COL_ADR		( VDP2_COLRAM + 0x00400 )

#define		NBG2_CEL_ADR		( VDP2_VRAM_B0 + 0x1000)
#define		NBG2_MAP_ADR		( VDP2_VRAM_A1 + 0x800 )
#define		NBG2_COL_ADR		( VDP2_COLRAM + 0x00600 )


#define		BACK_COL_ADR		( VDP2_VRAM_A1 + 0x1fffe )

#define STATE_FALLING 0
#define STATE_RISING 1
int state = 0;

FIXED scale = toFIXED(1.0);
FIXED scaleSpeed = toFIXED(0.0);

FIXED screenX = toFIXED(0.0);
FIXED screenY = toFIXED(0.0);
#define SCREEN_BOUND_L toFIXED(-160)
#define SCREEN_BOUND_R toFIXED(160)
#define SCREEN_BOUND_T toFIXED(-112)
#define SCREEN_BOUND_B toFIXED(112)


//function prototypes
static void set_sprite(PICTURE *pcptr , Uint32 NbPicture, TEXTURE *txptr);
void handleInput(void);
void initSprites(void);	
void initVDP2(void);
void updateBG(void);
Uint8 handleSpriteCollision(FIXED x, FIXED y);
void handleGroundCollision(FIXED x, FIXED y);
void writeBlock(Uint16 x, Uint16 y, Uint16 data);
Uint8 checkShotCollision(SpriteNode node);
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
	defaultSprite.pos[X] = toFIXED(0.0); //start offscreen (160 + sprite width)
	defaultSprite.pos[Y] = toFIXED(0.0);
	defaultSprite.pos[Z] = toFIXED(169);
	defaultSprite.pos[S] = toFIXED(1.0); 
	defaultSprite.ang = DEGtoANG(0.0);
	defaultSprite.attr = CIRCLE_ATTR;
	defaultSprite.type = TYPE_CIRCLE;
	defaultSprite.dx = toFIXED(0.0);
	defaultSprite.dy = toFIXED(0.0);
	defaultSprite.state = STATE_NORM;
	headNode = addSpriteNode(NULL, defaultSprite);
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
	
	//init road
	slCharNbg2(COL_TYPE_256, CHAR_SIZE_2x2);
	slPageNbg2((void *)NBG2_CEL_ADR, 0 , PNB_1WORD|CN_10BIT);
	slPlaneNbg2(PL_SIZE_1x1);
	slMapNbg2((void *)NBG2_MAP_ADR , (void *)NBG2_MAP_ADR , (void *)NBG2_MAP_ADR , (void *)NBG2_MAP_ADR);
	Cel2VRAM(cel_road, (void *)NBG2_CEL_ADR, 83 * 64 * 4);
	Map2VRAM(map_road, (void *)NBG2_MAP_ADR, 64, 64, 3, 32);
	Pal2CRAM(pal_road, (void *)NBG2_COL_ADR, 256);
	slScrPosNbg2(toFIXED(0), toFIXED(0));
	
	slScrAutoDisp(NBG0ON | NBG1ON | NBG2ON | RBG0ON);	
}

void updateBG(void)
{
	slPrint("updateBG", slLocate(0,0));
	if (state == STATE_FALLING) {
		scaleSpeed -= toFIXED(0.001);
		scale += scaleSpeed;
		if (scale <= toFIXED(0.2)) {
			state = STATE_RISING;
			scaleSpeed = toFIXED(0.05);
		//	slPrintFX((screenX >> 4), slLocate(0,1));
			//slPrintFX((screenY >> 4), slLocate(0,2));
			if (!handleSpriteCollision(screenX, screenY))
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

Uint8 handleSpriteCollision(FIXED x, FIXED y)
{
	slPrint("handleSpriteCollision", slLocate(0,0));
	//if ball x > sprite x1 and < sprite x2 
	SpriteNode ptr = headNode;
	while (ptr != NULL) {
		switch (ptr->sprite.type) {
			case TYPE_CIRCLE:
				if (ptr->sprite.pos[X] - SPR_SIZE[TYPE_CIRCLE] < x && ptr->sprite.pos[X] + SPR_SIZE[TYPE_CIRCLE] > x) {
					if (ptr->sprite.pos[Y] - SPR_SIZE[TYPE_CIRCLE] < y && ptr->sprite.pos[Y] + SPR_SIZE[TYPE_CIRCLE] > y) {
						SPRITE_INFO tmp = defaultSprite;
						tmp.attr = SHOT_ATTR;
						tmp.type = TYPE_SHOT;
						tmp.pos[X] = x;
						tmp.pos[Y] = y;
						if (abs(ptr->sprite.pos[X] - x) < toFIXED(2))
							tmp.dx = toFIXED(0);
						else if (ptr->sprite.pos[X] < x)
							tmp.dx = toFIXED(-2);
						else
							tmp.dx = toFIXED(2);
						
						if (abs(ptr->sprite.pos[Y] - y) < toFIXED(2) && tmp.dx != toFIXED(0))
							tmp.dy = toFIXED(0);
						else if (ptr->sprite.pos[Y] < y)
							tmp.dy = toFIXED(-2);
						else
							tmp.dy = toFIXED(2);
						addSpriteNode(headNode, tmp);
						deleteSpriteNode(&headNode, ptr);
						return 1;
					}
				}
			break;
		}
		ptr = ptr->next;
	}
	return 0;
}

void handleGroundCollision(FIXED x, FIXED y) {
	slPrint("handleGroundCollision", slLocate(0,0));
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
	slPrint("updateSprites", slLocate(0,0));
	int i;
	SpriteNode ptr = headNode;
	while (ptr != NULL) {
		switch(ptr->sprite.type) {
		case TYPE_NULL:
			break;
		case TYPE_CIRCLE:
			if (ptr->sprite.state != STATE_FALL) {
				if (MapRead(RBG0_MAP_ADR, fixedToUint16(ptr->sprite.pos[X] >> 4), fixedToUint16(ptr->sprite.pos[Y] >> 4)) == 0x0000) {
					ptr->sprite.state = STATE_FALL;
					break;
				}
				#define CIRCLE_SPEED toFIXED(1.0)
				if (slRandom() > toFIXED(0.5))
					ptr->sprite.ang += DEGtoANG(10);
				else
					ptr->sprite.ang -= DEGtoANG(10);
				ptr->sprite.dx = slMulFX(CIRCLE_SPEED, slSin(ptr->sprite.ang));
				ptr->sprite.dy = slMulFX(CIRCLE_SPEED, slCos(ptr->sprite.ang));
				ptr->sprite.pos[X] += ptr->sprite.dx;
				ptr->sprite.pos[Y] += ptr->sprite.dy;
				// slPrint("Target X:", slLocate(0,0));
				// slPrintFX(ptr->sprite.pos[X], slLocate(10,0));
				// slPrint("Target Y:", slLocate(0,1));
				// slPrintFX(ptr->sprite.pos[Y], slLocate(10,1));
				// slPrint("Angle:", slLocate(0,2));
	//			slPrintHex(ptr->sprite.ang, slLocate(7,2));
				if (MapRead(RBG0_MAP_ADR, fixedToUint16(ptr->sprite.pos[X] >> 4), fixedToUint16(ptr->sprite.pos[Y] >> 4)) == 0x0000) {
					ptr->sprite.pos[X] -= ptr->sprite.dx;
					ptr->sprite.pos[Y] -= ptr->sprite.dy;
					ptr->sprite.ang += DEGtoANG(90);
				}
			}
			else {
				if (ptr->sprite.pos[S] > toFIXED(0.05)) //scale down sprite until it disappears
					ptr->sprite.pos[S] -= toFIXED(0.02);
				else
					deleteSpriteNode(&headNode, ptr);
			}
			break;
		case TYPE_SHOT:
			ptr->sprite.pos[X] += ptr->sprite.dx;
			ptr->sprite.pos[Y] += ptr->sprite.dy;
			if (checkShotCollision(ptr))
				deleteSpriteNode(&headNode, ptr);
			else if (abs(ptr->sprite.pos[X] - screenX) > SCREEN_BOUND_R || //remove shot sprite if it goes offscreen
				abs(ptr->sprite.pos[Y] - screenY) > SCREEN_BOUND_B) {
					deleteSpriteNode(&headNode, ptr);
				}
			break;
		}
		ptr = ptr->next;
	}
}

Uint8 checkShotCollision(SpriteNode node) 
{
	slPrint("checkShotCollision", slLocate(0,0));
	SpriteNode ptr = headNode;
	FIXED x = node->sprite.pos[X];
	FIXED y = node->sprite.pos[Y];
	while (ptr != NULL) {
		if (ptr != node){
			switch (ptr->sprite.type) {
				case TYPE_CIRCLE:
					if (ptr->sprite.pos[X] - SPR_SIZE[TYPE_CIRCLE] < x && ptr->sprite.pos[X] + SPR_SIZE[TYPE_CIRCLE] > x) {
						if (ptr->sprite.pos[Y] - SPR_SIZE[TYPE_CIRCLE] < y && ptr->sprite.pos[Y] + SPR_SIZE[TYPE_CIRCLE] > y) {
							deleteSpriteNode(&headNode, ptr);
							return 1;
						}
					}
				break;
			}
		}
		ptr = ptr->next;
	}
	return 0;
	
}

void dispSprites(void)
{
	slPrint("dispSprites", slLocate(0,0));
	int i;
	FIXED spriteScale = slDivFX(scale, toFIXED(1.0)); //reciprocal
	FIXED spritePos[XYZS];
	SpriteNode ptr = headNode;
	while (ptr != NULL) {
		spritePos[X] = slMulFX(ptr->sprite.pos[X] - screenX, spriteScale);
		spritePos[Y] = slMulFX(ptr->sprite.pos[Y] - screenY, spriteScale);
		//slPrintFX(spritePos[X], slLocate(0,4));
		//slPrintFX(spritePos[Y], slLocate(0,5));
		if (ptr->sprite.state != STATE_FALL)
			spritePos[S] = spriteScale;
		else
			spritePos[S] = slMulFX(ptr->sprite.pos[S], spriteScale);
		//slPrintFX(spriteScale, slLocate(0,6));
		slDispSprite(spritePos, &ptr->sprite.attr, DEGtoANG(0)); //todo: figure out why weird angle behavior happens
		ptr = ptr->next;
	}
}
 
void ss_main(void)
{
	int i;
	
	slInitSystem(TV_320x224,tex_sprites,1);
	slTVOff();
	set_sprite(pic_sprites, 3, tex_sprites);
	initSprites();
	initVDP2();
	slTVOn();
	for (i = 0; i < 5; i++) {
		addSpriteNode(headNode, defaultSprite);
	}
	while(1) {
		handleInput();
		updateBG();
		updateSprites();
		dispSprites();
		slSynch();
	} 
}

