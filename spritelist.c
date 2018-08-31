#include "sgl.h"
#include "game.h"
#include "spritelist.h"

int addSprite(SPRITE_INFO data)
{
	int i;
	for (i = 0; i < sizeof(sprites) / sizeof(SPRITE_INFO); i++) {
		if (sprites[i].state == SPRITE_STATE_NODISP) {
			// sprites[i] = data;
			memcpy(&sprites[i], &data, sizeof(SPRITE_INFO));
			return i;
		}
	}
	return -1;
}

void clearSpriteList(void) //clears all sprites in the SpriteList
{
	memset(sprites, 0, sizeof(sprites));
}
