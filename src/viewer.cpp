#include <mujoco/mujoco.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstring>

static void error_callback(int error, const char *description)
{
    std::fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

int main(int, char **)
{
    const char *xml = R"(<?xml version='1.0'?>
<mujoco model='viewer'>
  <option timestep='0.005'/>
  <worldbody>
    <geom name='ground' type='plane' size='0 0 1' rgba='0.8 0.9 0.8 1'/>
    <body name='box' pos='0 0 0.3'>
      <freejoint/>
      <geom type='box' size='0.05 0.05 0.05' rgba='0.2 0.4 0.9 1'/>
    </body>
  </worldbody>
</mujoco>
)";

    // Write to temp file for compatibility
    const char *tmp_path = "/tmp/viewer.xml";
    std::FILE *f = std::fopen(tmp_path, "wb");
    if (!f)
    {
        std::fprintf(stderr, "Failed to open %s\n", tmp_path);
        return 1;
    }
    std::fwrite(xml, 1, std::strlen(xml), f);
    std::fclose(f);

    char error[1024] = {0};
    mjModel *m = mj_loadXML(tmp_path, nullptr, error, sizeof(error));
    if (!m)
    {
        std::fprintf(stderr, "Model error: %s\n", error);
        return 1;
    }
    mjData *d = mj_makeData(m);
    if (!d)
    {
        std::fprintf(stderr, "Data error\n");
        mj_deleteModel(m);
        return 1;
    }

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        std::fprintf(stderr, "GLFW init failed\n");
        return 1;
    }
    // Request an OpenGL 2.1 context compatible with XQuartz GLX
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    GLFWwindow *win = glfwCreateWindow(800, 600, "MuJoCo Viewer", nullptr, nullptr);
    if (!win)
    {
        std::fprintf(stderr, "GLFW window failed\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    // Simple loop: step physics, clear screen, swap
    while (!glfwWindowShouldClose(win))
    {
        mj_step(m, d);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    mj_deleteData(d);
    mj_deleteModel(m);
    return 0;
}
