/**
Copyright (c) 2012 - Luu Gia Thuy

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#include <GL/glew.h>
#include <GL/glfw.h>

#include "ShaderManager.h"
#include "Shader.h"


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 800

#define BUFFER_OFFSET(bytes) ((unsigned char*)NULL + (bytes))

const char* TEXTURE_FILE_NAME			= "simpletexture.tga";
const char* VERTEX_SHADER_FILE_NAME		= "simple.vert";
const char* FRAGMENT_SHADER_FILE_NAME	= "simple.frag";
const char* SHADER_ATTR_POSITION		= "in_Position";
const char* SHADER_ATTR_TEXCOORD		= "in_TexCoord";
const char* SHADER_ATTR_NORMAL			= "in_Normal";
const char* SHADER_UNIF_MODELVIEWMAT	= "ModelViewMatrix";
const char* SHADER_UNIF_PROJMAT			= "ProjMatrix";
const char* SHADER_UNIF_TEXTUREMAP		= "TextureMap";


///////////////////////////////////////
/////			TYPES			//////
//////////////////////////////////////
struct SVertex {
	float	position[3];
	float	texCoord[2];
	float	normal[3];
};

struct STriangleObj {
	unsigned int vbo;
	unsigned int ibo;
	unsigned int triangleCount;
	unsigned int texture;
	float modelViewMatrix[16];
};

///////////////////////////////////////
/////			VARIABLES		//////
//////////////////////////////////////
int			g_IsRunning = 1;

CShader*		g_SimpleShader = NULL;
STriangleObj*	g_SimpleObj;

float		g_ProjMatrix[16] = {0};

// Initialize glfw and opengl, return 0 if failed
void initialize(void);
// set up scene
void setupScene(void);
// dispose scene
void disposeScene(void);
// render the scene
void renderScene(void);
// draw an STriangleObj
void drawTriangleObj(STriangleObj* inObj, CShader* inShader);
// key callback
void GLFWCALL keyFunction(int inKey, int inAction);
// mouse callback
void GLFWCALL mouseButtonFunction(int inButton, int inActon);
// GLFW window resize callback function
void GLFWCALL resizeFunction(int inWidth, int inHeight);
// shutdown window and exit program
void shutDown(int inReturnCode, const char* inErrorMsg);

int main (int argc, const char * argv[])
{
    initialize();
    
    while (g_IsRunning)
    {
        renderScene();
        
		// Check if window was closed
        g_IsRunning = g_IsRunning && glfwGetWindowParam( GLFW_OPENED );
    }
    
    shutDown(EXIT_SUCCESS, "");
}

void initialize()
{
    // initialze GLFW
    if (!glfwInit())
        shutDown(EXIT_FAILURE, "Failed to initialize GLFW");
    
    // Open OpenGL window
    if (!glfwOpenWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 0, 0, 0, 0, GLFW_WINDOW))
        shutDown(EXIT_FAILURE, "Failed to open GLFW window");
    
	// set window title
    glfwSetWindowTitle("Simple Shader Testing");
    
    // disable VSync (get as high FPS as possible)
    glfwSwapInterval(0);
    
	glewInit();
    
	// setup scene
	setupScene();
    
    // window resize callback function
    glfwSetWindowSizeCallback(resizeFunction);
    
    // keyboard input callback function
    glfwSetKeyCallback(keyFunction);
    
	// mouse button callback
	glfwSetMouseButtonCallback(mouseButtonFunction);
}

void setupScene()
{
	g_SimpleShader = CShaderManager::GetInstance()->GetShader(VERTEX_SHADER_FILE_NAME, FRAGMENT_SHADER_FILE_NAME, NULL);
	
	/// set up a rectangle object
	SVertex rectVertBuffer[4] = { {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f},
        {1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f},
        {1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f},
        {0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f}};
    
	int rectIndexBuffer[6] = {0, 1, 2, 0, 2, 3};
    
	g_SimpleObj = new STriangleObj;
	g_SimpleObj->vbo = 0;
	g_SimpleObj->ibo = 0;
	glGenBuffers(1, &g_SimpleObj->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_SimpleObj->vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(SVertex), &rectVertBuffer[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    
	glGenBuffers(1, &g_SimpleObj->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_SimpleObj->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,6 * sizeof(int), &rectIndexBuffer[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
	g_SimpleObj->triangleCount = 2;
    
	float modelViewMatrix[16] = {400.f, 0.f,	0.f, 0.f,
        0.f,	400.f,	0.f, 0.f,
        0.f,	0.f,	1.f, 0.f,
        350.f, 150.f,	0.f, 1.f};
    
	memcpy(&g_SimpleObj->modelViewMatrix[0], &modelViewMatrix[0], sizeof(float) * 16);
    
	// texture
	glGenTextures(1, &g_SimpleObj->texture);
	glBindTexture(GL_TEXTURE_2D, g_SimpleObj->texture);
    
	glfwLoadTexture2D(TEXTURE_FILE_NAME, GLFW_BUILD_MIPMAPS_BIT | GLFW_ORIGIN_UL_BIT);
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
	glBindTexture(GL_TEXTURE_2D, 0);
}

void disposeScene()
{
	/// free g_SimpleObj
	if (g_SimpleObj != NULL)
	{
		if (g_SimpleObj->vbo != 0)
		{
			glDeleteBuffers(1, &g_SimpleObj->vbo);
			g_SimpleObj->vbo = 0;
		}
        
		if (g_SimpleObj->ibo != 0)
		{
			glDeleteBuffers(1, &g_SimpleObj->ibo);
			g_SimpleObj->ibo = 0;
		}
        
		delete g_SimpleObj;
		g_SimpleObj = NULL;
	}
}

void renderScene()
{	
	glClearColor(0.4f, 0.5f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
	glUseProgram(g_SimpleShader->GetProgram());
    
	// projection matrix
	glUniformMatrix4fv(g_SimpleShader->GetUniformIndex(SHADER_UNIF_PROJMAT)
                       , 1, GL_FALSE, &g_ProjMatrix[0]);
	
	drawTriangleObj(g_SimpleObj, g_SimpleShader);
    
    glfwSwapBuffers();
}

void drawTriangleObj(STriangleObj* inObj, CShader* inShader)
{
	// set up model view matrix
	glUniformMatrix4fv(inShader->GetUniformIndex(SHADER_UNIF_MODELVIEWMAT)
                       , 1, GL_FALSE, &inObj->modelViewMatrix[0]);
    
	// texture
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, inObj->texture);
	glUniform1i(inShader->GetUniformIndex(SHADER_UNIF_TEXTUREMAP), 0);
    
	glBindBuffer(GL_ARRAY_BUFFER, inObj->vbo);
	// set up position
	int attrPosition = inShader->GetAttributeIndex(SHADER_ATTR_POSITION);
	glEnableVertexAttribArray(attrPosition);
	glVertexAttribPointer(attrPosition, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*)offsetof(SVertex, position));
    
	// set up texture coordinate
	int attrTexCoord = inShader->GetAttributeIndex(SHADER_ATTR_TEXCOORD);
	glEnableVertexAttribArray(attrTexCoord);
	glVertexAttribPointer(attrTexCoord, 2, GL_FLOAT, GL_TRUE, sizeof(SVertex), (GLvoid*)offsetof(SVertex, texCoord));
    
	// set up normal vector
	int attrNormal = inShader->GetAttributeIndex(SHADER_ATTR_NORMAL);
	glEnableVertexAttribArray(attrNormal);
	glVertexAttribPointer(attrNormal, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*)offsetof(SVertex, normal));
    
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inObj->ibo);
    
	glDrawElements(GL_TRIANGLES, inObj->triangleCount * 3, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
    
	// Unbind the buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLFWCALL keyFunction(int key, int action)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_ESC:
                g_IsRunning = 0;
                break;
            default:
                break;
        }
    }
}

void GLFWCALL mouseButtonFunction(int inButton, int inActon)
{
	/*if (inActon == GLFW_PRESS)
     {
     switch (inButton)
     {
     default:
     break;
     }
     }*/
}

void GLFWCALL resizeFunction(int w, int h)
{
    // set the new viewport
    glViewport(0, 0, w, h);
    
	// set up projection matrix
	//glOrtho
	// 2/(r-l)   0         0                -(r+l)/(r-l)
	// 0         2/(t-b)   0                -(t+b)/(t-b)
	// 0         0         -2/(cfar-cnear)  -(cfar+cnear)/(cfar-cnear)
	// 0         0         0                1
	float r = static_cast<float>(w);
	float l = 0.f;
	float b =  static_cast<float>(h);
	float t = 0.f;
	float cfar = 1000.f;
	float cnear = -1000.f;
    
	g_ProjMatrix[0] = 2.0f/(r-l);
	g_ProjMatrix[5] = 2.0f/(t-b);
	g_ProjMatrix[10] = -2.0f/(cfar-cnear);
	g_ProjMatrix[12] = -(r+l)/(r-l);
	g_ProjMatrix[13] = -(t+b)/(t-b);
	g_ProjMatrix[14] = -(cfar+cnear)/(cfar-cnear);
	g_ProjMatrix[15] = 1.f;
}

void shutDown(int returnCode, const char* errorMsg)
{
    glfwTerminate();
    if (returnCode != EXIT_SUCCESS)
        printf("%s\n", errorMsg);
    
	disposeScene();
    
    exit(returnCode);
}