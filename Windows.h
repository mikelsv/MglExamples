#define MGL_ZOOM_MAX	25000
#define MGL_ZOOM_MIN	.05

class MglWindows{
public:
	// Screen
	KiVec2 screen, move, mouse;
	float zoom;

	// Glsl
	MglSimpleGlslTriangle glsl_triangle;
	MglGlslTexture glsl_texture;
	GlslShader glsl_shader;
	MglMenu glsl_menu;
	MglPopUp glsl_popup;

	// Timer
	MTimer ttime;
	double stime;

	// FPS
	double fps_time;
	int fps, fps_count;

	MglWindows(){
		// Set window screen size
		screen = KiInt2(1024, 768);
		move = KiInt2(0, 0);
		zoom = 1.;
		fps_time = 0;
	}

	// Init
	void Init(){
		// Load font: glsl/font.bin
		GlslFontTexture.Init();

#ifdef MGL_EXAMPLE_TRIANGLE
		glsl_triangle.Init();
#endif

#ifdef MGL_EXAMPLE_TEXTURE
		glsl_texture.Init();
#endif

#ifdef MGL_EXAMPLE_SHADER
		glsl_shader.Init();
		glsl_shader.SetFont(GlslFontTexture.GetTexture());
#endif

#ifdef MGL_EXAMPLE_MENU
		glsl_menu.Init();
		glsl_menu.SetFont(GlslFontTexture.GetTexture());
#endif

#ifdef MGL_EXAMPLE_POPUP
		glsl_popup.Init();
		glsl_popup.SetFont(GlslFontTexture.GetTexture());
#endif

		// Timer
		ttime.Init();
		stime = ttime.dtime();
	}

	// Insert
	void InsertPopUp(VString text, int lifetime = 30){
		if(glsl_popup.GetProgramId())
			glsl_popup.Insert(text, lifetime);
	}

	// Update
	void Resize(int x, int y){
		screen = KiInt2(x, y);

		if(glsl_shader.GetProgramId())
			glsl_shader.UpdateRes(screen);

		if(glsl_popup.GetProgramId())
			glsl_popup.UpdateRes(screen);
	}

	void UpdateFps(){
		fps_count ++;

		if(glfwGetTime() - fps_time >= 1.0){		
			fps_time += 1.0;

			fps = fps_count;
			fps_count = 0;

			if(glsl_shader.GetProgramId())
				glsl_shader.UpdateFps(fps);
		}
	}

	// Mouse
	void UpdateMouseMove(double x, double y){
		mouse = KiVec2(x, y);

		if(glsl_shader.GetProgramId())
			glsl_shader.UpdateMouseSelect(KiVec2(x, screen.y - y), KiVec2(0), 0);

		if(glsl_menu.GetProgramId())
			glsl_menu.UpdateMouse(KiVec2(x, screen.y - y));
	}

	void UpdateMouseClick(int button, int action){
		// Menu mouse
		if(glsl_menu.GetProgramId()){

			// Show menu
			if(button == GLFW_MOUSE_BUTTON_2 && action == 0){
				glsl_menu.UpdateMouseMenu(KiVec2(mouse.x, screen.y - mouse.y));

				glsl_menu.CleanMenu();

				glsl_menu.InsertItem(0, "Hello World!");
				glsl_menu.InsertItem(1, "1234567890");
				glsl_menu.InsertItem(2, "");
				glsl_menu.InsertItem(1, "London is the...");
			
				glsl_menu.DrawMenu();
			}else{ // Hide menu
				glsl_menu.CleanMenu();
				glsl_menu.DrawMenu();
			}
		}

		// PopUp mouse
		if(glsl_popup.GetProgramId() && button == GLFW_MOUSE_BUTTON_1 && action == 1){
			// Close PopUp
			glsl_popup.OnCLick(KiVec2(mouse.x, screen.y - mouse.y));
		}

	}

	void UpdateMouseZoom(int offset){
		float zoom_new;

		if(offset > 0)
			zoom_new = min(MGL_ZOOM_MAX, (1 + 1 / 3.0) * zoom);
		else
			zoom_new = max(MGL_ZOOM_MIN, (1 - 1 / 3.0) * zoom);

		zoom = zoom_new;

		// Update shaders
		if(glsl_shader.GetProgramId())
			glsl_shader.UpdateZoom(zoom);
	}

	// Render
	void Render(){
		float iTime = (ttime.dtime() - stime) / 1000;

		// if GetProgramId() != 0 then Render()
		if(glsl_triangle.GetProgramId())
			glsl_triangle.Render();

		if(glsl_texture.GetProgramId())
			glsl_texture.Render(GlslFontTexture.GetTexture());

		if(glsl_shader.GetProgramId())
			glsl_shader.Render(iTime);

		if(glsl_menu.GetProgramId())
			glsl_menu.Render(iTime);

		if(glsl_popup.GetProgramId())
			glsl_popup.Render(iTime);
	}

} MglWindows;

static void WndUpdate(GLFWwindow* window){
	MglWindows.UpdateFps();
}

static void WndRenderScene(GLFWwindow* window){
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

	MglWindows.Render();
}

// Cllback on windows resize
static void WndResize(GLFWwindow* window, int w, int h){
    if (h < 1)
		return ;

	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	MglWindows.Resize(w, h);

	return ;
}

// Callback on Key pressed
static void WndKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
	return ;
}

static void WndMouseClickCallback(GLFWwindow* window, int button, int action, int mods){
	MglWindows.UpdateMouseClick(button, action);
	/*
	switch (button) {
		case GLFW_MOUSE_BUTTON_1:
			GiCadWindows.mouse[0].Click(action);
			GiCadWindows.glsl_main.UpdateMouseSelect(GiCadWindows.mouse[0].cur, GiCadWindows.mouse[0].hold, GiCadWindows.mouse[0].down);
			//GiCadWindows.mouse_hold = GiCadWindows.mouse_cur;
			//GiCadWindows.mouse_down[1] = action;
			//GiCadWindows.glsl.UpdateMouseSelect(GiCadWindows.mouse[0].cur, );

			if(GiCadWindows.glsl_menu.IsActive())
				GiCadWindows.HideMenu();

			GiCadWindows.glsl_popup.OnCLick(KiVec2(GiCadWindows.mouse[2].cur.x, GiCadWindows.size.y - GiCadWindows.mouse[2].cur.y));

		break;

		case GLFW_MOUSE_BUTTON_2:
			if(action == 0 && GiCadWindows.mouse[GLFW_MOUSE_BUTTON_2].down)
				GiCadWindows.ShowMenu();

			if(action == 1)
				GiCadWindows.HideMenu();

			GiCadWindows.mouse[GLFW_MOUSE_BUTTON_2].Click(action);
		break;

		case GLFW_MOUSE_BUTTON_3:
		//MaticalsOpenGl.mouse_down = 1;
		//dragging = action;
		//GiCadWindows.mouse_down[3] = action;
		GiCadWindows.mouse[2].Click(action);
		//
		break;
	}*/
}

static void WndMouseMotionCallback(GLFWwindow* window, double x, double y){
	MglWindows.UpdateMouseMove(x, y);

	//MaticalsOpenGl.SetMouse(KiInt2(x, y));
	/*
	// Left button
	GiCadWindows.mouse[0].cur = KiVec2(x, GiCadWindows.size.y - y) / GiCadWindows.zoom / GISCREENZOOM;
	GiCadWindows.glsl_main.UpdateMouseSelect(GiCadWindows.mouse[0].cur, GiCadWindows.mouse[0].hold, GiCadWindows.mouse[0].down);

	// Center button
	if(GiCadWindows.mouse[2].down){
		KiVec2 delta = GiCadWindows.mouse[2].hold - KiVec2(x, y);
		GiCadWindows.move += KiVec2(delta.x, delta.y * MGLYSCALE);

		// Limits
		if(GiCadWindows.move.x < -5000)
			GiCadWindows.move.x = -5000;
		if(GiCadWindows.move.x > 5000)
			GiCadWindows.move.x = 5000;
		if(GiCadWindows.move.y < -5000)
			GiCadWindows.move.y = -5000;
		if(GiCadWindows.move.y > 5000)
			GiCadWindows.move.y = 5000;

		GiCadWindows.mouse[2].hold += GiCadWindows.mouse[2].hold - KiInt2(x, y);
		GiCadWindows.glsl_main.UpdateMove(GiCadWindows.move);
		GiCadWindows.glsl_objects.UpdateMove(GiCadWindows.move);
	}

	GiCadWindows.mouse[2].cur = KiInt2(x, y);
	GiCadWindows.mouse[2].hold = KiInt2(x, y);

	if(GiCadWindows.glsl_menu.IsActive())
		GiCadWindows.glsl_menu.UpdateMouse(KiInt2(x, GiCadWindows.size.y - y));

	//print("Mouse ", itos(GiCadWindows.mouse[2].cur.x), "\r\n");

	//GiCadWindows.mouse_move = KiInt2(x, y);

	//if (dragging) {
	//	mousex += x;
	//	mousey += y;
	//}
	*/
}

static void WndMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset){
	MglWindows.UpdateMouseZoom(yoffset);
	/*
	double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

	float zoom = GiCadWindows.zoom + yoffset * 0.2f;
	//KiVec2 move = KiVec2(GiCadWindows.size) / GiCadWindows.zoom;
	

	if(yoffset > 0){
		zoom = min(GICAD_ZOOM_MAX, (1 + 1 / 3.0) * GiCadWindows.zoom);
		//GiCadWindows.move.x += (GiCadWindows.size.x / GiCadWindows.zoom - GiCadWindows.size.x / zoom) / 2.;
		//GiCadWindows.move.y += (GiCadWindows.size.y / GiCadWindows.zoom - GiCadWindows.size.y / zoom) / 2.;
	}
	else
		zoom = max(GICAD_ZOOM_MIN, (1 - 1 / 3.0) * GiCadWindows.zoom);

	//KiVec2 size = KiVec2(GiCadWindows.size);
	KiVec2 move = (GiCadWindows.size / GiCadWindows.zoom - GiCadWindows.size / zoom) / 2.;
	//GiCadWindows.move = GiCadWindows.move + (GiCadWindows.size / GiCadWindows.zoom - GiCadWindows.size / zoom) / 2.;
	

	float center_x = GiCadWindows.size.x / 2;
	float center_y = GiCadWindows.size.y / 2;

	GiCadWindows.move = KiInt2(GiCadWindows.move.x * GiCadWindows.zoom / zoom, GiCadWindows.move.y * GiCadWindows.zoom / zoom);
	
	
	//GiCadWindows.move += KiInt2((xpos - center_x) * GiCadWindows.zoom / zoom, (ypos - center_y) * GiCadWindows.zoom / zoom);
	//GiCadWindows.move -= KiInt2(xpos * yoffset, 0);
	//GiCadWindows.move += KiInt2((xpos - center_x) * GiCadWindows.zoom / zoom, (ypos - center_y) * GiCadWindows.zoom / zoom);

	GiCadWindows.zoom = zoom;

	GiCadWindows.glsl_main.UpdateZoom(GiCadWindows.zoom);
	GiCadWindows.glsl_main.UpdateMove(GiCadWindows.move);

	//
	GiCadWindows.glsl_objects.UpdateZoom(GiCadWindows.zoom);
	GiCadWindows.glsl_objects.UpdateMove(GiCadWindows.move);

	//glfwSetCursorPos(window, GiCadWindows.size.x / 2, GiCadWindows.size.y / 2);
//	GiCadWindows.mouse_move = KiInt2(GiCadWindows.size.x / 2, GiCadWindows.size.y / 2);
*/
}

// Cabblack on drop files
void WndDrop(GLFWwindow* window, int count, const char** paths){
    for (int i = 0; i < count; i++){
		MglWindows.InsertPopUp(LString() + "File dropped: " + paths[i] + ".");
	}
}