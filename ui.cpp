#include "Uincludes.h"
#include <X11/Xlib.h>
#include <iostream>
#include <cstring>
#include <windows.h>
#include <gl/gl.h> 
unigned long RGBtoPixel(Display* display, int screen, Color color) {
    XColor xcolor;
    xcolor.red = color.r;
    xcolor.green = color.g;
    xcolor.blue = color.b;
    xcolor.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(display, DefaultColormap(display, screen), &xcolor)) {
        std::cerr << "Nem sikerült színt lefoglalni!\n";
        return BlackPixel(display, screen);
    }
    return xcolor.pixel;
}

Display* display = nullptr;
Window window;
int screen;

struct Button {
    int x, y, w, h;
    const char* label;
    void (*onClick)();
    Color bgColor;
    Color textColor;
};

struct Slider {
    int minValue;
    int maxValue;
    int currentValue;
    int x, y, w, h;
    void (*onChange)(int);
    Color trackColor;
    Color knobColor;
};

Button currentButton;
Slider currentSlider;
bool sliderInitialized = false;
bool sliderDragging = false;

void initUI() {
    display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "Nem tudom megnyitni az X11 display-t!\n";
        exit(1);
    }

    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display,
                                 RootWindow(display, screen),
                                 10, 10, 800, 600, 1,
                                 BlackPixel(display, screen),
                                 WhitePixel(display, screen));

    XSelectInput(display, window, ExposureMask | KeyPressMask |
                                ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    XMapWindow(display, window);
}

void renderButton(int x, int y, int w, int h, const char* label, void (*onClick)(),
                  Color bgColor, Color textColor)
{
    currentButton = { x, y, w, h, label, onClick, bgColor, textColor };
}

void RenderSlider(int minValue, int maxValue, void (*onChange)(int),
                  Color trackColor, Color knobColor)
{
    currentSlider = { minValue, maxValue, minValue, 50, 200, 300, 20, onChange, trackColor, knobColor };
    sliderInitialized = true;
}

void renderUI() {
    XEvent event;
    bool running = true;

    GC gc = XCreateGC(display, window, 0, 0);
    XFontStruct* font = XLoadQueryFont(display, "fixed");

    if (!font) {
        std::cerr << "Nem található betűtípus\n";
        exit(1);
    }

    XSetFont(display, gc, font->fid);

    auto drawAll = [&]() {

        XSetForeground(display, gc, RGBtoPixel(display, screen, currentButton.bgColor));
        XFillRectangle(display, window, gc,
                       currentButton.x, currentButton.y,
                       currentButton.w, currentButton.h);


        XSetForeground(display, gc, RGBtoPixel(display, screen, currentButton.textColor));
        XDrawString(display, window, gc,
                    currentButton.x + 10,
                    currentButton.y + currentButton.h / 2,
                    currentButton.label, std::strlen(currentButton.label));


        if (sliderInitialized) {
            XSetForeground(display, gc, RGBtoPixel(display, screen, currentSlider.trackColor));
            XFillRectangle(display, window, gc,
                           currentSlider.x,
                           currentSlider.y + currentSlider.h / 2 - 2,
                           currentSlider.w, 4);

            int knobX = currentSlider.x +
                ((currentSlider.currentValue - currentSlider.minValue) * currentSlider.w) /
                (currentSlider.maxValue - currentSlider.minValue);

            XSetForeground(display, gc, RGBtoPixel(display, screen, currentSlider.knobColor));
            XFillRectangle(display, window, gc,
                           knobX - 5, currentSlider.y, 10, currentSlider.h);
        }
    };

    while (running) {
        XNextEvent(display, &event);

        if (event.type == Expose) {
            drawAll();
        }

        if (event.type == KeyPress) {
            running = false;
        }

        if (event.type == ButtonPress) {
            int cx = event.xbutton.x;
            int cy = event.xbutton.y;


            if (cx >= currentButton.x && cx <= currentButton.x + currentButton.w &&
                cy >= currentButton.y && cy <= currentButton.y + currentButton.h) {
                if (currentButton.onClick) {
                    currentButton.onClick();
                }
            }


            if (sliderInitialized &&
                cx >= currentSlider.x && cx <= currentSlider.x + currentSlider.w &&
                cy >= currentSlider.y && cy <= currentSlider.y + currentSlider.h) {
                sliderDragging = true;
            }
        }

        if (event.type == ButtonRelease) {
            sliderDragging = false;
        }

        if (event.type == MotionNotify && sliderDragging) {
            int cx = event.xmotion.x;

            if (sliderInitialized) {
                int relativeX = cx - currentSlider.x;
                if (relativeX < 0) relativeX = 0;
                if (relativeX > currentSlider.w) relativeX = currentSlider.w;

                int newValue = currentSlider.minValue +
                    relativeX * (currentSlider.maxValue - currentSlider.minValue) / currentSlider.w;

                if (newValue != currentSlider.currentValue) {
                    currentSlider.currentValue = newValue;
                    if (currentSlider.onChange) {
                        currentSlider.onChange(currentSlider.currentValue);
                    }
                    XClearWindow(display, window);
                    drawAll();
                }
            }
        }
    }

    XFreeGC(display, gc);
    XFreeFont(display, font);
}

void shutdownUI() {
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}
