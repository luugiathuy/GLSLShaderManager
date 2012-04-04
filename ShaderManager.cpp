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

#include "Shader.h"
#include "ShaderManager.h"
#include <GL/glew.h>

/// Maximum line length of a shader's source file
const int MAX_LINE_LENGTH = 1024;

CShaderManager* CShaderManager::s_Instance = NULL;
const char* CShaderManager::DEFAULT_SHADER = "DefaultShader";

CShaderManager::CShaderManager()
{
	// create default shader for unsuccessful GetShader()
	// the default Shader has program value which is 0 (default)
	CShader* theDefaultShader = new CShader();
	m_ShaderMap[DEFAULT_SHADER] = theDefaultShader;
}

CShaderManager::~CShaderManager()
{
	// clean up all shaders
	TShaderMap::iterator iter;
	for (iter = m_ShaderMap.begin(); iter != m_ShaderMap.end(); ++iter)
	{
		Dispose(iter->second);
		delete iter->second;
	}
	m_ShaderMap.clear();
}

CShaderManager* CShaderManager::GetInstance()
{
	if (s_Instance == NULL)
		s_Instance = new CShaderManager;
	return (s_Instance);
}

CShader* CShaderManager::GetShader(const char* inVertFileName, const char* inFragFileName, const char* inGeomFileName)
{
	std::string theString = inVertFileName;
	if(inFragFileName)
		theString += inFragFileName;
	if(inGeomFileName)
		theString += inGeomFileName;

	TShaderMap::iterator i = m_ShaderMap.find(theString);
	if(i != m_ShaderMap.end())
	{
		return (i->second);
	}
	else
	{
		CShader* theResult = Load(inVertFileName, inFragFileName, inGeomFileName);
		if(theResult != NULL)
		{
			// successful loaded and linked shader program, added to map and return the result
			m_ShaderMap[theString] = theResult;
			return theResult;
		}
	}

	// if load/link unsuccessfully, return default shader which program = 0
	return m_ShaderMap[DEFAULT_SHADER];
}

CShader* CShaderManager::Load(const char* inVertexFilename, const char* inFragmentFilename, const char* inGeometryFilename)
{
	unsigned int vertShader = 0, fragShader = 0, geomShader = 0, programShader = 0;
	bool loadStatus;
	
	// load and compile vertex. geometric and fragment sources
	loadStatus = LoadShader(GL_VERTEX_SHADER, inVertexFilename, vertShader);
	loadStatus &= LoadShader(GL_FRAGMENT_SHADER, inFragmentFilename, fragShader);
	// if geometry file is provided, load it
	if (inGeometryFilename)
		loadStatus &= LoadShader(GL_GEOMETRY_SHADER_EXT, inGeometryFilename, geomShader);


	if (loadStatus) 
	{
		// create a program
		programShader = glCreateProgram();

		if (programShader == 0)
			return NULL;

		// attach the vertex and fragment shader codes, and the geometric if available
		glAttachShader(programShader, vertShader);

		if (geomShader != 0)
			glAttachShader(programShader, geomShader);

		glAttachShader(programShader, fragShader);

		// link
		glLinkProgram(programShader);

		// check link status
		GLint status = GL_FALSE;
		glGetProgramiv(programShader, GL_LINK_STATUS, &status);
		if (status != GL_TRUE) {
			// The link has failed, check log info
			int logLength = 1;
			glGetProgramiv(programShader, GL_INFO_LOG_LENGTH, &logLength);

			char* infoLog = (char*)malloc(logLength+1);
			glGetProgramInfoLog(programShader, logLength, &logLength, infoLog);
			printf("Failed to link the shader: %s\n", infoLog);
			free(infoLog);

			return NULL;
		}

		// check if the shader will run in the current OpenGL state
		glValidateProgram(programShader);
		glGetProgramiv(programShader, GL_VALIDATE_STATUS, &status);
		if (status != GL_TRUE) {
			printf("Shader program will not run in this OpenGL environment!\n");
			return NULL;
		}


		// the program has been loaded/linked successfully
		CShader* theResult = new CShader;
		theResult->SetVertShader(vertShader);
		theResult->SetFragShader(fragShader);
		theResult->SetGeomShader(geomShader);
		theResult->SetProgram(programShader);

		return theResult;
	}

	return NULL;
}

void CShaderManager::Dispose(CShader* inShader)
{
	if (inShader->GetProgram() != 0) {
		glDeleteProgram(inShader->GetProgram());
	}

	if (inShader->GetVert() != 0) {
		glDeleteShader(inShader->GetVert());
	}

	if (inShader->GetGeom() != 0) {
		glDeleteShader(inShader->GetGeom());
	}

	if (inShader->GetFrag() != 0) {
		glDeleteShader(inShader->GetFrag());
	}
}

bool CShaderManager::LoadShader(unsigned int inShaderType, const std::string& inFileName, GLuint &inOutShader)
{
	if (inFileName.empty()) {
		printf("Shader's filename is empty!\n");
		return false;
	}

	// load shader source from file
	int sourceSize = 0;
	char** pShaderSource = LoadSource(sourceSize, inFileName);
	if (pShaderSource == NULL){
		printf("Cannot load file source %s.\n", inFileName.c_str());
		return false;
	}

	// create shader pointer
	inOutShader = glCreateShader(inShaderType);
	if (inOutShader == 0) {
		printf("Cannot create shader, type: %u\n", inShaderType);
		return false;
	}

	// compile shader
	glShaderSource(inOutShader, sourceSize, (const GLchar**)pShaderSource, NULL);
	glCompileShader(inOutShader);

	// free up the source
	for (int i=0; i < sourceSize; ++i) {
		if (pShaderSource[i] != NULL)
			free(pShaderSource[i]);
	}
	free(pShaderSource);

	// check compilation success
	GLint status = GL_FALSE;
	glGetShaderiv(inOutShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		// fail to compile, check the log
		int logLength = 1;
		glGetShaderiv(inOutShader, GL_INFO_LOG_LENGTH, &logLength);

		char* infoLog = (char*)malloc(logLength + 1);
		glGetShaderInfoLog(inOutShader, logLength, &logLength, infoLog);
		printf("Failed to compile shader %s\n%s", inFileName.c_str(), infoLog);
		free(infoLog);

		return false;
	}

	return true;
} // end LoadShader


GLchar** CShaderManager::LoadSource(int& outLineCount, const std::string &inFileName)
{
	char line[MAX_LINE_LENGTH];
	int length = 0, lineLength;
	GLchar** pSource;

	outLineCount = 0;
	FILE* pFile = fopen(inFileName.c_str(), "r");
	if (pFile == NULL) {
		printf("Cannot open file: %s\n", inFileName.c_str());
		return NULL;
	}

	// get the total length of the code
	while (fgets(line, MAX_LINE_LENGTH, pFile) != NULL) {
		lineLength = strlen(line);
		if (lineLength > MAX_LINE_LENGTH) {
			printf("Cannot read lines longer than %i characters in file: %s", MAX_LINE_LENGTH, inFileName.c_str());
			outLineCount = 0;
			fclose(pFile);
			return NULL;
		}
		length += lineLength + 1;
		++outLineCount;
	}

	rewind(pFile);
	pSource = (GLchar**)malloc(sizeof(GLchar*) * (outLineCount));
	if (pSource == NULL) {
		printf("Out of memory!\n");
		return NULL;
	}

	length = 0;
	int i = 0;
	while (fgets(line, MAX_LINE_LENGTH, pFile) != NULL) {
		lineLength = strlen(line);
		pSource[i] = (GLchar*)malloc(sizeof(GLchar) * (lineLength + 1));
		if (pSource[i] == NULL) {
			printf("Out of memory!\n");
			return NULL;
		}

		GLchar* ps = pSource[i];
		GLchar* pl = line;

		// concatenates the string
		while (*pl != '\0')
			*ps++ = *pl++;

		*ps++ = '\0';
		++i;
	}

	fclose(pFile);

	return pSource;
} // end LoadSource