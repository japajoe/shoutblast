#ifndef FAYA_SHADER_HPP
#define FAYA_SHADER_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace ShoutBlast
{
	class Shader
	{
	public:
		Shader();
    	Shader(const Shader &other);
    	Shader(Shader &&other) noexcept;
		Shader& operator=(const Shader &other);
		Shader& operator=(Shader &&other) noexcept;
		void Generate(const std::string &vertexSource, const std::string &fragmentSource);
		void Destroy();
		void Use();
		uint32_t GetId() const;
		void SetInt(const char *name, int32_t value);
		void SetFloat(const char *name, float value);
		void SetFloat2(const char *name, const float *value);
		void SetFloat3(const char *name, const float *value);
		void SetFloat4(const char *name, const float *value);
		void SetMat2(const char *name, const float *value);
		void SetMat3(const char *name, const float *value);
		void SetMat4(const char *name, const float *value);
		void SetIntEx(int32_t location, int32_t value);
		void SetUIntEx(int32_t location, uint32_t value);
		void SetFloatEx(int32_t location, float value);
		void SetFloat2Ex(int32_t location, const float *value);
		void SetFloat3Ex(int32_t location, const float *value);
		void SetFloat4Ex(int32_t location, const float *value);
		void SetMat2Ex(int32_t location, const float *value);
		void SetMat3Ex(int32_t location, const float *value);
		void SetMat4Ex(int32_t location, const float *value);
	private:
		uint32_t m_id;
	};
}

#endif