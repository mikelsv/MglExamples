// Font glsl/font.bin - 1024 x 1024 px
class GlslFontTexture{
	GLuint texture;

public:
	void Init(){
		MString data = LoadFile("glsl/font.bin");

		if(!data)
			return ;

		GLuint t = 0;
		//glEnable(GL_TEXTURE_2D);
		glGenTextures( 1, &t );
		glBindTexture(GL_TEXTURE_2D, t);

		// Set parameters to determine how the texture is resized
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		// Set parameters to determine how the texture wraps at edges
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T, GL_REPEAT );
		// Read the texture data from file and upload it to the GPU
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data.data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		texture = t;
	}

	GLuint GetTexture(){
		return texture;
	}

} GlslFontTexture;


class GlslShader : public MglSimpleGlsl{
	GLuint vbo, vao, vertex_id, color_id;

	GLint uni_iTime, uni_iRes, uni_iMove, uni_iZoom, uni_iMouse, uni_iFps, unu_iChan0, uni_GiCircles, uni_GiCirclesCount;
	GLuint font_id;

public:
	bool Init(){
		if(!Load(LoadFile("glsl/shader.vs"), LoadFile("glsl/shader.fs")))
			return 0;

		InitVbo();

		// Uniforms
		uni_iTime = GetUniformLocation("iTime");
		uni_iRes = GetUniformLocation("iResolution");
		uni_iMove = GetUniformLocation("iMove");
		uni_iZoom = GetUniformLocation("iZoom");
		uni_iMouse = GetUniformLocation("iMouse");
		uni_iFps = GetUniformLocation("iFps");

		UpdateZoom(1);

		return 1;
	}

	int InitVbo(){
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		return 1;
	}

	void UpdateRes(KiVec2 res){
		UseProgram();
		glUniform2f(uni_iRes, res.x, res.y);
		glUseProgram(0);
	}

	void SetFont(GLuint f){
		font_id = f;
	}

	void UpdateMove(KiVec2 p){
		UseProgram();
		glUniform2f(uni_iMove, p.x, p.y);
		glUseProgram(0);
	}

	void UpdateZoom(float zoom){
		UseProgram();
		glUniform1f(uni_iZoom, zoom);
		glUseProgram(0);
	}

	void UpdateMouseSelect(KiVec2 cur, KiVec2 hold, bool down){
		UseProgram();
		
		if(down)
			glUniform4f(uni_iMouse, cur.x, cur.y, hold.x, hold.y);
		else
			glUniform4f(uni_iMouse, cur.x, cur.y, 0, 0);
		
		glUseProgram(0);
	}

	void UpdateFps(int fps){
		UseProgram();
		glUniform1i(uni_iFps, fps);
		glUseProgram(0);
	}

	void Render(float iTime){
		//GLint tex_id = 0;

	   // activate corresponding render state	
		UseProgram();
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(vao);

		glUniform1fv(uni_iTime, 1, &iTime);

		float xpos = -1, ypos = -1;
		float w = 2, h = 2;
		
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, font_id);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}
};