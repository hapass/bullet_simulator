#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <utility>
#include <chrono>
#include <numeric>
#include <algorithm>

using namespace std;
using namespace std::chrono;

struct float2 { float x; float y; };

struct BulletManager 
{
  BulletManager() 
  : walls { -1.0f, 1.0f, 1.0f, -1.0f, -0.8f, 0.5f, 0.2f, -0.1f, -0.3f, 0.5f, 0.2f, -0.5f, -0.6f, 0.6f, 0.2f, -0.4f }
  , bullets { 0.15, 0.25, -0.75, 0.85, 0.95, -0.35 }
  {}

  void Update(float time) 
  {
    bullets[0] += 0.001;
  }
  void Fire(float2 pos, float2 dir, float speed, float time, float life_time) {}

  float walls[16];
  float bullets[6];
};

int main(int argc, char** argv)
{
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

  GLuint buffers[2];
  glGenBuffers(2, buffers);
  GLuint arrays[2];
  glGenVertexArrays(2, arrays);

  BulletManager m;

  glBindVertexArray(arrays[0]);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m.walls), m.walls, GL_DYNAMIC_DRAW);

  glBindVertexArray(arrays[1]);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m.bullets), m.bullets, GL_DYNAMIC_DRAW);

  glPointSize(2.0);

  int64_t frame_counter = 0;
  auto start = steady_clock::now();

  while (!glfwWindowShouldClose(window))
  {
    m.Update(0.16f);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramId);

    // draw walls
    glBindVertexArray(arrays[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m.walls), m.walls, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, 8);

    // draw bullets
    glBindVertexArray(arrays[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m.bullets), m.bullets, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_POINTS, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();

    auto current_time = steady_clock::now();
    frame_counter++;
    if (duration_cast<milliseconds>(current_time - start).count() > 1000)
    {
      cout << "FPS: " << frame_counter << "\n";
      frame_counter = 0;
      start = current_time;
    }
  }

  glfwTerminate();
  return 0;
}