typedef struct SpriteInfo {
	FIXED pos[XYZS];
	ANGLE ang;
	SPR_ATTR attr;
	FIXED dx, dy;
	Uint8 type;
} SPRITE_INFO;

const SPR_ATTR CIRCLE_ATTR = SPR_ATTRIBUTE(0,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
const SPR_ATTR SHOT_ATTR = SPR_ATTRIBUTE(1,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
#define TYPE_NULL 0
#define TYPE_CIRCLE 1
#define TYPE_SHOT 2

const FIXED SPR_SIZE[] = {toFIXED(0), toFIXED(16), toFIXED(8)};
struct SpriteList {
	SPRITE_INFO sprite;
	struct SpriteList* next;
};

typedef struct SpriteList* SpriteNode;

