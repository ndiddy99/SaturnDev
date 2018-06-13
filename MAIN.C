/*----------------------------------------------------------------------*/
/*	Pad Control							*/
/*----------------------------------------------------------------------*/
#include	"sgl.h"
#include	"sega_sys.h"

#include "structs.h"
#include "assetrefs.h"

#define		RBG0_CEL_ADR		VDP2_VRAM_A0
#define		RBG0_MAP_ADR		VDP2_VRAM_B0
#define		RBG0_COL_ADR		( VDP2_COLRAM + 0x00200 )
#define		RBG0_PAR_ADR		( VDP2_VRAM_A1 + 0x1fe00 )
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
		// {t,(a)|(((d)>>24)&0xc0),c,g,(d)&0x0f3f}
		sprites[i].attr = DEFAULT_ATTR;
	}
}

void updateBG(void)
{
	if (state == STATE_FALLING) {
		scaleSpeed -= toFIXED(0.001);
		scale += scaleSpeed;
		if (scale <= toFIXED(0.2)) {
			state = STATE_RISING;
			scaleSpeed = toFIXED(0.05);
			slPrintFX((screenX >> 4) & 0xffff0000, slLocate(0,1));
			slPrintFX((screenY >> 4) & 0xffff0000, slLocate(0,2));
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
	slPrintFX(scaleSpeed, slLocate(0,0));
	slPrintFX(scale, slLocate(0,3));
}

void dispSprites(void)
{
	int i;
	FIXED spriteScale = slDivFX(scale, toFIXED(1.0)); //reciprocal
	for (i = 0; i < numDispSprites; i++) {
		sprites[i].pos[X] = slMulFX(-screenX, spriteScale);
		sprites[i].pos[Y] = slMulFX(-screenY, spriteScale);
		sprites[i].pos[S] = spriteScale;
		slPrintFX(spriteScale, slLocate(0,4));
		slDispSprite(sprites[i].pos, &sprites[i].attr, sprites[i].ang);
	}
}
 
void ss_main(void)
{
	slInitSystem(TV_320x224,tex_sprites,1);
	slTVOff();
	set_sprite(pic_sprites, 3, tex_sprites);
	initSprites();
	
	slColRAMMode(CRM16_2048);
	slBack1ColSet((void *)BACK_COL_ADR , 0);
	
	slRparaInitSet((void *)RBG0_PAR_ADR);
	slCharRbg0(COL_TYPE_256, CHAR_SIZE_2x2);
	slPageRbg0((void *)RBG0_CEL_ADR, 0, PNB_1WORD|CN_12BIT);
	slPlaneRA(PL_SIZE_1x1);
	sl1MapRA((void *)RBG0_MAP_ADR);
	slOverRA(2);
	Cel2VRAM(cel_map1, (void *)RBG0_CEL_ADR, 3 * 64 * 4);
	Map2VRAM(map_map1, (void *)RBG0_MAP_ADR, 64, 64, 1, 0);
	Pal2CRAM(pal_map1, (void *)RBG0_COL_ADR, 256);
	
	slDispCenterR(toFIXED(160.0) , toFIXED(112.0));
	slLookR(toFIXED(0.0) , toFIXED(0.0));
	
	slScrAutoDisp(NBG0ON | RBG0ON);
	slTVOn();
	sprites[0].pos[X] = toFIXED(0.0);
	while(1) {
		handleInput();
		updateBG();
		dispSprites();
		slSynch();
	} 
}

