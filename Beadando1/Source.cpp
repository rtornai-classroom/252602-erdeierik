#include <iostream>
#include <algorithm>
#include <cmath>

enum eVertexArrayObject { VAOQuad, VAOCount };
enum eBufferObject { VBOQuad, BOCount };
enum eProgram { MainProgram, ProgramCount };
enum eTexture { NoTexture, TextureCount };

#include <common.cpp>

GLchar windowTitle[] = "Beadando 1 Erdei Erik";

GLint locationObjectType;
GLint locationCircleCenter;
GLint locationRadius;
GLint locationIsIntersecting;

float circleX = 300.0f;
float circleY = 300.0f;
float velX = 5.0f;
float velY = 0.0f;
float radius = 50.0f;
float lineY = 300.0f;

static std::array<GLfloat, 8> quadVertices = {
    -0.5f,  0.5f,
     0.5f,  0.5f,
    -0.5f, -0.5f,
     0.5f, -0.5f
};

void initShadersAndBuffers() {
    ShaderInfo shader_info[ProgramCount][3] = { {
        { GL_VERTEX_SHADER,     "./Vertex.glsl" },
        { GL_FRAGMENT_SHADER,   "./Fragment.glsl" },
        { GL_NONE, nullptr } }
    };

    program[MainProgram] = LoadShaders(shader_info[MainProgram]);

    locationMatProjection = glGetUniformLocation(program[MainProgram], "matProjection");
    locationMatModelView = glGetUniformLocation(program[MainProgram], "matModelView");
    locationObjectType = glGetUniformLocation(program[MainProgram], "objectType");
    locationCircleCenter = glGetUniformLocation(program[MainProgram], "circleCenter");
    locationRadius = glGetUniformLocation(program[MainProgram], "radius");
    locationIsIntersecting = glGetUniformLocation(program[MainProgram], "isIntersecting");

    glBindVertexArray(VAO[VAOQuad]);
    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOQuad]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
}

void display(GLFWwindow* window, GLdouble currentTime) {
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    circleX += velX;
    circleY += velY;

    if (circleX + radius > 600.0f) {
        circleX = 600.0f - radius - (circleX + radius - 600.0f);
        velX = -velX;
    }
    else if (circleX - radius < 0.0f) {
        circleX = radius + (radius - circleX);
        velX = -velX;
    }

    if (circleY + radius > 600.0f) {
        circleY = 600.0f - radius - (circleY + radius - 600.0f);
        velY = -velY;
    }
    else if (circleY - radius < 0.0f) {
        circleY = radius + (radius - circleY);
        velY = -velY;
    }

    float closestX = std::max(200.0f, std::min(circleX, 400.0f));
    float closestY = std::max(lineY - 1.5f, std::min(circleY, lineY + 1.5f));

    float distX = circleX - closestX;
    float distY = circleY - closestY;
    bool isIntersecting = (distX * distX + distY * distY) <= (radius * radius);

    glUseProgram(program[MainProgram]);
    glUniformMatrix4fv(locationMatProjection, 1, GL_FALSE, glm::value_ptr(matProjection));

    glBindVertexArray(VAO[VAOQuad]);

    matModelView = glm::translate(glm::mat4(1.0f), glm::vec3(300.0f, lineY, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(200.0f, 3.0f, 1.0f));
    glUniformMatrix4fv(locationMatModelView, 1, GL_FALSE, glm::value_ptr(matModelView));
    glUniform1i(locationObjectType, 1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    matModelView = glm::translate(glm::mat4(1.0f), glm::vec3(circleX, circleY, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(radius * 2.0f, radius * 2.0f, 1.0f));
    glUniformMatrix4fv(locationMatModelView, 1, GL_FALSE, glm::value_ptr(matModelView));
    glUniform1i(locationObjectType, 0);
    glUniform2f(locationCircleCenter, circleX, circleY);
    glUniform1f(locationRadius, radius);
    glUniform1i(locationIsIntersecting, isIntersecting ? 1 : 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    matProjection = glm::ortho(0.0f, 600.0f, 0.0f, 600.0f, -1.0f, 1.0f);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_UP) {
        lineY += 10.0f;
    }
    if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_DOWN) {
        lineY -= 10.0f;
    }

    if (action == GLFW_PRESS && key == GLFW_KEY_S) {
        float angleRad = glm::radians(25.0f);
        velX = std::cos(angleRad) * 10.0f;
        velY = std::sin(angleRad) * 10.0f;
    }
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {}

int main(void) {
    windowWidth = 600;
    windowHeight = 600;

    init(3, 3, GLFW_OPENGL_CORE_PROFILE);

    glfwSetWindowSize(window, windowWidth, windowHeight);

    initShadersAndBuffers();

    framebufferSizeCallback(window, windowWidth, windowHeight);

    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanUpScene(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}