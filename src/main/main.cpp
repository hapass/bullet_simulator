#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <utility>
#include <chrono>
#include <assert.h>

using namespace std;
using namespace std::chrono;

struct Vec2 { float x; float y; };

template<typename T>
class PointBuffer
{
public:
  const float* Data() const { return buffer.data(); }
  const size_t Size() const { return sizeof(float) * buffer.size(); }
  const size_t PointsCount() const { return buffer.size() / 2; }
protected:
  void Remove(size_t i, size_t elements_count, size_t floats_per_element)
  {
    assert(elements_count != 0 && i < elements_count);

    size_t item_offset = i * floats_per_element;
    size_t last_offset = (elements_count - 1) * floats_per_element;
    for (int j = 0; j < floats_per_element; j++) swap(buffer[item_offset + j], buffer[last_offset + j]);
    for (int j = 0; j < floats_per_element; j++) buffer.pop_back();
  }

  vector<T> buffer;
};

class WallsBuffer: public PointBuffer<float>
{
public:
  void Add(Vec2 start, Vec2 end)
  {
    buffer.push_back(start.x);
    buffer.push_back(start.y);
    buffer.push_back(end.x);
    buffer.push_back(end.y);
  }

  float& StartX(size_t i) { assert(WallsCount() != 0 && i < WallsCount()); return buffer[i * FLOATS_PER_WALL + 0]; }
  float& StartY(size_t i) { assert(WallsCount() != 0 && i < WallsCount()); return buffer[i * FLOATS_PER_WALL + 1]; }
  float& EndX(size_t i) { assert(WallsCount() != 0 && i < WallsCount()); return buffer[i * FLOATS_PER_WALL + 2]; }
  float& EndY(size_t i) { assert(WallsCount() != 0 && i < WallsCount()); return buffer[i * FLOATS_PER_WALL + 3]; }

  void Remove(size_t i) { PointBuffer::Remove(i, WallsCount(), FLOATS_PER_WALL); }

  const size_t WallsCount() const { return buffer.size() / FLOATS_PER_WALL; }
  static const size_t FLOATS_PER_WALL = 4;
};

class BulletsBuffer: public PointBuffer<float>
{
public:
  void Add(Vec2 pos)
  {
    buffer.push_back(pos.x);
    buffer.push_back(pos.y);
  }

  float& X(size_t i) { assert(BulletsCount() != 0 && i < BulletsCount()); return buffer[i * FLOATS_PER_BULLET + 0]; }
  float& Y(size_t i) { assert(BulletsCount() != 0 && i < BulletsCount()); return buffer[i * FLOATS_PER_BULLET + 1]; }

  void Remove(size_t i) { PointBuffer::Remove(i, PointsCount(), FLOATS_PER_BULLET); }

  const size_t BulletsCount() const { return PointsCount(); }
  static const size_t FLOATS_PER_BULLET = 2;
};

class BulletManager 
{
public:
  BulletManager() 
  {
    walls.Add({-1.0f, 1.0f}, {1.0f, -1.0f});
    walls.Add({-0.8f, 0.5f}, {0.2f, -0.1f});
    walls.Add({-0.3f, 0.5f}, {0.2f, -0.5f});
    walls.Add({-0.6f, 0.6f}, {0.2f, -0.4f});
  }

  void Update(float time) 
  {
    bullets.X(0) += 0.001;
  }

  void Fire(Vec2 pos, Vec2 dir, float speed, float time, float life_time) 
  {
    bullets.Add(pos);
  }

  const PointBuffer<float>& Walls() { return walls; }
  const PointBuffer<float>& Bullets() { return bullets; }

private:
  WallsBuffer walls;
  BulletsBuffer bullets;
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
  m.Fire({0.15, 0.25}, {0.0f, 0.0f}, 0.0f, 0.0f, 0.0f);
  m.Fire({-0.75, 0.85}, {0.0f, 0.0f}, 0.0f, 0.0f, 0.0f);
  m.Fire({0.95, -0.35}, {0.0f, 0.0f}, 0.0f, 0.0f, 0.0f);

  glBindVertexArray(arrays[0]);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glBufferData(GL_ARRAY_BUFFER, m.Walls().Size(), m.Walls().Data(), GL_DYNAMIC_DRAW);

  glBindVertexArray(arrays[1]);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glBufferData(GL_ARRAY_BUFFER, m.Bullets().Size(), m.Bullets().Data(), GL_DYNAMIC_DRAW);

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
    glBufferData(GL_ARRAY_BUFFER, m.Walls().Size(), m.Walls().Data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, m.Walls().PointsCount());

    // draw bullets
    glBindVertexArray(arrays[1]);
    glBufferData(GL_ARRAY_BUFFER, m.Bullets().Size(), m.Bullets().Data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_POINTS, 0, m.Bullets().PointsCount());

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