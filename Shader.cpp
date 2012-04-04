#include "Shader.h"
#include <GL/glew.h>

CShader::CShader()
: m_VertexShader(0),
m_GeometricShader(0),
m_FragmentShader(0),
m_Program(0)
{
}

CShader::~CShader()
{
}

int CShader::GetUniformIndex(const char* inVarName)
{
	return GetVariableIndex(inVarName, true);;
}

int CShader::GetAttributeIndex(const char* inVarName)
{
	return GetVariableIndex(inVarName, false);
}

int CShader::GetVariableIndex(const char *inVarName, bool inIsUniform)
{
	int theResult = -1;

	if (m_Program != 0)
	{
		TVariableMap::iterator iter;
		iter = m_VariableMap.find(inVarName);
		if (iter == m_VariableMap.end())
		{
			if (inIsUniform) // uniform variables
				theResult = glGetUniformLocation(m_Program, inVarName);
			else // attribute variables
				theResult = glGetAttribLocation(m_Program, inVarName);
			if(theResult != -1)
				m_VariableMap[inVarName] = theResult;
		}
		else
			theResult = iter->second;
	}

	return theResult;
}