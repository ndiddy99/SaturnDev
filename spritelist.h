#ifndef SPRITELIST_H
#define SPRITELIST_H

typedef struct SpriteInfo {
	FIXED pos[XYZS];
	Uint16 ang;
	SPR_ATTR attr;
	FIXED dx, dy;
	Uint8 type;
	Uint8 state;
} SPRITE_INFO;

struct SpriteList {
	SPRITE_INFO sprite;
	struct SpriteList* next;
};

typedef struct SpriteList* SpriteNode;

#define fixedToUint16(fixed) (fixed >> 16)

SpriteNode createSpriteNode();
SpriteNode addSpriteNode(SpriteNode head, SPRITE_INFO data);
void deleteSpriteNode(SpriteNode* head, SpriteNode node);
void clearSpriteList(SpriteNode* head);

#endif
