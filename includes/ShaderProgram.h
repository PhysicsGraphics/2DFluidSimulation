#ifndef SHADER_H_
#define SHADER_H_

class ShaderProgram {

public:
	ShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
	virtual ~ShaderProgram();
	void UseShader() const;
	unsigned int GetProgramId() const;
	void DeleteShader();

private:
	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int program;

	unsigned int LoadShader(unsigned int shaderType, const char* shaderFile);
};

#endif /* SHADER_H_ */

