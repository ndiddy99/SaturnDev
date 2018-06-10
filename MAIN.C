/*----------------------------------------------------------------------*/
/*	Pad Control							*/
/*----------------------------------------------------------------------*/
#include	"sgl.h"
#include	"sega_sys.h"

#include "structs.h"
#include "assetrefs.h"

#define		NBG1_CEL_ADR		( VDP2_VRAM_B1 + 0x02000 )
#define		NBG1_MAP_ADR		( VDP2_VRAM_B1 + 0x12000 )
#define		NBG1_COL_ADR		( VDP2_COLRAM + 0x00200 )
#define		BACK_COL_ADR		( VDP2_VRAM_A1 + 0x1fffe )

#define NUM_SPRITES 200
SPRITE_INFO sprites[NUM_SPRITES];
const SPR_ATTR DEFAULT_ATTR = SPR_ATTRIBUTE(0,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
int numDispSprites = 1;

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
		sprites[0].pos[X] += toFIXED(1.0);
	else if (data & PER_DGT_KL)
		sprites[0].pos[X] -= toFIXED(1.0);
	if (data & PER_DGT_KU)
		sprites[0].pos[Y] -= toFIXED(1.0);
	else if (data & PER_DGT_KD)
		sprites[0].pos[Y] += toFIXED(1.0);
	if (data & PER_DGT_TA)
		sprites[0].pos[S] += toFIXED(0.1);
	if (data & PER_DGT_TB)
		sprites[0].pos[S] -= toFIXED(0.1);
	if (data & PER_DGT_TL)
		sprites[0].ang += DEGtoANG(1.0);
	if (data & PER_DGT_TR)
		sprites[0].ang -= DEGtoANG(1.0);
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

void dispSprites(void)
{
	int i;
	for (i = 0; i < numDispSprites; i++) {
		// sprites[i].pos[X] = toFIXED((slRandom() % 320) - 160);
		// sprites[i].pos[Y] = toFIXED((slRandom() % 224) - 112);
		slDispSprite(sprites[i].pos, &sprites[i].attr, sprites[i].ang);
	}
}

void loadBmp(Uint16 *vram, Uint16 vramWidth, Uint16 *img, Uint16 imgWidth, Uint16 imgHeight) {
	int i;
	Uint32 vramOffset = 0;
	Uint32 imgOffset = 0;
	
	for (i = 0; i < imgHeight; i++) {
		memcpy((void *)(vram + vramOffset), (void *)(img + imgOffset), imgWidth * sizeof(Uint16));
		vramOffset += vramWidth;
		imgOffset += imgWidth;
	}
}

 

void ss_main(void)
{
	slInitSystem(TV_320x224,tex_sprites,1);
	slTVOff();
	set_sprite(pic_sprites, 3, tex_sprites);
	initSprites();
	slPrint("dongo" , slLocate(9,2));
	
	slColRAMMode(CRM16_1024);
	slBack1ColSet((void *)BACK_COL_ADR , 0);
	slBitMapNbg1(COL_TYPE_32768, BM_512x256, (void *)VDP2_VRAM_A0);
	loadBmp((Uint16 *)VDP2_VRAM_A0, 512, waifu, 256, 220);
	
	slScrPosNbg1(toFIXED(0.0) , toFIXED(0.0));
	slScrAutoDisp(NBG0ON | NBG1ON);
	slTVOn();
	sprites[0].pos[X] = toFIXED(0.0);
	while(1) {
		handleInput();
		dispSprites();
		slSynch();
	} 
}

