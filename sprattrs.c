#ifndef SPRATTRS_C
#define SPRATTRS_C

//sprite attributes
SPR_ATTR PLAYER_ATTR = SPR_ATTRIBUTE(0,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip); //used only for death animation
SPR_ATTR EYE_ATTR = SPR_ATTRIBUTE(1,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR SCLERA_ATTR = SPR_ATTRIBUTE(2,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR CIRCLE_ATTR = SPR_ATTRIBUTE(3,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR SHOT_ATTR = SPR_ATTRIBUTE(4,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR PUSH_ATTR = SPR_ATTRIBUTE(5,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);

//needs to be in the same order as sprite type defines in spritelist.h
FIXED SPR_SIZE[] = {toFIXED(0), toFIXED(16), toFIXED(8), toFIXED(0), toFIXED(16)}; //collision radius
SPR_ATTR* SPR_ATTRS[] = {NULL, &CIRCLE_ATTR, &SHOT_ATTR, NULL, &PUSH_ATTR};


#endif
