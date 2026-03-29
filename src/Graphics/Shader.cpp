#include "Shader.hpp"
#include "OpenGL.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>
#include <regex>
#include <cstring>
#include <functional>
#include <unordered_map>
#include <vector>

namespace ShoutBlast
{
    Shader::Shader()
	{
		m_id = 0;
	}

    Shader::Shader(const Shader &other)
	{
        m_id = other.m_id;
    }

    Shader::Shader(Shader &&other) noexcept
	{
		m_id = std::exchange(other.m_id, 0);
    }

    Shader& Shader::operator=(const Shader &other) 
	{
        if (this != &other) 
		{ 
            m_id = other.m_id;
        }
        return *this;
    }

    Shader& Shader::operator=(Shader &&other) noexcept
	 {
        if (this != &other) 
		{
            m_id = std::exchange(other.m_id, 0);
        }
        return *this;
    }

	enum ShaderType
	{
		ShaderType_Fragment = GL_FRAGMENT_SHADER,
		ShaderType_Vertex = GL_VERTEX_SHADER
	};

	static uint32_t CompileShader(const std::string &source, ShaderType shaderType)
	{
        uint32_t shader = glCreateShader(shaderType);
        const char *shaderSource = source.c_str();
        glShaderSource(shader, 1, &shaderSource, nullptr);
        glCompileShader(shader);

        int32_t success = 0;

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			switch(shaderType)
			{
			case GL_FRAGMENT_SHADER:
				throw std::runtime_error("Fragment shader compilation failed: " + std::string(infoLog));
			case GL_VERTEX_SHADER:
				throw std::runtime_error("Vertex shader compilation failed: " + std::string(infoLog));
			default:
				throw std::runtime_error("Unknown shader compilation failed: " + std::string(infoLog));
			}
        }

		return shader;
	}

	static uint32_t CreateAndLinkProgram(uint32_t *shaders, uint32_t count)
	{
		if(!shaders || count == 0)
			return 0;
		
		uint32_t id = glCreateProgram();

		for(uint32_t i = 0; i < count; i++)
		{
			glAttachShader(id, shaders[i]);
		}

		glLinkProgram(id);

		int32_t success = 0;

        glGetProgramiv(id, GL_LINK_STATUS, &success);

        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(id, 512, nullptr, infoLog);
			glDeleteProgram(id);
            throw std::runtime_error("Shader program linking failed: " + std::string(infoLog));
        }

		for(uint32_t i = 0; i < count; i++)
		{
			glDeleteShader(shaders[i]);
		}

		return id;
	}

    void Shader::Generate(const std::string &vertexSource, const std::string &fragmentSource)
    {
		uint32_t vertexShader = CompileShader(vertexSource, ShaderType_Vertex);
		uint32_t fragmentShader = CompileShader(fragmentSource, ShaderType_Fragment);

		uint32_t shaders[2] = {
			vertexShader,
			fragmentShader
		};

        m_id = CreateAndLinkProgram(shaders, 2);
    }

    void Shader::Destroy()
    {
        if (m_id)
            glDeleteProgram(m_id);
		m_id = 0;
    }

	void Shader::Use()
	{
		glUseProgram(m_id);
	}

    uint32_t Shader::GetId() const
    {
        return m_id;
    }

	void Shader::SetInt(const char *name, int32_t value)
	{
		glUniform1i(glGetUniformLocation(m_id, name), value);
	}

	void Shader::SetFloat(const char *name, float value)
	{
		glUniform1f(glGetUniformLocation(m_id, name), value);
	}

	void Shader::SetFloat2(const char *name, const float *value)
	{
		glUniform2fv(glGetUniformLocation(m_id, name), 1, value);
	}

	void Shader::SetFloat3(const char *name, const float *value)
	{
		glUniform3fv(glGetUniformLocation(m_id, name), 1, value);
	}

	void Shader::SetFloat4(const char *name, const float *value)
	{
		glUniform4fv(glGetUniformLocation(m_id, name), 1, value);
	}

	void Shader::SetMat2(const char *name, const float *value)
	{
		glUniformMatrix2fv(glGetUniformLocation(m_id, name), 1, false, value);
	}

	void Shader::SetMat3(const char *name, const float *value)
	{
		glUniformMatrix3fv(glGetUniformLocation(m_id, name), 1, false, value);
	}

	void Shader::SetMat4(const char *name, const float *value)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, false, value);
	}

	void Shader::SetIntEx(int32_t location, int32_t value)
	{
		glUniform1i(location, value);
	}

	void Shader::SetUIntEx(int32_t location, uint32_t value)
	{
		glUniform1ui(location, value);
	}

	void Shader::SetFloatEx(int32_t location, float value)
	{
		glUniform1f(location, value);
	}

	void Shader::SetFloat2Ex(int32_t location, const float *value)
	{
		glUniform2fv(location, 1, value);
	}

	void Shader::SetFloat3Ex(int32_t location, const float *value)
	{
		glUniform3fv(location, 1, value);
	}

	void Shader::SetFloat4Ex(int32_t location, const float *value)
	{
		glUniform4fv(location, 1, value);
	}

	void Shader::SetMat2Ex(int32_t location, const float *value)
	{
		glUniformMatrix2fv(location, 1, false, value);
	}

	void Shader::SetMat3Ex(int32_t location, const float *value)
	{
		glUniformMatrix3fv(location, 1, false, value);
	}

	void Shader::SetMat4Ex(int32_t location, const float *value)
	{
		glUniformMatrix4fv(location, 1, false, value);
	}
}