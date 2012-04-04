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

#pragma once

#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <string>
#include <map>

// Forward declaration
class CShader;

class CShaderManager
{
////////////////////////////////////////////////////////////
//	Types
////////////////////////////////////////////////////////////
protected:
	typedef std::map<std::string, CShader*> TShaderMap;

////////////////////////////////////////////////////////////
//	Fields
////////////////////////////////////////////////////////////
protected:

	/// Map of shader name and shader obj pointer
	TShaderMap m_ShaderMap;

	/// Default shader string
	static const char* DEFAULT_SHADER;

	/// The unique instance of this class
	static CShaderManager*	s_Instance;

////////////////////////////////////////////////////////////
//	Methods
////////////////////////////////////////////////////////////
public:
	/// Destructor
	~CShaderManager();

	// Get the unique instance of this class
	static CShaderManager*	GetInstance();

	/**
	 * Get shader object pointer
	 * @return if successful load/link, the loaded/linked shader object point is return, otherwise return the default shader
	 */
	CShader* GetShader(const char* inVertFileName, const char* inFragFileName, const char* inGeomFileName);

protected:
	/// Default constructor (protected)
	CShaderManager();

	/**
	 * Load, compile and create a vertex/fragment/geometric shader program and link the program
	 * @return CShader object pointer if all shaders are loaded successfully, NULL otherwise
	 */
	CShader* Load(const char* inVertexFilename
				, const char* inFragmentFilename
				, const char* inGeometryFilename);

	/** Releases all resources */
	void Dispose(CShader* inShader);

	/** 
	 * Load and compile a shader
	 * @param inShaderType type of shader: GL_VERTEX_SHADER, GL_GEOMETRY_SHADER_EXT or GL_FRAGMENT_SHADER
	 * @param inFileName shader file name
	 * @param inOutShader the pointer to shader
	 * @return true if successfully loaded, false otherwise
	 */
	bool LoadShader(unsigned int inShaderType,  const std::string& inFileName, unsigned int & inOutShader);


	/**
	 * Load a shader source code from a file. 
	 * @return the pointer to the source
	 */
	char** LoadSource(int& outLineCount, const std::string& inFileName);

}; // end class ShaderManager

#endif