#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;

    // Конструктор читает и компилирует шейдеры
    Shader(const char* vertexPath, const char* fragmentPath) {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        // Настраиваем потоки на перехват ошибок
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (std::ifstream::failure& e) {
            std::cerr << "[Shader] ОШИБКА: Файлы шейдеров не прочитаны! Проверь пути: " << vertexPath << "\n";
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertex, fragment;
        
        // 1. Компилируем Vertex Shader (отвечает за 3D-координаты)
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        CheckCompileErrors(vertex, "VERTEX");

        // 2. Компилируем Fragment Shader (отвечает за цвет/текстуру пикселей)
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        CheckCompileErrors(fragment, "FRAGMENT");

        // 3. Собираем их в готовую программу
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        CheckCompileErrors(ID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // Активация шейдера перед отрисовкой
    void Use() {
        glUseProgram(ID);
    }

    // Передача 3D-матриц (камеры, позиции) в видеокарту
    void SetMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

private:
    void CheckCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "[Shader] ОШИБКА КОМПИЛЯЦИИ: " << type << "\n" << infoLog << "\n";
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "[Shader] ОШИБКА ЛИНКОВКИ ПРОГРАММЫ: " << type << "\n" << infoLog << "\n";
            }
        }
    }
};