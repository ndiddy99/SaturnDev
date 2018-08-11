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

#include "game.h"
#include "sprattrs.c"
#include "assetrefs.h"
#include "spritelist.h"

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

#define		NBG3_CEL_ADR		( VDP2_VRAM_B1 + 0x2000)
#define		NBG3_MAP_ADR		( VDP2_VRAM_A1 + 0x1000 )
#define		NBG3_COL_ADR		( VDP2_COLRAM + 0x00800 )

#define		BACK_COL_ADR		( VDP2_VRAM_A1 + 0x1fffe )

#define PLAYER_STATE_FALLING 0
#define PLAYER_STATE_RISING 1
#define PLAYER_STATE_DEAD 2
int playerState = PLAYER_STATE_FALLING;
#define GAME_STATE_START 0
#define GAME_STATE_FADEIN 1
#define GAME_STATE_NORMAL 2 
#define GAME_STATE_FADEOUT 3
#define GAME_STATE_COMPLETE 4
int gameState; //for main game state machine
FIXED scale = toFIXED(1.0);
FIXED scaleSpeed = toFIXED(0.0);

SPRITE_INFO defaultSprite;
SpriteNode headNode = NULL;
SpriteNode eye1;
SpriteNode eye2;
#define NUM_PLAYER_SPRITES 4 //the number of sprites used for player face animation
Uint8 numSprites; //the number of sprites the engine's keeping track of
Uint8 dispFace; //1 if we're displaying the "player face" sprites, 0 otherwise

FIXED screenX = toFIXED(0.0);
FIXED screenY = toFIXED(0.0);
FIXED bg2X = toFIXED(0.0);
FIXED bg2Y = toFIXED(0.0);
FIXED bg3X = toFIXED(0.0);
FIXED bg3Y = toFIXED(0.0);
#define SCREEN_BOUND_L toFIXED(-160)
#define SCREEN_BOUND_R toFIXED(160)
#define SCREEN_BOUND_T toFIXED(-112)
#define SCREEN_BOUND_B toFIXED(112)


//function prototypes
static void set_sprite(PICTURE *pcptr , Uint32 NbPicture, TEXTURE *txptr);
static void handleInput(void);
static void initSprites(void);	
static void initVDP2(void);
static void updateBG(void);
static Uint8 handleSpriteCollision(FIXED x, FIXED y);
static Uint8 handleGroundCollision(FIXED x, FIXED y);
static void writeBlock(Uint16 x, Uint16 y, Uint16 data);
static void setShotVelocity(FIXED playerX, FIXED playerY, FIXED spriteX, FIXED spriteY, FIXED* dx, FIXED* dy);
static void updateSprites(void);
static Uint8 checkShotCollision(SpriteNode node);
static void dispSprites(void);

static void set_sprite(PICTURE *pcptr, Uint32 NbPicture, TEXTURE *texptr)
{
	TEXTURE *txptr;
 
	for(; NbPicture-- > 0; pcptr++){
		txptr = texptr + pcptr->texno;
		slDMACopy((void *)pcptr->pcsrc,
			(void *)(SpriteVRAM + ((txptr->CGadr) << 3)),
			(Uint32)((txptr->Hsize * txptr->Vsize * 4) >> (pcptr->cmode)));
	}
}
#define EYE_MAX_XPOS toFIXED(-7)
#define EYE_MIN_XPOS toFIXED(-13)
#define EYE_MAX_YPOS toFIXED(-10)
#define EYE_MIN_YPOS toFIXED(0)
#define EYE_MOVE_SPEED toFIXED(0.5)
#define EYE_DISTANCE toFIXED(23)
static void handleInput(void)
{
	Uint16 data = ~Smpc_Peripheral[0].data;
	if (playerState != PLAYER_STATE_DEAD) {
		if (data & PER_DGT_KR) {
			screenX += toFIXED(2.0);
			if (eye1->sprite.pos[X] < EYE_MAX_XPOS)
				eye1->sprite.pos[X] += EYE_MOVE_SPEED;
		}
		else if (data & PER_DGT_KL) {
			screenX -= toFIXED(2.0);
			if (eye1->sprite.pos[X] > EYE_MIN_XPOS)
				eye1->sprite.pos[X] -= EYE_MOVE_SPEED;
		}
		if (data & PER_DGT_KU) {
			screenY -= toFIXED(2.0);
			if (eye1->sprite.pos[Y] > EYE_MAX_YPOS)
				eye1->sprite.pos[Y] -= EYE_MOVE_SPEED;
		}
		else if (data & PER_DGT_KD) {
			screenY += toFIXED(2.0);
			if (eye1->sprite.pos[Y] < EYE_MIN_YPOS)
				eye1->sprite.pos[Y] += EYE_MOVE_SPEED;
		}
		eye2->sprite.pos[X] = eye1->sprite.pos[X] + EYE_DISTANCE;
		eye2->sprite.pos[Y] = eye1->sprite.pos[Y];
	}
	
	
}

static void initSprites(void)
{
	defaultSprite.pos[X] = toFIXED(0.0); //start offscreen (160 + sprite width)
	defaultSprite.pos[Y] = toFIXED(0.0);
	defaultSprite.pos[Z] = toFIXED(169);
	defaultSprite.pos[S] = toFIXED(1.0); 
	defaultSprite.ang = 0;
	defaultSprite.attr = CIRCLE_ATTR;
	defaultSprite.type = TYPE_CIRCLE;
	defaultSprite.dx = toFIXED(0.0);
	defaultSprite.dy = toFIXED(0.0);
	defaultSprite.state = STATE_NORM;
	headNode = addSpriteNode(NULL, defaultSprite);
	//add eye sprites
	SPRITE_INFO info = defaultSprite;
	info.attr = SCLERA_ATTR;
	info.type = TYPE_FACE;
	info.pos[X] = toFIXED(-12);
	info.pos[Y] = toFIXED(-8);
	info.pos[Z] = toFIXED(160);
	addSpriteNode(headNode, info);
	info.pos[X] = toFIXED(11);
	addSpriteNode(headNode, info);
	info.pos[X] = 0;
	info.pos[Y] = 0;
	
	info.attr = EYE_ATTR;
	eye1 = addSpriteNode(headNode, info);
	eye2 = addSpriteNode(headNode, info);
	
	eye1->sprite.pos[X] = toFIXED(-10);
	eye1->sprite.pos[Y] = toFIXED(-7);
	eye2->sprite.pos[X] = toFIXED(10);
	eye2->sprite.pos[Y] = toFIXED(-7);
	
}


static void initVDP2(void)
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
	slScrPosNbg1(toFIXED(-160.0) + toFIXED(32.0), toFIXED(-116.0) + toFIXED(32.0)); //plus half width of sprite
	slPriorityNbg1(7);
	slColorCalc(CC_RATE | CC_TOP | NBG1ON);
	//slColRateNbg1(0x08); 
	slColRateNbg1(0x00);
	
	//init clouds
	slCharNbg2(COL_TYPE_256, CHAR_SIZE_2x2);
	slPageNbg2((void *)NBG2_CEL_ADR, 0 , PNB_1WORD|CN_10BIT);
	slPlaneNbg2(PL_SIZE_1x1);
	slMapNbg2((void *)NBG2_MAP_ADR , (void *)NBG2_MAP_ADR , (void *)NBG2_MAP_ADR , (void *)NBG2_MAP_ADR);
	Cel2VRAM(cel_cloud, (void *)NBG2_CEL_ADR, 11 * 64 * 4);
	//offset parameter (32 here) seems to be # of tiles before start of this bg's tiles in that vram bank * 2
	Map2VRAM(map_cloud, (void *)NBG2_MAP_ADR, 64, 64, 3, 32); 
	Pal2CRAM(pal_cloud, (void *)NBG2_COL_ADR, 256);
	slScrPosNbg2(toFIXED(0), toFIXED(0));
	slColRateNbg2(0x05);
	
	//init road
	slCharNbg3(COL_TYPE_256, CHAR_SIZE_2x2);
	slPageNbg3((void *)NBG3_CEL_ADR, 0 , PNB_1WORD|CN_10BIT);
	slPlaneNbg3(PL_SIZE_1x1);
	slMapNbg3((void *)NBG3_MAP_ADR , (void *)NBG3_MAP_ADR , (void *)NBG3_MAP_ADR , (void *)NBG3_MAP_ADR);
	Cel2VRAM(cel_road, (void *)NBG3_CEL_ADR, 83 * 64 * 4);
	Map2VRAM(map_road, (void *)NBG3_MAP_ADR, 64, 64, 3, 64); 
	Pal2CRAM(pal_road, (void *)NBG3_COL_ADR, 256);
	slScrPosNbg3(toFIXED(0), toFIXED(0));
	
	slColorCalcOn(NBG1ON | NBG2ON);
	slScrAutoDisp(NBG0ON | NBG2ON | NBG3ON | RBG0ON);
}

static void updateBG(void)
{
	static SpriteNode playerNode;
	static FIXED playerFallSpeed;
	
	slPrint("updateBG", slLocate(0,0));
	if (gameState == GAME_STATE_NORMAL) {
		switch (playerState) {
		case PLAYER_STATE_FALLING:
			scaleSpeed -= toFIXED(0.001);
			scale += scaleSpeed;
			if (scale <= toFIXED(0.2)) {
				playerState = PLAYER_STATE_RISING;
				scaleSpeed = toFIXED(0.05);
			//	slPrintFX((screenX >> 4), slLocate(0,1));
				//slPrintFX((screenY >> 4), slLocate(0,2));
				if (!handleSpriteCollision(screenX, screenY)) {
					if (!handleGroundCollision((screenX >> 4), (screenY >> 4))) { //divide by 16- 16 px per tile
						playerState = PLAYER_STATE_DEAD;
						slScrAutoDisp(NBG0ON | NBG2ON | NBG3ON | RBG0ON); //turn off player's bg layer
						dispFace = 0;
						SPRITE_INFO tmp = defaultSprite;
						tmp.attr = PLAYER_ATTR;
						tmp.type = TYPE_NULL;
						tmp.pos[X] = screenX;
						tmp.pos[Y] = screenY;
						tmp.pos[S] = toFIXED(1.0);
						playerNode = addSpriteNode(headNode, tmp);
						playerFallSpeed = toFIXED(0.0);
					}
				}
			}
		break;
		case PLAYER_STATE_RISING:
			scaleSpeed -= toFIXED(0.001);
			scale += scaleSpeed;
			if (scaleSpeed <= 0) {
				playerState = PLAYER_STATE_FALLING;
				scaleSpeed = 0;
			}
		break;
		case PLAYER_STATE_DEAD:
			if (playerNode->sprite.pos[S] > toFIXED(0.0)) {
				playerFallSpeed += toFIXED(0.005);
				playerNode->sprite.pos[S] -= playerFallSpeed;
				playerNode->sprite.ang += 5;
			}
			else { //reset player pos
				scale = toFIXED(1.0);
				scaleSpeed = toFIXED(0.0);
				screenX = toFIXED(0.0);
				screenY = toFIXED(0.0);
				playerState = PLAYER_STATE_FALLING;
				slScrAutoDisp(NBG0ON | NBG1ON | NBG2ON | NBG3ON | RBG0ON);
				dispFace = 1;
				deleteSpriteNode(&headNode, playerNode);
			}
		break;
		}
	}
	slLookR(screenX, screenY);
	slZoomR(scale, scale);
	//cloud movement
	bg2X += toFIXED(1.0);
	bg2Y += toFIXED(1.0);
	slScrPosNbg2(bg2X, bg2Y);
	bg3X += toFIXED(0.5);
	slScrPosNbg3(bg3X, bg3Y);
}

static Uint8 handleSpriteCollision(FIXED x, FIXED y)
{
	slPrint("handleSpriteCollision", slLocate(0,0));
	//if ball x > sprite x1 and < sprite x2 
	SpriteNode ptr = headNode;
	int count = 0;
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
						setShotVelocity(x, y, ptr->sprite.pos[X], ptr->sprite.pos[Y], &tmp.dx, &tmp.dy);
						addSpriteNode(headNode, tmp);
						deleteSpriteNode(&headNode, ptr);
						return 1;
					}
				}
			break;
		}
		count++;
		slPrintHex(count, slLocate(0,2));
		ptr = ptr->next;
	}
	return 0;
}

static Uint8 handleGroundCollision(FIXED x, FIXED y) {
	slPrint("handleGroundCollision", slLocate(0,0));
	#define BLOCK_THRESHOLD_LOW toFIXED(0.2)
	#define BLOCK_THRESHOLD_HIGH toFIXED(0.8)
	FIXED xDecimal = x & 0x0000ffff;
	FIXED yDecimal = y & 0x0000ffff;
	if (MapRead(RBG0_MAP_ADR, fixedToUint16(x), fixedToUint16(y)) == 0x0000) //if no ground
		return 0;
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
	return 1;
}

//when I have more block types, this should scan them to make sure they're breakable before breaking them
static void writeBlock(Uint16 x, Uint16 y, Uint16 data) {
	if (MapRead(RBG0_MAP_ADR, x, y) == 0x0004)
		MapWrite(RBG0_MAP_ADR, x, y, 1, data);
}

static void updateSprites(void)
{
	slPrint("updateSprites", slLocate(0,0));
	int i;
	SpriteNode ptr = headNode;
	int count = 0;
	Uint8 deleted ;
	while (ptr != NULL) {
		deleted = 0;
		switch(ptr->sprite.type) {
		case TYPE_NULL:
			break;
		case TYPE_CIRCLE:
			if (ptr->sprite.state != STATE_DEAD) {
				if (MapRead(RBG0_MAP_ADR, fixedToUint16(ptr->sprite.pos[X] >> 4), fixedToUint16(ptr->sprite.pos[Y] >> 4)) == 0x0000) {
					ptr->sprite.state = STATE_DEAD;
					break;
				}
				if (slRandom() > toFIXED(0.5))
					ptr->sprite.ang += 10;
				else
					ptr->sprite.ang -= 10;
				ptr->sprite.dx = slCos(DEGtoANG(ptr->sprite.ang));
				ptr->sprite.dy = slSin(DEGtoANG(ptr->sprite.ang));
				ptr->sprite.pos[X] += ptr->sprite.dx;
				ptr->sprite.pos[Y] += ptr->sprite.dy;
				
				if (MapRead(RBG0_MAP_ADR, fixedToUint16(ptr->sprite.pos[X] >> 4), fixedToUint16(ptr->sprite.pos[Y] >> 4)) == 0x0000) {
					ptr->sprite.pos[X] -= ptr->sprite.dx;
					ptr->sprite.pos[Y] -= ptr->sprite.dy;
					ptr->sprite.ang += 90;
				}
			}
			else {
				if (ptr->sprite.pos[S] > toFIXED(0.05)) //scale down sprite until it disappears
					ptr->sprite.pos[S] -= toFIXED(0.02);
				else {
					deleteSpriteNode(&headNode, ptr);
					deleted = 1;
				}
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
					deleted = 1;
				}
			break;
		}
		count++;
		slPrintHex(count, slLocate(0,2));
		if (ptr->next != NULL && !deleted) 
			ptr = ptr->next;
		else
			break;
	}
}

static void setShotVelocity(FIXED playerX, FIXED playerY, FIXED spriteX, FIXED spriteY, FIXED* dx, FIXED* dy) {
	if (abs(spriteX - playerX) < toFIXED(2))
		*dx = toFIXED(0);
	else if (spriteX < playerX)
		*dx = toFIXED(-2);
	else
		*dx = toFIXED(2);
	
	if (abs(spriteY - playerY) < toFIXED(2) && *dx != toFIXED(0))
		*dy = toFIXED(0);
	else if (spriteY < playerY)
		*dy = toFIXED(-2);
	else
		*dy = toFIXED(2);	
}

static Uint8 checkShotCollision(SpriteNode node) 
{
	slPrint("checkShotCollision", slLocate(0,0));
	SpriteNode ptr = headNode;
	FIXED x = node->sprite.pos[X];
	FIXED y = node->sprite.pos[Y];
	int count = 0;
	while (ptr != NULL) {
		if (ptr != node){
			switch (ptr->sprite.type) {
				case TYPE_CIRCLE:
					if (ptr->sprite.pos[X] - SPR_SIZE[TYPE_CIRCLE] < x && ptr->sprite.pos[X] + SPR_SIZE[TYPE_CIRCLE] > x) {
						if (ptr->sprite.pos[Y] - SPR_SIZE[TYPE_CIRCLE] < y && ptr->sprite.pos[Y] + SPR_SIZE[TYPE_CIRCLE] > y) {
							deleteSpriteNode(&headNode, ptr);
							SPRITE_INFO tmp = defaultSprite;
							tmp.attr = SHOT_ATTR;
							tmp.type = TYPE_SHOT;
							tmp.pos[X] = ptr->sprite.pos[X];
							tmp.pos[Y] = ptr->sprite.pos[Y];
							setShotVelocity(x, y, ptr->sprite.pos[X], ptr->sprite.pos[Y], &tmp.dx, &tmp.dy);
							addSpriteNode(headNode, tmp);
							return 1;
						}
					}
				break;
			}
		}
		count++;
		slPrintHex(count, slLocate(0,2));
		ptr = ptr->next;
	}
	return 0;
	
}

static void dispSprites(void)
{
	slPrint("dispSprites", slLocate(0,0));
	int i;
	FIXED spriteScale = slDivFX(scale, toFIXED(1.0)); //reciprocal
	FIXED spritePos[XYZS];
	SpriteNode ptr = headNode;
	while (ptr != NULL) {
		while (!dispFace && ptr->sprite.type == TYPE_FACE) {
			if (ptr->next != NULL)
				ptr = ptr->next;
			else
				break;
		}
		if (ptr->sprite.type == TYPE_FACE) {
			spritePos[X] = ptr->sprite.pos[X];
			spritePos[Y] = ptr->sprite.pos[Y];
			spritePos[Z] = ptr->sprite.pos[Z];
		}
		else {
			spritePos[X] = slMulFX(ptr->sprite.pos[X] - screenX, spriteScale);
			spritePos[Y] = slMulFX(ptr->sprite.pos[Y] - screenY, spriteScale);
			spritePos[Z] = toFIXED(169);
		}
		slPrintFX(spritePos[X], slLocate(0,4));
		slPrintFX(spritePos[Y], slLocate(0,5));
		if (ptr->sprite.state == STATE_DEAD)
			spritePos[S] = slMulFX(ptr->sprite.pos[S], spriteScale);
		else if (ptr->sprite.type == TYPE_NULL || ptr->sprite.type == TYPE_FACE)
			spritePos[S] = ptr->sprite.pos[S];
		else
			spritePos[S] = spriteScale;
		//slPrintFX(spriteScale, slLocate(0,6));
		slDispSprite(spritePos, &ptr->sprite.attr, DEGtoANG(ptr->sprite.ang));
		if (ptr->next != NULL)
			ptr = ptr->next;
		else
			break;
	}
}

void loadLevel(Uint16 map[])
{
	Map2VRAM(map, (void *)RBG0_MAP_ADR, 64, 64, 1, 0);	
}

void runLevel(void)
{
	gameState = GAME_STATE_START;
	int i;
	#define NORMAL_COL_RATIO 0x0f
	Uint8 colorRatio = 0x00;
	SpriteNode player; //player sprite for animate up
	numSprites = 0;
	dispFace = 0;
	screenX = 0;
	screenY = 0;
	slTVOff();
	set_sprite(pic_sprites, 5, tex_sprites);
	initSprites();
	initVDP2();
	slTVOn();
	
	SPRITE_INFO tmp = defaultSprite;
	tmp.attr = PLAYER_ATTR;
	tmp.type = TYPE_NULL;
	tmp.pos[X] = screenX;
	tmp.pos[Y] = screenY;
	tmp.pos[S] = toFIXED(6.0);
	player = addSpriteNode(headNode, tmp);
	
	while (1) {
		switch (gameState) {
			case GAME_STATE_START:
				updateBG();
				dispSprites();
				slSynch();
				if (player->sprite.pos[S] > toFIXED(1.0)) {
					player->sprite.pos[S] -= toFIXED(0.1);
				}
				else {
					deleteSpriteNode(&headNode, player);
					slScrAutoDisp(NBG0ON | NBG1ON | NBG2ON | NBG3ON | RBG0ON);
					dispFace = 1;
					gameState = GAME_STATE_FADEIN;
				}
			break;
			case GAME_STATE_FADEIN:
				if (colorRatio < NORMAL_COL_RATIO) {
					colorRatio++;
					slColRateNbg1(colorRatio);
					updateBG();
					updateSprites();
					dispSprites();
					slSynch();
				}
				else
					gameState = GAME_STATE_NORMAL;
			break;
			case GAME_STATE_NORMAL:
				handleInput();
				updateBG();
				updateSprites();
				dispSprites();
				slSynch();
				if (numSprites <= NUM_PLAYER_SPRITES) {
					gameState = GAME_STATE_FADEOUT;
				}
			break;
			case GAME_STATE_FADEOUT:
				if (colorRatio > 0) {
					colorRatio--;
					slColRateNbg1(colorRatio);
					updateBG();
					dispSprites();
					slSynch();
				}
				else {
					slScrAutoDisp(NBG0ON | NBG2ON | NBG3ON | RBG0ON);
					dispFace = 0;
					SPRITE_INFO tmp = defaultSprite;
					tmp.attr = PLAYER_ATTR;
					tmp.type = TYPE_NULL;
					tmp.pos[X] = screenX;
					tmp.pos[Y] = screenY;
					tmp.pos[S] = toFIXED(1.0);
					player = addSpriteNode(headNode, tmp);
					gameState = GAME_STATE_COMPLETE;
				}
			break;
			case GAME_STATE_COMPLETE:
				dispSprites();
				updateBG();
				slSynch();
				if (player->sprite.pos[S] < toFIXED(6.0)) {
					player->sprite.pos[S] += toFIXED(0.1);
					player->sprite.ang += 10;
				}
				else {
					clearSpriteList(&headNode);
					return;
				}
			break;
		}
	}
}

