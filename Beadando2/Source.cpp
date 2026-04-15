#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

enum eVertexArrayObject {
    VAOControlPoints,
    VAOCurvePoints,
    VAOCount
};

enum eBufferObject {
    VBOControlPoints,
    VBOCurvePoints,
    BOCount
};

enum eProgram {
    BezierProgram,
    ProgramCount
};

enum eTexture {
    NoTexture,
    TextureCount
};

#include "common.cpp"

GLchar windowTitle[] = "Erdei Erik Martin W01MZS - Beadando 2";

vector<vec2> controlPoints;
vector<vec2> curvePoints;
GLint dragged = -1;

GLint uColorLocation;
GLint uIsPointLocation;

float NCR(int n, int r) {
    if (r == 0 || r == n) return 1.0f;
    if (r > n / 2) r = n - r;
    double result = 1.0;
    for (int k = 1; k <= r; ++k) {
        result *= n - k + 1;
        result /= k;
    }
    return (float)result;
}

vec2 getBezierPoint(float t) {
    vec2 p(0.0f, 0.0f);
    int n = controlPoints.size() - 1;
    for (int i = 0; i <= n; ++i) {
        float blending = NCR(n, i) * pow(t, i) * pow(1.0f - t, n - i);
        p += blending * controlPoints[i];
    }
    return p;
}

void updateCurve() {
    curvePoints.clear();
    if (controlPoints.empty()) return;
    if (controlPoints.size() == 1) {
        curvePoints.push_back(controlPoints[0]);
        return;
    }

    int resolution = 150;
    for (int i = 0; i <= resolution; ++i) {
        float t = (float)i / resolution;
        curvePoints.push_back(getBezierPoint(t));
    }
}

GLfloat dist2(vec2 P1, vec2 P2) {
    vec2 delta = P1 - P2;
    return dot(delta, delta);
}

GLint getActivePoint(float sensitivity, vec2 mousePos) {
    float s = sensitivity * sensitivity;
    for (int i = 0; i < controlPoints.size(); i++) {
        if (dist2(controlPoints[i], mousePos) < s) return i;
    }
    return -1;
}

void updateBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOControlPoints]);
    glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(vec2), controlPoints.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOCurvePoints]);
    glBufferData(GL_ARRAY_BUFFER, curvePoints.size() * sizeof(vec2), curvePoints.data(), GL_DYNAMIC_DRAW);
}

void init(GLFWwindow* window) {
    ShaderInfo shader_info[] = {
        { GL_VERTEX_SHADER,   "./vertexShader.glsl" },
        { GL_FRAGMENT_SHADER, "./fragmentShader.glsl" },
        { GL_NONE,            nullptr }
    };

    program[BezierProgram] = LoadShaders(shader_info);

    uColorLocation = glGetUniformLocation(program[BezierProgram], "uColor");
    uIsPointLocation = glGetUniformLocation(program[BezierProgram], "uIsPoint");

    glGenVertexArrays(VAOCount, VAO);
    glGenBuffers(BOCount, BO);

    glBindVertexArray(VAO[VAOControlPoints]);
    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOControlPoints]);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO[VAOCurvePoints]);
    glBindBuffer(GL_ARRAY_BUFFER, BO[VBOCurvePoints]);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(10.0f);
    glLineWidth(2.0f);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program[BezierProgram]);

    if (controlPoints.empty()) return;

    glBindVertexArray(VAO[VAOControlPoints]);
    glUniform4f(uColorLocation, 0.6f, 0.6f, 0.6f, 1.0f);
    glUniform1i(uIsPointLocation, 0);
    glDrawArrays(GL_LINE_STRIP, 0, controlPoints.size());

    glUniform4f(uColorLocation, 1.0f, 0.2f, 0.2f, 1.0f);
    glUniform1i(uIsPointLocation, 1);
    glDrawArrays(GL_POINTS, 0, controlPoints.size());

    if (curvePoints.size() > 1) {
        glBindVertexArray(VAO[VAOCurvePoints]);
        glUniform4f(uColorLocation, 0.2f, 1.0f, 0.2f, 1.0f);
        glUniform1i(uIsPointLocation, 0);
        glDrawArrays(GL_LINE_STRIP, 0, curvePoints.size());
    }

    glBindVertexArray(0);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
    if (dragged >= 0) {
        float xNorm = (xPos / windowWidth) * 2.0f - 1.0f;
        float yNorm = ((windowHeight - yPos) / windowHeight) * 2.0f - 1.0f;
        controlPoints[dragged] = vec2(xNorm, yNorm);

        updateCurve();
        updateBuffers();
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        float xNorm = (xPos / windowWidth) * 2.0f - 1.0f;
        float yNorm = ((windowHeight - yPos) / windowHeight) * 2.0f - 1.0f;
        vec2 mousePos(xNorm, yNorm);

        int clickedIndex = getActivePoint(0.05f, mousePos);

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (clickedIndex != -1) {
                dragged = clickedIndex;
            }
            else {
                controlPoints.push_back(mousePos);
                updateCurve();
                updateBuffers();
                dragged = controlPoints.size() - 1;
            }
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (clickedIndex != -1) {
                controlPoints.erase(controlPoints.begin() + clickedIndex);
                updateCurve();
                updateBuffers();
            }
        }
    }
    else if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
        dragged = -1;
    }
}

int main(void) {
    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) exit(EXIT_FAILURE);

    init(window);

    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}