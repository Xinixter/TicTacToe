#ifndef SHADER_H
#define SHADER_H

#include <string_view>
#include <filesystem>


class Shader {
public:
	unsigned int m_SID;

	void Load(
		const char* vertexPath,
		const char* fragPath,
		const char* geoPath = nullptr);

	Shader& Use();

private:
	void Compile(const char* v, const char* f, const char* g = nullptr);
	void LogError(unsigned int id, const std::string_view& name);

};

#endif