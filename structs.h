typedef struct SpriteInfo {
	FIXED pos[XYZS];
	ANGLE ang;
	SPR_ATTR attr;
	Uint8 type;
} SPRITE_INFO;

#define TYPE_NULL 0
#define TYPE_CIRCLE 1

struct SpriteList {
	SPRITE_INFO sprite;
	struct SpriteList* next;
};

typedef struct SpriteList* SpriteNode;

