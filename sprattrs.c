#ifndef SPRATTRS_C
#define SPRATTRS_C

//sprite attributes
SPR_ATTR PLAYER_ATTR = SPR_ATTRIBUTE(0,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip); //used only for death animation
SPR_ATTR EYE_ATTR = SPR_ATTRIBUTE(1,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR SCLERA_ATTR = SPR_ATTRIBUTE(2,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR CIRCLE_ATTR = SPR_ATTRIBUTE(3,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR SHOT_ATTR = SPR_ATTRIBUTE(4,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR PUSH_ATTR = SPR_ATTRIBUTE(5,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);

//number sprite attributes
SPR_ATTR ZERO_ATTR  = SPR_ATTRIBUTE(30,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR ONE_ATTR   = SPR_ATTRIBUTE(31,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR TWO_ATTR   = SPR_ATTRIBUTE(32,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR THREE_ATTR = SPR_ATTRIBUTE(33,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR FOUR_ATTR  = SPR_ATTRIBUTE(34,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR FIVE_ATTR  = SPR_ATTRIBUTE(35,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR SIX_ATTR   = SPR_ATTRIBUTE(36,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR SEVEN_ATTR = SPR_ATTRIBUTE(37,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR EIGHT_ATTR = SPR_ATTRIBUTE(38,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);
SPR_ATTR NINE_ATTR  = SPR_ATTRIBUTE(39,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip);

SPR_ATTR LIFE_ATTR  = SPR_ATTRIBUTE(40,No_Palet,No_Gouraud,CL32KRGB|SPenb|ECdis,sprNoflip); //for lives icon

//needs to be in the same order as sprite type defines in spritelist.h
FIXED SPR_SIZE[] = {toFIXED(0), toFIXED(16), toFIXED(8), toFIXED(0), toFIXED(16)}; //collision radius
SPR_ATTR* SPR_ATTRS[] = {NULL, &CIRCLE_ATTR, &SHOT_ATTR, NULL, &PUSH_ATTR};
int POINTS[] = {0, 100, 0, 0, 150}; //points per thing

SPR_ATTR* DIGITS[] = {&ZERO_ATTR, &ONE_ATTR, &TWO_ATTR, &THREE_ATTR, &FOUR_ATTR, &FIVE_ATTR, &SIX_ATTR, &SEVEN_ATTR, &EIGHT_ATTR, &NINE_ATTR};

#endif
