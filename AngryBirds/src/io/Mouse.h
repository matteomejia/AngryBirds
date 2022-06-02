#ifndef MOUSE_H
#define MOUSE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Mouse
{
	// las variables son estaticas porque solo puede existir un mouse

public:
	// callback del cursor
	static void cursorPosCallback(GLFWwindow* window, double _x, double _y);
	// callback para el zoom
	static void mouseWheelCallback(GLFWwindow* window, double dx, double dy);

	// accesores
	static double getMouseX();
	static double getMouseY();
	static double getDX();
	static double getDY();
	static double getScrollDX();
	static double getScrollDY();
private:
	// posicion
	static double x;
	static double y;

	// posicion prev
	static double lastX;
	static double lastY;

	// cambio de posicion
	static double dx;
	static double dy;

	// cambio de giro de la rueda
	static double scrollDx;
	static double scrollDy;

	// primer cambio
	static bool firstMouse;
};

#endif