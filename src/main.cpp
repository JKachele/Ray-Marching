#include <iostream>
#include <glad/glad.h>
#include <glfw3.h>
#include <Shadinclude.hpp>
#include <cstring>

using std::cout;
using std::cerr;
using std::string;
using std::endl;

int windowWidth;
int windowHeight;
GLFWwindow* glfwWindow;
unsigned int shaderProgramID;
unsigned int vaoID;
unsigned int vboID;
unsigned int eboID;

float vertexArray[12] {
    -1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
     1.0f,-1.0f,0.0f
};

int elementArray[6] = {
        0, 2, 1,
        0, 3, 2
};

void startWindow(const char* title) {
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return;
    }

    // Set the opengl version to opengl version 4.6 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);   // Window will not be visible during creation
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);  // Window will be resizeable

    // Create the window
    glfwWindow = glfwCreateWindow(windowWidth, windowHeight, title, nullptr, nullptr);
    if (glfwWindow == nullptr) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        glfwTerminate();
        return;
    }

    // set the viewport to the size of the window
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        windowWidth = width;
        windowHeight = height;
    });

    // Set the window position to the middle of the screen
    const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (vidMode != nullptr) {
        glfwSetWindowPos(glfwWindow,
                         (vidMode->width - windowWidth) / 2, (vidMode->height - windowHeight) / 2);
    }

    glfwSwapInterval(1);    // Enable v-sync: Sets max FPS to screen refresh rate

    glfwShowWindow(glfwWindow);
}

void initShader(const string& vertexShaderFile, const string& fragmentShaderFile) {
    string vertexShaderString;
    string fragmentShaderString;
    const char* vertexShader;
    const char* fragmentShader;

    vertexShaderString = Shadinclude::load(vertexShaderFile);
    vertexShader = strcpy(new char[vertexShaderString.length() + 1],
                                vertexShaderString.c_str());
    fragmentShaderString = Shadinclude::load(fragmentShaderFile);
    fragmentShader = strcpy(new char[fragmentShaderString.length() + 1],
                                  fragmentShaderString.c_str());

    // =========================================================
    // Compile the shaders
    // =========================================================
    unsigned int vertexID;
    unsigned int fragmentID;

    // create a unique id for the vertex shader
    vertexID = glCreateShader(GL_VERTEX_SHADER);

    // Load and compile the vertex shader
    glShaderSource(vertexID, 1, &vertexShader, nullptr);
    glCompileShader(vertexID);

    // Check for shader compile errors
    int success;
    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexID, 512, nullptr, infoLog);
        cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }
    // create a unique id for the fragment shader
    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

    // Load and compile the fragment shader
    glShaderSource(fragmentID, 1, &fragmentShader, nullptr);
    glCompileShader(fragmentID);

    // Check for shader compile errors
    glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentID, 512, nullptr, infoLog);
        cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // =========================================================
    // link the shaders
    // =========================================================
    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexID);
    glAttachShader(shaderProgramID, fragmentID);
    glLinkProgram(shaderProgramID);

    // Check for linking errors
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgramID, 512, nullptr, infoLog);
        cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }
    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
}

void uploadFloat(const string &varName, const float val) {
    int varLocation = glGetUniformLocation(shaderProgramID, varName.c_str());
    glUseProgram(shaderProgramID);
    glUniform1f(varLocation, val);
}

void uploadVec2(const string &varName, const float valX, const float valY) {
    int varLocation = glGetUniformLocation(shaderProgramID, varName.c_str());
    glUseProgram(shaderProgramID);
    glUniform2f(varLocation, valX, valY);
}

void initScene() {
    std::string defaultShaderVert = "../assets/shaders/defaultVert.glsl";
    std::string defaultShaderFrag = "../assets/shaders/defaultFrag.glsl";
    initShader(defaultShaderVert, defaultShaderFrag);

    // ============================================================
    // Generate VAO, VBO, and EBO buffer objects, and send to GPU
    // ============================================================
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glGenBuffers(1, &eboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elementArray), elementArray, GL_STATIC_DRAW);

    // Add the vertex Attribute pointers
    int posSize = 3;
    int vertexSizeBytes = posSize * sizeof(float);

    glVertexAttribPointer(0, posSize, GL_FLOAT, false, vertexSizeBytes, 0);
    glEnableVertexAttribArray(0);
}

void render() {
    glUseProgram(shaderProgramID);

    uploadVec2("uResolution", windowWidth, windowHeight);
    uploadFloat("uTime", glfwGetTime());


    glBindVertexArray(vaoID);

    glEnableVertexAttribArray(0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);

    glBindVertexArray(0);
    glUseProgram(0);
}

void gameLoop() {
    float beginTime = glfwGetTime();
    float endTime;
    float dt = -1;

    glClearColor(0, 0, 0, 1);     // Sets the background color of the screen

    while(!glfwWindowShouldClose(glfwWindow)) {

        glfwPollEvents();       // looks or events like a mouse or keyboard being pressed

        glClear(GL_COLOR_BUFFER_BIT);       // Sets the color of the screen to be updated

        if (dt >= 0) {
            render();
        }

        glfwSwapBuffers(glfwWindow);   // Render the new frame to the window

        endTime = glfwGetTime();        // Calculate the tine to render a frame (Framerate)
        dt = endTime - beginTime;
        beginTime = endTime;
    }
}

int main() {
    windowWidth = 1920;
    windowHeight = 1080;
    startWindow("RayMarching");
    initScene();
    gameLoop();
    return 0;
}
