#ifndef SHADER_H
#define SHADER_H

#include <string_view>


class Shader {
public:
	unsigned int m_SID;

	void Load(
		const std::filesystem::path& vertexPath,
		const std::filesystem::path& fragPath,
		const std::filesystem::path& geoPath);

	Shader& Use();

private:
	void Compile(const char* v, const char* f, const char* g = nullptr);
	void LogError(unsigned int id, const std::string_view& name);

};

#endif