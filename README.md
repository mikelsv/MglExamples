# MglExamples
Hello. This is OpenGL glsl examples.

## Setup
```
git clone https://github.com/mikelsv/MglExamples.git
git clone https://github.com/mikelsv/msvcore2.git

MSVS 2012 - 2022:
Open MglExamples/MglExamples.vcxproj

Copy glfw3.dll to Debug | Release.
Or run glfw_setup.bat

GCC:
cd ./MglExamples
make
```

## Examples
Open MglExamples.cpp. Find defines section and configure it. (Uncomment for use)
```
// Select examples here <<<-----------------------------------------
#define MGL_EXAMPLE_TRIANGLE // Show triangle
#define MGL_EXAMPLE_TEXTURE // Show texture
#define MGL_EXAMPLE_SHADER // Show shader like shadertoy: glsl/shader.fs
#define MGL_EXAMPLE_MENU // Show shader menu on Right click
#define MGL_EXAMPLE_POPUP // Show popup messages
```

### MGL_EXAMPLE_TRIANGLE
Draw triangle. Based on MglSimpleGlslTriangle in https://github.com/mikelsv/msvcore2/blob/master/opengl/mgl_shader_simple.h

### MGL_EXAMPLE_TEXTURE
Draw texture. By default load glsl/font.bin (binary version on font.png). Based on MglGlslTexture in https://github.com/mikelsv/msvcore2/blob/master/opengl/mgl_shader_simple.h

### MGL_EXAMPLE_SHADER
Draw shader: glsl/shader.vs + glsl/shader.fs. Like as shadertoy.com. Based on GlslShader in Glsl.h.

### MGL_EXAMPLE_MENU
Draw menu on Right click. Shaders: glsl/menu.vs + glsl/menu.fs. Based on MglMenu in https://github.com/mikelsv/msvcore2/blob/master/opengl/mgl_shader_gui.h

### MGL_EXAMPLE_POPUP
Draw popup message. On start program. Or on Drag & Drop file to window. Shaders: glsl/popup.vs & glsl/popup.fs. Based on MglPopUp in https://github.com/mikelsv/msvcore2/blob/master/opengl/mgl_shader_gui.h