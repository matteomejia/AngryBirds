#include "mouse.h"

// definiciones estaticas porque no hay constructor

double Mouse::x = 0;
double Mouse::y = 0;

double Mouse::lastX = 0;
double Mouse::lastY = 0;

double Mouse::dx = 0;
double Mouse::dy = 0;

double Mouse::scrollDx = 0;
double Mouse::scrollDy = 0;

bool Mouse::firstMouse = true;

// callback del cursor
void Mouse::cursorPosCallback(GLFWwindow* window, double _x, double _y)
{
    x = _x;
    y = _y;

    if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    dx = x - lastX;
    dy = lastY - y;
    lastX = x;
    lastY = y;
}

// callback de la rueda
void Mouse::mouseWheelCallback(GLFWwindow* window, double dx, double dy)
{
    scrollDx = dx;
    scrollDy = dy;
}

// accesores
double Mouse::getMouseX()
{
    return x;
}
double Mouse::getMouseY()
{
    return y;
}
double Mouse::getDX()
{
    double _dx = dx;
    dx = 0;
    return _dx;
}
double Mouse::getDY()
{
    double _dy = dy;
    dy = 0;
    return _dy;
}
double Mouse::getScrollDX()
{
    double _scrollDx = scrollDx;
    scrollDx = 0;
    return _scrollDx;
}
double Mouse::getScrollDY()
{
    double _scrollDy = scrollDy;
    scrollDy = 0;
    return _scrollDy;
}
