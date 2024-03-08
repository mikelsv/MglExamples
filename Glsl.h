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

class GlslObjectsHead{
public:
	int pos, size;
	GLuint type; // GL_LINES or GL_POLYGON
	MRGB color;
};

class GlslObjectsData{
public:
	float x, y, z;	
};

class GlslObjectsColor{
public:
	KiVec3 color;
	
	void SetColor(MRGB rgb){
		color.x = rgb.red / 255.;
		color.y = rgb.green / 255.;
		color.z = rgb.blue / 255.;
	}

};

class GlslObjectsBuffer{
	MString head, data, color;
	int head_count, data_count;
	int update;

public:
	void Reserve(int hcount, int dcount){
		if(head.size() < sizeof(GlslObjectsHead) * hcount)
			head.Reserve(sizeof(GlslObjectsHead) * hcount);

		if(data.size() < sizeof(GlslObjectsData) * dcount)
			data.Reserve(sizeof(GlslObjectsData) * dcount);

		if(color.size() < sizeof(GlslObjectsColor) * dcount)
			color.Reserve(sizeof(GlslObjectsColor) * dcount);

		head_count = hcount;
		data_count = dcount;
		update = 1;

		return ;
	}

	GlslObjectsHead* GetHead(){
		return (GlslObjectsHead*) head.data;
	}

	GlslObjectsData* GetData(){
		return (GlslObjectsData*) data.data;
	}

	GlslObjectsColor* GetColors(){
		return (GlslObjectsColor*) color.data;
	}

	int GetHeadCount(){
		return head_count;
	}

	int GetDataCount(){
		return data_count;
	}

	bool GetUpdate(){
		return update;
	}

	void Updated(){
		update = 0;
	}

} GlslObjectsBuffer;

class GlslObjects : public MglSimpleGlsl{
	// Buffers
	GLuint vertex_array, vertex_pos, vertex_color;
	
	GLuint vertex_id, color_id;

	// Uniform
	GLuint uni_iMove, uni_iRes, uni_iZoom;

	// Render texture.
	// vec4 from, vec4 to;

struct vertex{
 GLfloat x;
 GLfloat y;
};

public:
	int Init(){
		if(!Load(LoadFile("glsl/objects.vs"), LoadFile("glsl/objects.fs")))
			return 0;

		color_id = GetUniformLocation("textColor");	
		uni_iMove = GetUniformLocation("iMove");
		uni_iRes = GetUniformLocation("iResolution");
		uni_iZoom = GetUniformLocation("iZoom");

		InitVbo();

		return 1;
	}

	int InitVbo(){
		// Array
		glGenVertexArrays(1, &vertex_array);
		glBindVertexArray(vertex_array);

		// Position
		glGenBuffers(1, &vertex_pos);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_pos);
		glEnableVertexAttribArray(0);
		
		// Colors
		glGenBuffers(1, &vertex_color);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_color);		
		
		//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		
		//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		return 1;
	}

	void UpdateMove(KiVec2 p){
		UseProgram();
		glUniform2f(uni_iMove, p.x, p.y);
		glUseProgram(0);
	}

	void UpdateRes(KiVec2 res){
		UseProgram();
		glUniform2f(uni_iRes, res.x, res.y);
		glUseProgram(0);
	}

	void UpdateZoom(float zoom){
		UseProgram();
		glUniform1f(uni_iZoom, zoom);
		glUseProgram(0);
	}

	void Render(){
	   // activate corresponding render state	
		UseProgram();
		//glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(vertex_array);

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

		static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
   .5, .5, 0.
};

		// render glyph texture over quad
		//glBindTexture(GL_TEXTURE_2D, tex_id);
		// update content of VBO memory
		
		
		if(GlslObjectsBuffer.GetUpdate()){
			// Positions buffer
			glBindBuffer(GL_ARRAY_BUFFER, vertex_pos);
			glBufferData(GL_ARRAY_BUFFER, GlslObjectsBuffer.GetDataCount() * sizeof(GlslObjectsData), GlslObjectsBuffer.GetData(), GL_STATIC_DRAW);
			

		//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(
			   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			   3,                  // size
			   GL_FLOAT,           // type
			   GL_FALSE,           // normalized?
			   0,                  // stride
			   (void*)0            // array buffer offset
			);			

			// Color buffer
			glBindBuffer(GL_ARRAY_BUFFER, vertex_color);
			glBufferData(GL_ARRAY_BUFFER, GlslObjectsBuffer.GetDataCount() * sizeof(GlslObjectsColor), GlslObjectsBuffer.GetColors(), GL_STATIC_DRAW);

			glVertexAttribPointer(
			   1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			   3,                  // size
			   GL_FLOAT,           // type
			   GL_FALSE,           // normalized?
			   0,                  // stride
			   (void*)0            // array buffer offset
			);
			glEnableVertexAttribArray(1);

			// Ok
			GlslObjectsBuffer.Updated();

			// Unbind
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		
		
		// Render
		auto head = GlslObjectsBuffer.GetHead(), head_end = head + GlslObjectsBuffer.GetHeadCount();
		while(head < head_end){
			glDrawArrays(head->type, head->pos, head->size);
			head ++;
		}

		// Draw all
		//glDrawArrays(GL_LINE_LOOP, 0, GlslObjectsBuffer.GetDataCount());
				
		glBindVertexArray(0);		
		glUseProgram(0);
	}	
};

class GlslLayerCircle{
public:
	double x, y, dia;
};

class GlslLayerPathEl{
public:
	double x, y;

};

class GlslLayerPath{
public:
	OList<GlslLayerPathEl> path;

	void AddPoint(double x, double y){
		GlslLayerPathEl *el = path.NewE();
		el->x = x;
		el->y = y;
	}
};

class GlslLayer{
	// Head
	int layer_id;
	MString name;
	MRGB color;

	// Data
	OList<GlslLayerCircle> cls;
	OList<GlslLayerPath> paths;

	GlslLayerPath *last_path;

public:
	GlslLayer(){
		last_path = 0;
		color.set(rand(), rand(), rand());
	}

	// Get
	int GetId(){
		return layer_id;
	}

	MRGB GetColor(){
		return color;
	}

	// Set
	void SetId(int id){
		layer_id = id;
	}

	// Add
	void AddCircle(double x, double y, double dia){
		GlslLayerCircle *el = cls.NewE();
		el->x = x;
		el->y = y;
		el->dia = dia;
	}

	void AddPath(){
		last_path = paths.NewE();
	}

	void AddPoi(double x, double y){
		if(!last_path)
			last_path = paths.NewE();

		last_path->AddPoint(x, y);
	}

	MRGB RandomColor(){
		return MRGB(rand(), rand(), rand());
	}

	// Draw
	void Draw(GlslObjects &objects, int cirpoi_size = 12){
		int cir_count = 0, path_count = 0, path_size = 0, size = 0;

		// Count
		cir_count += cls.Size();
			
		GlslLayerPath *pel = 0;
		while(pel = paths.Next(pel)){
			path_count ++;
			path_size += pel->path.Size();
		}
				

		GlslObjectsBuffer.Reserve(cir_count + path_count, cir_count * cirpoi_size + path_size);

		GlslObjectsHead *head = GlslObjectsBuffer.GetHead();
		GlslObjectsData *data = GlslObjectsBuffer.GetData();
		GlslObjectsColor *color = GlslObjectsBuffer.GetColors();

		// Make circles
		GlslLayerCircle *cel = 0;

		// Circles
		while(cel = cls.Next(cel)){
			head->pos = size;
			head->type = GL_POLYGON;
			head->size = cirpoi_size;

			for(int i = 0; i < cirpoi_size; i++){
				float angle = i * 2 * PI / cirpoi_size;
				data->x = cel->x + (cos(angle) * cel->dia / 2);
				data->y = cel->y + (sin(angle) * cel->dia / 2);
				data->z = 0;
				data ++;

				color->SetColor(RandomColor());
				color ++;
			}

			head ++;
			size += cirpoi_size;
		}
			

		// Make paths
		pel = 0;
		GlslLayerPathEl *pel2 = 0;

		while(pel = paths.Next(pel)){
			head->pos = size;
			head->size = pel->path.Size();
			head->type = GL_LINE_LOOP;
			head ++;
				
			while(pel2 = pel->path.Next(pel2)){
				data->x = pel2->x;
				data->y = pel2->y;

				data ++;
				size ++;

				color->SetColor(RandomColor());
				color ++;
			}		
		}

		return ;
	}

	// ~
	void Clean(){
		last_path = 0;

		cls.Clean();
	}

};