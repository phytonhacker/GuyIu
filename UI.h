// UI.h
#pragma once

struct Color {
    unsigned short r, g, b;
};

void initUI();
void renderButton(int x, int y, int w, int h, const char* label, void (*onClick)(),
                  Color bgColor = {0xFFFF, 0x0000, 0x0000},
                  Color textColor = {0xFFFF, 0xFFFF, 0xFFFF});
void RenderSlider(int minValue, int maxValue, void (*onChange)(int),
                  Color trackColor = {0xAAAA, 0xAAAA, 0xAAAA},
                  Color knobColor = {0x0000, 0x0000, 0xFFFF});
void renderUI();
void shutdownUI();
