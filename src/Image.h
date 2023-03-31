//
// Created by Xaine on 30/03/2023.
//
#include <d3dx9tex.h>

class Image {
public:
    explicit Image(PDIRECT3DTEXTURE9 pTexture9, int i, int i1) {
        texture = pTexture9;
        width = i;
        height = i1;
    }

    int width;
    int height;
    PDIRECT3DTEXTURE9 texture;
};

