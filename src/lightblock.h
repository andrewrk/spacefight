#ifndef LIGHTBLOCK_H
#define LIGHTBLOCK_H


static const GLchar *LIGHT_BLOCK_FIELDS[] = {
    "LightDirection",
    "La",
    "Ld",
    "Ls",
    NULL,
};

enum LightBlockField {
    LIGHT_DIRECTION,
    LIGHT_LA,
    LIGHT_LD,
    LIGHT_LS
};

static const GLchar *MATERIAL_BLOCK_FIELDS[] = {
    "Ka",
    "Kd",
    "Ks",
    "Shininess",
    NULL,
};

enum MaterialBlock {
    MATERIAL_KA,
    MATERIAL_KD,
    MATERIAL_KS,
    MATERIAL_SHININESS
};


#endif // LIGHTBLOCK_H
