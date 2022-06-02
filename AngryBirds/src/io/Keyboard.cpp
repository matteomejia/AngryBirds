#include "keyboard.h"


// array para guardar los estados de las teclas
bool Keyboard::keys[GLFW_KEY_LAST] = { 0 };

// array que indica si el estado ha cambiado
bool Keyboard::keysChanged[GLFW_KEY_LAST] = { 0 };

// funcion de callback
void Keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_RELEASE)
	{ // si es una tecla levantada
		if (!keys[key])
		{
			keys[key] = true;
		}
	}
	else
	{ // si es una tecla presionada
		keys[key] = false;
	}
	keysChanged[key] = action != GLFW_REPEAT;
}

// devuelve el estado de una tecla
bool Keyboard::key(int key)
{
	return keys[key];
}

// devuelve si el estado cambio, sirve para evitar registrar toques seguidos
bool Keyboard::keyChanged(int key)
{
	bool ret = keysChanged[key];
	keysChanged[key] = false;
	return ret;
}

// tecla es presionada
bool Keyboard::keyWentDown(int key)
{
	return keys[key] && keyChanged(key);
}

// tecla es levantada
bool Keyboard::keyWentUp(int key)
{
	return !keys[key] && keyChanged(key);
}