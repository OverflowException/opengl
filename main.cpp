#include "glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>

#include "shader.h"

#include "glm/matrix.hpp"
#include "glm/gtc/matrix_transform.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void print_mat4(const glm::mat4& m) {
    std::cout << m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << std::endl;
    std::cout << m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << std::endl;
    std::cout << m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << std::endl;
    std::cout << m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3] << std::endl;
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build shaders
    ShaderManager sm;
    if (!sm.build("ico.vs", "ico.fs")) {
        std::cout << sm.error_msg << std::endl;
        return 0;
    }

    // read data
    std::vector<float> pb;
    std::vector<float> nb;
    std::vector<GLuint> ib;

    std::string p_file = "./data/ico_pos.data";
    std::string n_file = "./data/ico_norm.data";
    std::string i_file = "./data/ico_index.data";

    std::ifstream ifs;
    ifs.open(p_file);
    if (!ifs.is_open()) {
        std::cout << "Cannot open " << p_file << std::endl;
        return 0;
    }
    std::copy(
        std::istream_iterator<float>(ifs),
        std::istream_iterator<float>(),
        std::insert_iterator<std::vector<float>>(pb, pb.begin()));
    ifs.close();

    ifs.open(n_file);
    if (!ifs.is_open()) {
        std::cout << "Cannot open " << n_file << std::endl;
        return 0;
    }
    std::copy(
        std::istream_iterator<float>(ifs),
        std::istream_iterator<float>(),
        std::insert_iterator<std::vector<float>>(nb, nb.begin()));
    ifs.close();

    ifs.open(i_file);
    if (!ifs.is_open()) {
        std::cout << "Cannot open " << i_file << std::endl;
        return 0;
    }
    std::copy(
        std::istream_iterator<float>(ifs),
        std::istream_iterator<float>(),
        std::insert_iterator<std::vector<GLuint>>(ib, ib.begin()));
    ifs.close();

    GLuint array;
    GLuint buffers[3];

    glGenVertexArrays(1, &array);
    glGenBuffers(3, buffers);

    glBindVertexArray(array);

    // position attribute
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, pb.size() * sizeof(float), pb.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // buffer->array binding happens here
    glEnableVertexAttribArray(0);

    // normal attribute
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, nb.size() * sizeof(float), nb.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); // buffer->array binding happens here
    glEnableVertexAttribArray(1);

    // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib.size() * sizeof(GLuint), ib.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    // Compute matrices
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
                                glm::vec3(0.0f, -4.0f, 0.0f),
                                glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 model(1.0);

    // set up point light source
    glm::vec4 light_pos = glm::vec4(2.0f, -1.0f, 1.0f, 1.0);
    light_pos = view * light_pos;

    // depth test
    glEnable(GL_DEPTH_TEST);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set uniforms
        sm.setMat4("proj", proj);
        sm.setMat4("view", view);
        sm.setMat4("model", model);

        // set point light source
        sm.setVec3("light.position", glm::vec3(light_pos));
        sm.setVec3("light.ambient", glm::vec3(0.3f));
        sm.setVec3("light.diffuse", glm::vec3(0.25f));
        sm.setVec3("light.specular", glm::vec3(0.25f * 0.2f));

        // set material
        sm.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
        sm.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
        sm.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f)); // specular lighting doesn't have full effect on this object's material
        sm.setFloat("material.shininess", 32.0f);
        // sm.setVec4("light_pos", view * light_pos); // as in view space
        // sm.setVec4("light_color", light_color);

        sm.use();

        // render
        // ------
        glBindVertexArray(array);
        glDrawElements(GL_TRIANGLES, ib.size(), GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, vb.size() / 6);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(3, buffers);
    glDeleteVertexArrays(1, &array);
    glDeleteProgram(sm.id);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

