#ifndef SHADER_H
#define SHADER_H

class Shader {
public:
	unsigned int m_SID;

	void Compile(
		const char* vertexSource,
		const char* fragSource,
		const char* geoSource = nullptr);
	
	Shader& Use();

private:
	void LogError(unsigned int id, const std::string_view& name);

};

#endif