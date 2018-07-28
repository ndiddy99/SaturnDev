typedef struct SpriteInfo {
	FIXED pos[XYZS];
	Uint16 ang;
	SPR_ATTR attr;
	FIXED dx, dy;
	Uint8 type;
	Uint8 state;
} SPRITE_INFO;

const SPR_ATTR CIRCLE_ATTR = SPR_ATTRIBUTE(0,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
const SPR_ATTR SHOT_ATTR = SPR_ATTRIBUTE(1,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
const SPR_ATTR PLAYER_ATTR = SPR_ATTRIBUTE(2,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip); //used only for death animation
const SPR_ATTR EYE_ATTR = SPR_ATTRIBUTE(3,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
const SPR_ATTR SCLERA_ATTR = SPR_ATTRIBUTE(4,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis|MESHon,sprNoflip);
#define TYPE_NULL 0
#define TYPE_CIRCLE 1
#define TYPE_SHOT 2
#define TYPE_FACE 3 //what makes up char's face

#define STATE_NORM 0
#define STATE_FALL 1

const FIXED SPR_SIZE[] = {toFIXED(0), toFIXED(16), toFIXED(8)};
struct SpriteList {
	SPRITE_INFO sprite;
	struct SpriteList* next;
};

typedef struct SpriteList* SpriteNode;

