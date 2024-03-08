#define PROJECTNAME "MglExamples"
#define PROJECTVER PROJECTNAME ## _versions
#define USEMSV_MSVCORE

#define USEMSV_MLIST
#define USEMSV_MWND

#define USEMSV_XDATACONT
#define USEMSV_HTTP
#define USEMSV_CONSOLE

#include "../msvcore2/msvcore.cpp"

Versions PROJECTVER[] = {
    // new version to up
	"0.0.0.2", "08.03.2024 18:30",
	"0.0.0.1", "08.03.2024 12:17"
};

// GL3W
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3dll.lib")

// OpenGL
#include "../msvcore2/opengl/mgl.h"

////////////////////////////////////////////////////////////////////
// Select examples here <<<-----------------------------------------
//#define MGL_EXAMPLE_TRIANGLE // Show triangle
//#define MGL_EXAMPLE_TEXTURE // Show texture
#define MGL_EXAMPLE_SHADER // Show shader like shadertoy: glsl/shader.fs
#define MGL_EXAMPLE_OBJECTS // Show objects circles and lines
#define MGL_EXAMPLE_MENU // Show shader menu on Right click
#define MGL_EXAMPLE_POPUP // Show popup messages
// End select examples
////////////////////////////////////////////////////////////////////

// Windows
#include "Glsl.h"
#include "Windows.h"

int main(int args, char* arg[], char* env[]){
	// Init
	print(PROJECTNAME, " v.", PROJECTVER[0].ver, " (", PROJECTVER[0].date, ").\r\n");
	msvcoremain(args, arg, env);

	// Glfw
	glfwInit();
	glfwWindowHint ( GLFW_RESIZABLE, 1 );
	
	// Window
	GLFWwindow* window;
	char null = '\0';
	window = glfwCreateWindow(MglWindows.screen.x, MglWindows.screen.y, VString(LString() + PROJECTNAME + " " + PROJECTVER[0].ver + VString(&null, 1)), 0, NULL); // 

	glfwMakeContextCurrent(window);

	// Load OpenGL
	gladLoadGL();
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

	// OpenGL flags
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// OpgnGL Debug
	MsvGlDebug(1, 0);

	// Random init
	srand(time());

	// Init windows
	MglWindows.Init();
	WndResize(window, MglWindows.screen.x, MglWindows.screen.y);

	// Add objects if active (MGL_EXAMPLE_OBJECTS)
	if(MglWindows.glsl_objects.GetProgramId()){
		GlslLayer &lay = MglWindows.layer;

		// Add circle 
		lay.AddCircle(0, 0, 100);

		lay.AddCircle(200, 200, 100);

		// Add circle
		//for(int x = -100; x < 100; x += 100){
		//	for(int y = -100; y < 100; y += 100){
		//		lay.AddCircle(x, y, 100);
		//	}
		//}
		
		// Add path
		lay.AddPath();
		lay.AddPoi(10, 10);
		lay.AddPoi(100, 10);
		lay.AddPoi(100, 100);
		lay.AddPoi(10, 100);

		// Set data to glsl
		MglWindows.layer.Draw(MglWindows.glsl_objects);
	}

	// PopUp if active (MGL_EXAMPLE_POPUP)
	MglWindows.InsertPopUp(LString() + "Wellcome to MglExamples " + PROJECTVER[0].ver + ".");
	MglWindows.InsertPopUp(LString() + "We have a cookies!");

	// Callbacks
	glfwSetWindowSizeCallback(window, WndResize);
	glfwSetKeyCallback(window, WndKeyCallback);
	glfwSetMouseButtonCallback(window, WndMouseClickCallback);
	glfwSetCursorPosCallback(window, WndMouseMotionCallback);
	glfwSetScrollCallback(window, WndMouseScrollCallback);
	glfwSetDropCallback(window, WndDrop);

	// Process
	while(!glfwWindowShouldClose(window)){
		// Update windows
		WndUpdate(window);

		// Render scene
		WndRenderScene(window);

		// Swap buffers
		glfwSwapBuffers(window);

		// Pool Events
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	return 0;
}