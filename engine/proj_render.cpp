#include "stdafx.h"
#include "proj_render.h"

#include "Vec3f.hxx"
//#define GLM_FORCE_RADIANS <-- strange!
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>

#include <iostream> // <-- fbo to pgm|ppm
#include <fstream> // <-- fbo to pgm|ppm
#include <string> // <-- fbo to pgm|ppm

// ---> GL_TRIANGLES
// Vertexarrays (Hauptspeicher)
// OpenGL1.1 - Displaylist (VRam d. Grafikkarte, statisch)
//      2003 - VBO (VRam + einfaches dynamisches Ändern d. Daten)

// VBO tutorial: http://www.ozone3d.net/tutorials/opengl_vbo.php
// using multiple vs. using 1 large VBO: https://www.opengl.org/discussion_boards/showthread.php/176365-Working-with-multiple-VBO-s

proj::Render::Render()
{
    hRC=NULL;                           // Permanent Rendering Context
    hDC=NULL;                           // Private GDI Device Context
//    hWnd=NULL;                          // Holds Our Window Handle

    for (int iVAO=0; iVAO<VBOCOUNT; iVAO++) vCount[iVAO] = 0;
    posAttrib = 0;

    fBgColR = 1.0f;
    fBgColG = 0.5f;
    fBgColB = 1.0f; // <-- optional (yellow background)

    bits_color = 24;
    bits_zbuffer = 16;

    b_Minimal = false;
    b_PNG = false;

    iWireframe = 0;

    Init_VAOs();
    Init_Textures();

    b_Guardrail = false;
    b_Curbstones = true;
}

int proj::Render::Init()
{
// only f. fixed pipeline --> glEnable(GL_TEXTURE_2D);
    glClearColor(0.3f,0.5f,1.0f,0.0f); // <-- optional (blue background)
    glClearDepth(1.0f);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST); // <-- !
// schneidet "zu viel" weg -->    glEnable(GL_CULL_FACE);
	
	return 0;
}

GLvoid proj::Render::ReSizeGLScene(GLsizei width, GLsizei height) // Resize And Initialize The GL Window
{
    if (height==0)                     // Prevent A Divide By Zero By
    {
        height = 1;                    // Making Height Equal One
    }
    glViewport(0, 0, width, height);   // Reset The Current Viewport
}

HDC proj::Render::GL_attach_to_DC(HWND hWnd)
{ 
    GLuint PixelFormat;                     // Holds The Results After Searching For A Match
    int bits = 8;

#define ERR_HDC           0
#define ERR_PIXELFORMAT   0
#define ERR_CONTEXT       0
#define ERR_GLMAKECURRENT 0

    static PIXELFORMATDESCRIPTOR pfd=       // pfd Tells Windows How We Want Things To Be
    {
        sizeof(PIXELFORMATDESCRIPTOR),      // Size Of This Pixel Format Descriptor
        1,                                  // Version Number
        PFD_DRAW_TO_WINDOW |                // Format Must Support Window
        PFD_SUPPORT_OPENGL |                // Format Must Support OpenGL
        PFD_DOUBLEBUFFER,                   // Must Support Double Buffering
        PFD_TYPE_RGBA,                      // Request An RGBA Format
        bits,                               // Select Our Color Depth
        0, 0, 0, 0, 0, 0,                   // Color Bits Ignored
        0,                                  // No Alpha Buffer
        0,                                  // Shift Bit Ignored
        0,                                  // No Accumulation Buffer
        0, 0, 0, 0,                         // Accumulation Bits Ignored
        16,                                 // 16Bit Z-Buffer (Depth Buffer)
        0,                                  // No Stencil Buffer
        0,                                  // No Auxiliary Buffer
        PFD_MAIN_PLANE,                     // Main Drawing Layer
        0,                                  // Reserved
        0, 0, 0                             // Layer Masks Ignored
    };
    if (!(hDC=GetDC(hWnd)))                 // Did We Get A Device Context?
    {
        MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return ERR_HDC;
    }
    if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd))) // Did Windows Find A Matching Pixel Format?
    {
        MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return ERR_PIXELFORMAT;
    }
    if(!SetPixelFormat(hDC,PixelFormat,&pfd)) // Are We Able To Set The Pixel Format?
    {
        char str[255];
        DWORD err = GetLastError();
        sprintf(str, "Can't Set The PixelFormat, err=%d", err);
        MessageBox(NULL,str,"ERROR",MB_OK|MB_ICONEXCLAMATION);

// --> on NVidia GTX 780 error : -1073283066 = 0xc0070006
        return ERR_PIXELFORMAT;
    }

/*  ... when running in MTS-Thread --> s. http://www.graphicsgroups.com/6-opengl/0bafc0120a809ed8.htm

    According to the Windows OpenGL documentation:
    "Setting the pixel format of a window more than once can lead to significant 
        complications for the Window Manager and for multithread applications, so it 
        is not allowed. An application can only set the pixel format of a window one 
        time. Once a window's pixel format is set, it cannot be changed." 
        The card drivers that allow you do change the format are not following the 
        rules; the ones (like GeForce3) that are giving you problems are giving you 
        problems because you're violating the rules. 
        As has been suggested elsewhere, create for yourself an invisible and/or 
        off-screen dummy window, and use the rendering context you create for that 
        window as your display-list share. You can either create it from a 
        registered class with CS_OWNDC style, or you can just create a compatible DC 
        and then not delete that DC until the program is done. Either way, this will 
        be low-overhead solution, plus it should work on all compliant video 
        drivers.
*/

    if (!(hRC=wglCreateContext(hDC))) // Are We Able To Get A Rendering Context?
    {
        MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return ERR_CONTEXT;
    }
    if(!wglMakeCurrent(hDC,hRC)) // Try To Activate The Rendering Context
    {
        MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return ERR_GLMAKECURRENT;
    }
   
    ReSizeGLScene(width, height); // Set Up Our Perspective GL Screen
    return hDC;
}

void CHECK_FRAMEBUFFER_STATUS()
{                                                         
    GLenum status;
    status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER); 
    switch(status)
    {
        case GL_FRAMEBUFFER_COMPLETE:
            break;

        case GL_FRAMEBUFFER_UNSUPPORTED: // choose different formats
            break;

        default: // programming error; will fail on all hardware
    //        fputs("Framebuffer Error\n", stderr);
            exit(-1);
    }
}

void proj::Render::init_FBO()
{
    glGenFramebuffers(1, &fb);
    glGenTextures(1, &color);
    glGenRenderbuffers(1, &depth);

    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glBindTexture(GL_TEXTURE_2D, color);
    glTexImage2D(GL_TEXTURE_2D, 
            0, 
            GL_RGBA, 
            fbo_width, fbo_height,
            0, 
            GL_RGBA,
            GL_UNSIGNED_BYTE, 
            NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fbo_width, fbo_height);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

    CHECK_FRAMEBUFFER_STATUS();
}

// s. http://open.gl/drawing
void proj::Render::InitShaders()
{
    char buffer[512];
    // "GLSL" starts -->

//#define COLORIZE
//#define TEXTURIZE
#define COLS_N_TEXTURES
#ifdef COLORIZE
    const GLchar *vertexShaderSource[] = { // vec4 (x,y,z,w)
        "#version 150\n"
        "uniform mat4 MVPMatrix;\n"
        "in vec3 position;\n"
        "uniform vec3 offset;\n" // <-- for object-movement = uniform!!
        "in vec3 color;\n"
        "out vec3 fragColor;\n"
        "void main()\n"
        "{\n"
        "    fragColor = color;\n"
        "    vec4 totaloffset = vec4(offset.x, offset.y, offset.z, 0.0);\n"
        "    gl_Position = MVPMatrix * (vec4(position, 1.0) + totaloffset);\n" // totaloffset <-- fragementshader for movement! 
        "}"
    };
// der Vertexshader gibt die fragColor an den Fragment-Shader weiter!!
    const GLchar *fragmentShaderSource[] = {
        "#version 150\n"
        "in vec3 fragColor;\n"
        "out vec4 outColor;\n"
        "void main()\n"
        "{\n"
        "    outColor = vec4(fragColor, 1.0);\n"
        "}"
    };
#endif
#ifdef TEXTURIZE
    const GLchar *vertexShaderSource[] = {
        "#version 330 core\n"
        "uniform mat4 MVPMatrix;\n"
        "in vec3 position;\n"
        "uniform vec3 offset;\n" // <-- for object-movement = uniform!!
        "in vec2 vertexUV;\n"
        "out vec2 UV;\n"
        "void main()\n"
        "{\n"
//        "    gl_Position = MVPMatrix * vec4(position, 1.0);\n"
        "    vec4 totaloffset = vec4(offset.x, offset.y, offset.z, 0.0);\n"
        "    gl_Position = MVPMatrix * (vec4(position, 1.0) + totaloffset);\n" // totaloffset <-- fragementshader for movement! 
        "    UV = vertexUV;\n"
        "}"
    };
// der Vertexshader gibt die fragColor an den Fragment-Shader weiter!!
    const GLchar *fragmentShaderSource[] = {
        "#version 330 core\n"
        "in vec2 UV;\n"
        "uniform sampler2D myTexSampler;\n" // <-- hier hatte ich das Semikolon vergessen
        "out vec3 outColor;\n"              // <-- hier hatte ich zunächst .rgb einem vec4 zugeordnet (.rgba -> vec4)
        "void main()\n"
        "{\n"
        "    outColor = texture(myTexSampler, UV).rgb;\n" // texture2D ist deprecated
        "}"
    };
#endif
#ifdef COLS_N_TEXTURES
    const GLchar *vertexShaderSource[] = {
        "#version 330 core\n"
        "uniform mat4 MVPMatrix;\n"
        "in vec3 position;\n"
        "uniform vec3 offset;\n"  // <-- for object-movement = uniform!!
        "in vec2 vertexUV;\n"
        "out vec2 UV;\n"
        "in vec3 color;\n"
        "out vec3 fragColor;\n"
        "void main()\n"
        "{\n"
        "    fragColor = color;\n"
//        "    gl_Position = MVPMatrix * vec4(position, 1.0);\n"
        "    vec4 totaloffset = vec4(offset.x, offset.y, offset.z, 0.0);\n"
        "    gl_Position = MVPMatrix * (vec4(position, 1.0) + totaloffset);\n" // totaloffset <-- fragementshader for movement! 
        "    UV = vertexUV;\n"
        "}"
    };
// der Vertexshader gibt die fragColor an den Fragment-Shader weiter!!
    const GLchar *fragmentShaderSource[] = {
        "#version 330 core\n"
        "in vec2 UV;\n"
        "uniform sampler2D myTexSampler;\n" // <-- hier hatte ich das Semikolon vergessen
        "uniform int col_tex;\n" // 0 = color, 1 = texture
        "in vec3 fragColor;\n"
        "out vec3 outColor;\n"              // <-- texture
//        "out vec4 outColor;\n"              // <-- color
        "void main()\n"
        "{\n"
		"    if (col_tex==0)\n"
        "        outColor = vec3(fragColor);\n"
		"    else\n"
        "        outColor = texture(myTexSampler, UV).rgb;\n" // texture2D ist deprecated
        "}"
    };
#endif
// s. auch: http://wiki.lwjgl.org/index.php?title=GLSL_Tutorial:_Communicating_with_Shaders

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, vertexShaderSource, 0);
    glCompileShader(vertexShader);
    glGetShaderInfoLog(vertexShader, 512, NULL, buffer); // <-- debug, kann man sich schoen im debugger ansehen!!

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, fragmentShaderSource, 0); // set array of strings as source code
    glCompileShader(fragmentShader); // compile
    glGetShaderInfoLog(fragmentShader, 512, NULL, buffer); // <-- debug

    program = glCreateProgram(); // create empty program object
    glAttachShader(program, vertexShader); // attach shader
    glAttachShader(program, fragmentShader); // attach shader
    glLinkProgram(program); // link
    glUseProgram(program); // install ... and use in the further runtime ...

    // attribs (fix)
    posAttrib       = glGetAttribLocation(program, "position");
    colAttrib       = glGetAttribLocation(program, "color");
    texAttrib       = glGetAttribLocation(program, "vertexUV");
    // uniforms
    offsetAttrib    = glGetUniformLocation(program, "offset");       // object movement
    MVPMatrixAttrib = glGetUniformLocation(program, "MVPMatrix");    // camera movement
    SamplerAttrib   = glGetUniformLocation(program, "myTexSampler");
	col_texAttrib   = glGetUniformLocation(program, "col_tex");

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 View       = glm::lookAt(
                                glm::vec3(0,0,1.33), // Camera is at (4,3,-3), in World Space
                                glm::vec3(1,0,1.33), // and looks at the origin
                                glm::vec3(0,0,1)     // Head is up (set to 0,-1,0 to look upside-down)
                           );
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model      = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVPMatrix  = Projection * View * Model; // Remember, matrix multiplication is the other way around
    glUniformMatrix4fv(MVPMatrixAttrib, 1, GL_FALSE, &MVPMatrix[0][0]); // load matrix into shader

    glActiveTexture(GL_TEXTURE0);
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glBindTexture(GL_TEXTURE_2D, Texture[1]);
    glUniform1i(SamplerAttrib, 0);
}

void proj::Render::Init_VAOs()
{
    // s. proj.LoadVAOs() ...
/*
    for (int iVAO = 0; iVAO < VBOCOUNT; iVAO++)
    {
        aVAOs[iVAO].t_Shade = SHADER_COLOR_FLAT;
    }
*/
    aVAOs[VBO_2TRIANGLES].t_Shade   = SHADER_COLOR_FLAT;
    aVAOs[VBO_LEFT].t_Shade         = SHADER_COLOR_FLAT;
    aVAOs[VBO_RIGHT].t_Shade        = SHADER_COLOR_FLAT;
    aVAOs[VBO_ROAD].t_Shade         = SHADER_TEXTURE;
    aVAOs[VBO_TRAFFICSIGNS].t_Shade = SHADER_COLOR_FLAT;
    aVAOs[VBO_GUARDRAIL].t_Shade    = SHADER_COLOR_FLAT;
    aVAOs[VBO_CURBSTONES].t_Shade   = SHADER_COLOR_FLAT;
    aVAOs[VBO_MOVING1].t_Shade      = SHADER_TEXTURE;
    aVAOs[VBO_MOVING2].t_Shade      = SHADER_TEXTURE;
}

void proj::Render::Init_Textures()
{
    // s. proj.Init() ...
    aTextures[0].fXWorld = 5.0f; // [m]
    aTextures[0].fYWorld = 5.0f; // [m]
}

// Initialize / Bind Vertex Array Objects
void proj::Render::Bind_VAOs() // s. http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html
{
    glGenVertexArrays((VBOCOUNT-1), &vertexArray[0]);
    for (int iVAO = 0; iVAO < VBOCOUNT; iVAO++)
    {
        glBindVertexArray(vertexArray[iVAO]); // Bind
    
        glEnableVertexAttribArray(posAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[iVAO]);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // wichtig, hier das richtige Attrib (nicht 0 oder 1) zu übergeben!

        if (aVAOs[iVAO].t_Shade == SHADER_COLOR_FLAT) // flat (number of elements per Vertex = 3)
        {    
            glEnableVertexAttribArray(colAttrib); // s. http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_03
            glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[iVAO]);
            glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        }
        else if (aVAOs[iVAO].t_Shade == SHADER_TEXTURE) // texture (number of elements per Vertex = 2)
        {
            glEnableVertexAttribArray(texAttrib);
			// HACK!!!!!!
//			if (iVAO == VBO_MOVING2)
//				glBindBuffer(GL_ARRAY_BUFFER, m_Moving[1]->uvs); // set appropriate Texture-ID!!! <-- 2do!!!!!!!! saublöd, deswegen habe ich wochenlang
//			else 
//				glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[1]); // set appropriate Texture-ID!!! <-- 2do!!!!!!!! saublöd, deswegen habe ich wochenlang
                                                                                                          // das Importblender debuggt!!
			glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[iVAO]); // set appropriate Texture-ID!!! <-- 2do
			glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		}
        glBindVertexArray(0); // Unbind
    }
}

// most simple VBO/VAO, just in case errors happen...
int proj::Render::CreateVBO_2Triangles()
{
    #define COORDS_PER_VERTEX 3
    #define VERTICES_PER_TRI 3
    #define TRIANGLES 2
    #define TRI_COORDS TRIANGLES*VERTICES_PER_TRI*COORDS_PER_VERTEX // 18 = 2 Triangles * 3 Vertices * 3 Coordinates

    vCount[VBO_2TRIANGLES] = TRIANGLES*VERTICES_PER_TRI; // Vertices
    GLfloat Coords[TRI_COORDS];
    // Setup triangle vertices
    int i=0;
    Coords[i++] = 0.6f; Coords[i++] = 0.1f; Coords[i++] = 0.0f; 
    Coords[i++] = 0.9f; Coords[i++] = 0.5f; Coords[i++] = 0.0f; 
    Coords[i++] = 0.0f; Coords[i++] = 0.7f; Coords[i++] = 0.0f; 

    Coords[i++] = -0.4f; Coords[i++] = 0.1f; Coords[i++] = 0.0f; 
    Coords[i++] =  0.4f; Coords[i++] = 0.1f; Coords[i++] = 0.0f; 
    Coords[i++] =  0.0f; Coords[i++] = 0.7f; Coords[i++] = 0.0f; 

    glGenBuffers(1, &positionBuffer[VBO_2TRIANGLES]); // a) position-buffer
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[VBO_2TRIANGLES]);
    glBufferData(GL_ARRAY_BUFFER, TRI_COORDS*sizeof(GLfloat), Coords, GL_STATIC_DRAW); // init data storage

    GLfloat colors[TRI_COORDS]; // (r,g,b) <-- (x,y,z) 
    i=0;
    colors[i++] = 0.0f; colors[i++] = 1.0f; colors[i++] = 0.0f; 
    colors[i++] = 0.0f; colors[i++] = 1.0f; colors[i++] = 0.0f; 
    colors[i++] = 0.0f; colors[i++] = 1.0f; colors[i++] = 0.0f; 

    colors[i++] = 0.0f; colors[i++] = 0.0f; colors[i++] = 1.0f; 
    colors[i++] = 0.0f; colors[i++] = 0.0f; colors[i++] = 1.0f; 
    colors[i++] = 0.0f; colors[i++] = 0.0f; colors[i++] = 1.0f; 

    glGenBuffers(1, &colorBuffer[VBO_2TRIANGLES]); // b) color-buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[VBO_2TRIANGLES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount[VBO_2TRIANGLES]*3, colors, GL_STATIC_DRAW);

    #define UV_COORDS_PER_VERTEX 2
    #define TRI_UV_COORDS VERTICES_PER_TRI*COORDS_PER_VERTEX*UV_COORDS_PER_VERTEX // 12 = 2 Triangles * 3 Vertices * 2 uv-Coords

    GLfloat texCoords[TRI_UV_COORDS];
    i=0;
    texCoords[i++] = 0.0f; texCoords[i++] = 0.0f;
    texCoords[i++] = 0.0f; texCoords[i++] = 1.0f;
    texCoords[i++] = 1.0f; texCoords[i++] = 0.0f;

    texCoords[i++] = 0.0f; texCoords[i++] = 0.0f;
    texCoords[i++] = 0.0f; texCoords[i++] = 1.0f;
    texCoords[i++] = 1.0f; texCoords[i++] = 0.0f; 

    glGenBuffers(1, &uvBuffer[VBO_2TRIANGLES]); // c) texture-buffer
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[VBO_2TRIANGLES]);
    glBufferData(GL_ARRAY_BUFFER, TRI_UV_COORDS*sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

    return 0;
}

int proj::Render::CreateVBO_Scene()
{
    SceneParam &rc_Param = m_Scene->m_SceneLoader;

    unsigned int iLine;
    unsigned int iMarker;
    unsigned int sz;

    S_Point3D p0,p1,p2,p3;

    vCount[1] = 0; // VBO_LEFT
    vCount[2] = 0; // VBO_RIGHT
    vCount[3] = 0; // VBO_ROAD
    sz = (unsigned int)rc_Param.m_c_Markers.size(); // number of marker vectors (lines)
    for (iLine=0;iLine<sz;iLine++)
    {
        const std::vector<S_MarkerPoint> &rc_Marker = rc_Param.m_c_Markers[iLine];
        for (iMarker=0;iMarker<rc_Marker.size()-1;iMarker++) // no. of markersteps (typically > 500)
        {
            if (rc_Marker[iMarker].b_Visible) vCount[1+iLine]+=6; // 2 triangles per Quad
        }

        GLfloat* vertices  = new GLfloat[vCount[1+iLine]*3]; // Vertex.x/y/z
        GLfloat* colors    = new GLfloat[vCount[1+iLine]*3]; // Color.r/g/b
        GLfloat* texCoords = new GLfloat[vCount[1+iLine]*2]; // Texture.U/V

        int iV = 0;
        int iTx = 0;

        //      [0 ..  255] <-- 2do
        //      [0 .. 4095]
        // -> [0.0 ..  1.0]
        float f_R = rc_Param.m_c_Colors[iLine].u_Red   / 4095.0f;
        float f_G = rc_Param.m_c_Colors[iLine].u_Green / 4095.0f;
        float f_B = rc_Param.m_c_Colors[iLine].u_Blue  / 4095.0f;

        int textureID = rc_Param.m_TextureIDs[iLine];
        float fTexStrip = 0.0f; // store strip of texture, if not full texture is mapped to triangle
        float fTexIncr = 0.1f; // 2do <-- aus der "Höhe" des triangles berechnen!

        for (iMarker=0;iMarker<rc_Marker.size()-1;iMarker++) // no. of markersteps (typically > 500)
        {
            if (rc_Marker[iMarker].b_Visible)
            {
                /*  p3 +--+ p2     two triangles from 1 quad
                       | /|
                       |/ |
                    p0 +--+ p1
                */
                p0 = rc_Marker[iMarker].s_Left;
                p1 = rc_Marker[iMarker].s_Right;
                p2 = rc_Marker[iMarker+1].s_Right;
                p3 = rc_Marker[iMarker+1].s_Left;
// Farben funktionieren nicht, ich denke, man muss die Shader (vert+frag) einbauen
                // 1st Triangle
                vertices[iV] = p0.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 0.0f;  // U
                vertices[iV] = p0.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 0.0f;  // V
                vertices[iV] = p0.rl_Z;   colors[iV++] = f_B;
                vertices[iV] = p1.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 0.0f;
                vertices[iV] = p1.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 1.0f;
                vertices[iV] = p1.rl_Z;   colors[iV++] = f_B;
                vertices[iV] = p2.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 1.0f;
                vertices[iV] = p2.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 0.0f;
                vertices[iV] = p2.rl_Z;   colors[iV++] = f_B;
                /*
                  ^                Texture coordinates
                  |
                U |
                  +----->
                     V
                */
                texCoords[iTx++] = fTexStrip + 0.0f;     // p0 - U
                texCoords[iTx++] = 0.0f;                 // p0 - V
                texCoords[iTx++] = fTexStrip + 0.0f;     // p1 - U
                texCoords[iTx++] = 1.0f;                 // p1 - V
                texCoords[iTx++] = fTexStrip + fTexIncr; // p2 - U
                texCoords[iTx++] = 1.0f;                 // p2 - V

                // 2nd Triangle
                vertices[iV] = p2.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 0.0f;
                vertices[iV] = p2.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 0.0f;
                vertices[iV] = p2.rl_Z;   colors[iV++] = f_B;
                vertices[iV] = p3.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 0.0f;
                vertices[iV] = p3.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 1.0f;
                vertices[iV] = p3.rl_Z;   colors[iV++] = f_B;
                vertices[iV] = p0.rl_X;   colors[iV++] = f_R; //  texCoords[iTx++] = 1.0f;
                vertices[iV] = p0.rl_Y;   colors[iV++] = f_G; //  texCoords[iTx++] = 0.0f;
                vertices[iV] = p0.rl_Z;   colors[iV++] = f_B;

                texCoords[iTx++] = fTexStrip + fTexIncr; // p2 - U
                texCoords[iTx++] = 1.0f;                 // p2 - V
                texCoords[iTx++] = fTexStrip + fTexIncr; // p3 - U
                texCoords[iTx++] = 0.0f;                 // p3 - V
                texCoords[iTx++] = fTexStrip + 0.0f;     // p0 - U
                texCoords[iTx++] = 0.0f;                 // p0 - V
                fTexStrip += fTexIncr;
            }
        }

        // VBO, s. http://ogldev.atspace.co.uk/www/tutorial02/tutorial02.html
        glGenBuffers(1, &positionBuffer[1+iLine]);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer[1+iLine]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount[1+iLine]*3, vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &colorBuffer[1+iLine]);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer[1+iLine]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount[1+iLine]*3, colors, GL_STATIC_DRAW);

        glGenBuffers(1, &uvBuffer[1+iLine]);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer[1+iLine]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vCount[1+iLine]*2, texCoords, GL_STATIC_DRAW);

        delete [] colors;
        delete [] vertices;
        delete [] texCoords;
    }

    return 0;
}

// int proj::Render::CreateVBO_Scene_w_o_Texture_Wrap()

/* === Move objects ===
    http://www.arcsynthesis.org/gltut/positioning/Tut03%20A%20Better%20Way.html  <-- 2015-12-12, fkt. leider nicht mehr
*/

int proj::Render::DestroyScene_VBO()
{
    glDeleteVertexArrays(2, &vertexArray[0]);
    glDeleteBuffers(2, &positionBuffer[0]);
    glDeleteBuffers(2, &colorBuffer[0]);
//    glDeleteBuffers(2, &uvBuffer[0]);
    return 0;
}

void proj::Render::get_xyz_Hack(int iT, GLfloat &x, GLfloat &y, GLfloat &z, GLfloat &xto, GLfloat &yto, GLfloat &zto)
{
    SceneParam &rc_Param = m_Scene->m_SceneLoader;
    x = rc_Param.m_c_Trajectory[iT].s_Pos.rl_X;  // 2do Vec3f ...
    y = rc_Param.m_c_Trajectory[iT].s_Pos.rl_Y;
    z = rc_Param.m_c_Trajectory[iT].s_Pos.rl_Z + 1.34f;
    float xlook = rc_Param.m_c_Trajectory[iT+1].s_Pos.rl_X;
    float ylook = rc_Param.m_c_Trajectory[iT+1].s_Pos.rl_Y;
    float zlook = rc_Param.m_c_Trajectory[iT+1].s_Pos.rl_Z + 1.34f;
    xto = xlook;//-x;
    yto = ylook;//-y;
    zto = zlook;//-z;
}

// int proj::Render::Egopos_and_Lookat()

// rotate: https://www.opengl.org/discussion_boards/showthread.php/179290-Rotation-w-Rectangular-Pixels-(2D-VAO)

void proj::Render::DrawVAOs()
{
    float fXOffset = 2.0f, fYOffset = 2.0f, fZOffset = 1.0f;
    glm::vec3 v3;

    if (iWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

//    glUseProgram(program);
    glUniform3f(offsetAttrib, fXOffset, fYOffset, fZOffset);
    glBindVertexArray(vertexArray[VBO_2TRIANGLES]);
    glDrawArrays(GL_TRIANGLES, 0, vCount[VBO_2TRIANGLES]); // 2 triangles
//    glUseProgram(0);
    glUniform3f(offsetAttrib, 0.0f, 0.0f, 0.0f);

    glBindVertexArray(vertexArray[VBO_LEFT]);
    glDrawArrays(GL_TRIANGLES, 0, vCount[VBO_LEFT]); // lane marker Left
    glBindVertexArray(vertexArray[VBO_RIGHT]);
    glDrawArrays(GL_TRIANGLES, 0, vCount[VBO_RIGHT]); // lane marker Right

    // http://ogldev.atspace.co.uk/www/tutorial16/tutorial16.html
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture[TEX_ROADSURFACE]);

	glUniform1i(col_texAttrib,1);
    glBindVertexArray(vertexArray[VBO_ROAD]);
    glDrawArrays(GL_TRIANGLES, 0, vCount[VBO_ROAD]); // road
	glUniform1i(col_texAttrib,0);

    glBindVertexArray(vertexArray[VBO_TRAFFICSIGNS]);
    glDrawArrays(GL_TRIANGLES, 0, vCount[VBO_TRAFFICSIGNS]); // traffic signs

    if (b_Guardrail)
    {
        glBindVertexArray(vertexArray[VBO_GUARDRAIL]);
        glDrawArrays(GL_TRIANGLES, 0, vCount[VBO_GUARDRAIL]); // guardrails
    }

    // http://ogldev.atspace.co.uk/www/tutorial16/tutorial16.html
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture[1]);

    if (b_Curbstones)
    {
        glBindVertexArray(vertexArray[VBO_CURBSTONES]);
        glDrawArrays(GL_TRIANGLES, 0, vCount[VBO_CURBSTONES]); // curbstones
    }

	v3 = m_Moving[1]->position; // <-- Bewegung muss in einer eigenen loop erfolgen, da OpenGl nur bei neu malen bewegt (ala GLUT-Idle)
//    glUniform3f(offsetAttrib, v3.x, v3.y, v3.z);
//ROTATE (only after offset)
    glm::mat4 Model      = glm::mat4(1.0f);
	// first translate
	Model = glm::translate(Model,glm::vec3(v3.x,v3.y,v3.z));
	// then rotate
	// ! --> rotation is not "compatible" with offsetAttrib, that's why translation also done on Model matrix here
	// s. http://stackoverflow.com/questions/9920624/glm-combine-rotation-and-translation
//#define M_PI 3.14159265359
	// Reihenfolge der Rotationen ist wichtig, s. http://gamedev.stackexchange.com/questions/73467/glm-rotating-combining-multiple-quaternions
	glm::float32 f_VehRot_Z = -atan2(m_Moving[1]->direction[0],m_Moving[1]->direction[1]);
	glm::float32 f_VehRot_Z_DEG = glm::degrees(f_VehRot_Z);
	Model = glm::rotate(Model, f_VehRot_Z_DEG, glm::vec3(0.0f,0.0f,1.0f)); // where x, y, z is axis of rotation (e.g. 0 1 0)
//	glm::float32 f_VehTilt_DEG = 20.0f*abs(f_VehRot_Z);
//	Model = glm::rotate(Model, f_VehTilt_DEG, glm::vec3(0.0f,1.0f,0.0f)); // where x, y, z is axis of rotation (e.g. 0 1 0)
	p_cam->changeModel(Model);
//ROTATE
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture[TEX_CAR]);
	glUniform1i(col_texAttrib,1);
	glBindVertexArray(vertexArray[VBO_MOVING2]);
    glDrawArrays(GL_TRIANGLES, 0, vCount[VBO_MOVING2]); // moving object2 (car)
	glUniform1i(col_texAttrib,0);
//    glUniform3f(offsetAttrib, 0.0f, 0.0f, 0.0f);
	p_cam->resetModel();

//    glDisableVertexAttribArray(0);
//    glDisableVertexAttribArray(1);
    if (iWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (b_PNG) FBO_to_PPM();
}

//GLubyte pixels[fbo_height][fbo_width][3]; // too big for the stack
GLubyte puxels[1000][600][3]; // too big for the stack
void proj::Render::FBO_to_PPM() // write out as binary PPM (with lines in reverse order)
{
    glBindTexture(GL_TEXTURE_2D, color);//textureId[0]);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, puxels);
    glBindTexture(GL_TEXTURE_2D, 0); // <-?

    std::ofstream myfile;
    std::string filename;
    filename = "d:\\glShoot_FBO.ppm";
    myfile.open(filename.c_str());
    myfile << "P6" << std::endl;
    myfile << std::to_string((_ULonglong)fbo_width) << " " << std::to_string((_ULonglong)fbo_height) << std::endl; // "w h"
//    myfile << "1000" << " " << "600" << std::endl; // "w h"
    myfile << "255" << std::endl;
    myfile.close();
    myfile.open(filename.c_str(),std::ofstream::app | std::ofstream::binary); // binary-"trick" from: http://www.gamedev.net/topic/510801-render-to-memory/

    for (int i = 0; i < fbo_height; i++)
    {
        for (int j = 0; j < fbo_width; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                myfile << puxels[(fbo_height-1)-i][j][k];
            }
        }
    }
    myfile.close();
}
