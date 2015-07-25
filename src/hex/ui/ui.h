#ifndef UI_H
#define UI_H

class UiWindow {
public:
    UiWindow(int x, int y, int width, int height);
    virtual ~UiWindow();

    virtual void draw() = 0;

public:
    int x, y;
    int width, height;
};

#endif
