#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Keyboard
{
public:
    //callback
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    // devuelve el estado de una tecla
    static bool key(int key);
    // devuelve si el estado de una tecla ha cambiado
    static bool keyChanged(int key);
    // devuelve si una tecla se ha levantado
    static bool keyWentUp(int key);
    // devuelve si una tecla se ha presionado
    static bool keyWentDown(int key);

private:
    // array de estados
    static bool keys[];
    // array de cambios
    static bool keysChanged[];
};

#endif