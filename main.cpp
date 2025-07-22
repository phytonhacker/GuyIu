#include "Uincludes.h"

void onButtonClick() {
    std::cout << "Gomb megnyomva!" << std::endl;
}

void onSliderChange(int value) {
    std::cout << "Slider érték: " << value << std::endl;
}

int main() {
    initUI();

    renderButton(50, 50, 200, 100, "Kattints rám", onButtonClick);
    RenderSlider(0, 100, onSliderChange);

    renderUI();
    shutdownUI();

    return 0;
}
