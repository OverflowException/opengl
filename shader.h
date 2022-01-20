#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include "glad.h"
#include "glm/glm.hpp"

class ShaderManager {
public:
    GLuint id;
    std::string error_msg;
    
    bool build(const std::string& vs, const std::string& fs) {
        std::ifstream ifs;
        std::ostringstream ss;

        // read vertex shader
        ifs.open(vs);
        if (!ifs.is_open()) {
            error_msg = "[VERTEX SHADER FILE] Cannot open " + vs;
            return false;
        }

        ss << ifs.rdbuf();
        std::string vs_code = std::move(ss.str());

        ss.clear();
        ss.str("");
        ifs.close();

        // read fragment shader
        ifs.open(fs);
        if (!ifs.is_open()) {
            error_msg = "[FRAGMENT SHADER FILE] Cannot open " + fs;
            return false;
        }

        ss << ifs.rdbuf();
        std::string fs_code = std::move(ss.str());

        ss.clear();
        ss.str("");
        ifs.close();

        GLuint vs_id, fs_id;
        // Compile vertex shader
        vs_id = glCreateShader(GL_VERTEX_SHADER);
        const char *const vs_code_str = vs_code.c_str();
        glShaderSource(vs_id, 1, &vs_code_str, nullptr);
        glCompileShader(vs_id);
        if (!check_errors("VERTEX", vs_id)) {
            return false;
        }

        // Compile fragment shader
        fs_id = glCreateShader(GL_FRAGMENT_SHADER);
        const char* const fs_code_str = fs_code.c_str();
        glShaderSource(fs_id, 1, &fs_code_str, nullptr);
        glCompileShader(fs_id);
        if (!check_errors("FRAGMENT", fs_id)) {
            return false;
        }

        this->id = glCreateProgram();
        glAttachShader(this->id, vs_id);
        glAttachShader(this->id, fs_id);
        glLinkProgram(this->id);
        check_errors("PROGRAM", this->id);

        glDeleteShader(fs_id);
        glDeleteShader(vs_id);
        return true;
    }

    void use() {
        glUseProgram(this->id);
    }

    bool setFloat(const std::string& name, float val) {
        GLint u_loc = glGetUniformLocation(this->id, name.c_str());
        if (u_loc == -1) {
            return false;
        } else {
            glUniform1f(u_loc, val);
            return true;
        }
    }

    bool setInt(const std::string& name, int val) {
        GLint u_loc = glGetUniformLocation(this->id, name.c_str());
        if (u_loc == -1) {
            return false;
        } else {
            glUniform1i(u_loc, val);
            return true;
        }
    }

    bool setMat4(const std::string& name, const glm::mat4& m) {
        GLint u_loc = glGetUniformLocation(this->id, name.c_str());
        if (u_loc == -1) {
            return false;
        } else {
            glUniformMatrix4fv(u_loc, 1, GL_FALSE, &(m[0][0]));
            return true;
        }

    }

    bool setVec4(const std::string& name, const glm::vec4& v) {
        GLint u_loc = glGetUniformLocation(this->id, name.c_str());
        if (u_loc == -1) {
            return false;
        } else {
            glUniform4fv(u_loc, 1, &(v[0]));
            return true;
        }
    }

    bool setVec3(const std::string& name, const glm::vec3& v) {
        GLint u_loc = glGetUniformLocation(this->id, name.c_str());
        if (u_loc == -1) {
            return false;
        } else {
            glUniform3fv(u_loc, 1, &(v[0]));
            return true;
        }
    }

private:
    bool check_errors(const std::string& type, GLuint id) {
        int status;
        char log[1024];
        bool ret = true;

        if (type == "VERTEX" || type == "FRAGMENT") {
            glGetShaderiv(id, GL_COMPILE_STATUS, &status);
            if (status != GL_TRUE) {
                glGetShaderInfoLog(id, 1024, nullptr, log);
                error_msg = "[" + type + " SHADER COMPILATION]" + std::string(log);
                ret = false;
            } else {
                ret = true;
            }
        } else if (type == "PROGRAM") {
            glGetProgramiv(id, GL_LINK_STATUS, &status);
            if (status != GL_TRUE) {
                glGetProgramInfoLog(id, 1024, nullptr, log);
                error_msg = "[SHADER PROGRAM LINK]" + std::string(log);
                ret = false;
            } else {
                ret = true;
            }
        } else {
            assert(false);            
        }

        return ret;
    }

};