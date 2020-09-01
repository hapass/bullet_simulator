#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>
#include <iostream>

int main(int argc, char** argv)
{
  using namespace std;

  if (!glfwInit())
  {
    return -1;
  }

  // we don't need opengl function loader for OSX
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "Bullet manager", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  float vertices[] = {
    -0.5f, -0.5f,
    0.5f, -0.5f,
    0.0f, 0.5f,
  };

  // vertex shader
  GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  {
    const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);\n"
    "}\0";

    glShaderSource(vertexShaderId, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShaderId);

    GLint  success;
    char infoLog[512];
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);

    if(!success)
    {
      glGetShaderInfoLog(vertexShaderId, sizeof(infoLog), NULL, infoLog);
      std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
      glfwTerminate();
      return -1;
    }
  }

  // fragment shader
  GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  {
    const char* fragmentShaderSource = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

    glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShaderId);

    GLint success;
    char infoLog[512];
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

    if(!success)
    {
      glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
      std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
      glfwTerminate();
      return -1;
    }
  }

  // link shaders
  GLuint shaderProgramId = glCreateProgram();
  {
    glAttachShader(shaderProgramId, vertexShaderId);
    glAttachShader(shaderProgramId, fragmentShaderId);
    glLinkProgram(shaderProgramId);

    GLint success;
    char infoLog[512];
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
    if(!success)
    {
      glGetProgramInfoLog(shaderProgramId, sizeof(infoLog), NULL, infoLog);
      std::cerr << "Shader linking failed:\n" << infoLog << std::endl;
      glfwTerminate();
      return -1;
    }
  }

  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);

  // vertex buffer
  GLuint vertexBufferId;
  glGenBuffers(1, &vertexBufferId);

  GLuint vertexArrayId;
  glGenVertexArrays(1, &vertexArrayId);

  glBindVertexArray(vertexArrayId);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  while (!glfwWindowShouldClose(window))
  {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramId);
    glBindVertexArray(vertexArrayId);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}