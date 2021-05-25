
#include <GL/glew.h>
#include <GL/gl.h>

namespace ShaderUtils
{

GLuint CreateShaderProgram(const char * vertShader, const char * fragShader);

GLuint CompileAttachShader(GLuint programHandle, const char * filename, GLuint shaderType);

GLuint LinkProgram(GLuint programHandle);

} // namespace ShaderHelpers