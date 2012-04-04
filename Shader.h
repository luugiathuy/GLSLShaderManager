#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <map>

class CShader
{
////////////////////////////////////////////////////////////
//	Types
////////////////////////////////////////////////////////////
protected:
	typedef std::map<std::string, int> TVariableMap;

////////////////////////////////////////////////////////////
//	Fields
////////////////////////////////////////////////////////////
protected:
	/// map stores shader's variables
	TVariableMap m_VariableMap;

	/// Shader properties
	unsigned int m_VertexShader;
	unsigned int m_GeometricShader;
	unsigned int m_FragmentShader;
	unsigned int m_Program;


////////////////////////////////////////////////////////////
//	Methods
////////////////////////////////////////////////////////////
public:
	/// Constructor
	CShader();

	/// Destructor
	~CShader();

	/// Getter/setters shader properties
	inline unsigned int GetVert() { return m_VertexShader; }
	inline unsigned int GetGeom() { return m_GeometricShader; }
	inline unsigned int GetFrag() { return m_FragmentShader; }
	inline unsigned int GetProgram() { return m_Program; }

	inline void SetVertShader(unsigned int inValue) { m_VertexShader = inValue; }
	inline void SetGeomShader(unsigned int inValue) { m_GeometricShader = inValue; }
	inline void SetFragShader(unsigned int inValue) { m_FragmentShader = inValue; }
	inline void SetProgram(unsigned int inValue) { m_Program = inValue; }

	///Get index of an atribute variable of this shader
	int GetAttributeIndex(const char* inVarName);

	/// Get index of an uniform variable of this shader
	int GetUniformIndex(const char* inVarName);

protected:
	/**
	 * Get index of an variable of the shader
	 * @param inVarName the variable name
	 * @param inIsUniform whether the variable is an uniform
	 */
	int GetVariableIndex(const char* inVarName, bool inIsUniform);

}; // end class CShader

#endif