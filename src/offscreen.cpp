#include <mujoco/mujoco.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <GL/osmesa.h>

int main(int, char **)
{
    // Prefer OSMesa backend for headless rendering
    putenv(const_cast<char *>("MUJOCO_GL=osmesa"));

    const char *xml = R"(<?xml version='1.0'?>
<mujoco model='offscreen'>
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

    const char *tmp_path = "/tmp/offscreen.xml";
    if (FILE *f = std::fopen(tmp_path, "wb"))
    {
        std::fwrite(xml, 1, std::strlen(xml), f);
        std::fclose(f);
    }
    else
    {
        std::fprintf(stderr, "Failed to write %s\n", tmp_path);
        return 1;
    }

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

    // Create MuJoCo visualization context and offscreen OpenGL context
    mjvScene scn;
    mjv_defaultScene(&scn);
    mjvCamera cam;
    mjv_defaultCamera(&cam);
    mjvOption opt;
    mjv_defaultOption(&opt);
    mjrContext con;
    mjr_defaultContext(&con);

    // initialize scene
    mjv_makeScene(m, &scn, 1000);

    // offscreen buffer size
    int width = 640, height = 480;

    // Create and make current an OSMesa context so an OpenGL platform exists
    OSMesaContext osmesa = OSMesaCreateContextExt(OSMESA_RGBA, 24, 8, 0, nullptr);
    if (!osmesa)
    {
        std::fprintf(stderr, "OSMesaCreateContextExt failed\n");
        mjv_freeScene(&scn);
        mj_deleteData(d);
        mj_deleteModel(m);
        return 1;
    }
    std::vector<unsigned char> osmesa_fb(4 * width * height);
    if (!OSMesaMakeCurrent(osmesa, osmesa_fb.data(), GL_UNSIGNED_BYTE, width, height))
    {
        std::fprintf(stderr, "OSMesaMakeCurrent failed\n");
        OSMesaDestroyContext(osmesa);
        mjv_freeScene(&scn);
        mj_deleteData(d);
        mj_deleteModel(m);
        return 1;
    }

    // make MuJoCo rendering context (uses current GL platform)
    mjr_makeContext(m, &con, mjFONTSCALE_150);
    mjrRect viewport = {0, 0, width, height};
    mjr_setBuffer(mjFB_OFFSCREEN, &con);
    mjr_resizeOffscreen(width, height, &con);

    // allocate pixel buffer (RGB)
    std::vector<unsigned char> rgb(3 * width * height);

    // run simulation and capture N frames
    const int steps_per_frame = 10; // adjust motion speed
    const int num_frames = 300;     // ~10 seconds @30fps

    // open ffmpeg pipe
    std::string ffmpeg_cmd = "ffmpeg -y -f rawvideo -pixel_format rgb24 -video_size " +
                             std::to_string(width) + "x" + std::to_string(height) +
                             " -framerate 30 -i - -pix_fmt yuv420p -vf vflip output.mp4";
    FILE *ff = popen(ffmpeg_cmd.c_str(), "w");
    if (!ff)
    {
        std::fprintf(stderr, "Failed to start ffmpeg\n");
        mjr_freeContext(&con);
        mjv_freeScene(&scn);
        mj_deleteData(d);
        mj_deleteModel(m);
        return 1;
    }

    for (int frame = 0; frame < num_frames; ++frame)
    {
        for (int k = 0; k < steps_per_frame; ++k)
        {
            mj_step(m, d);
        }
        // update scene and render into offscreen buffer
        mjv_updateScene(m, d, &opt, nullptr, &cam, mjCAT_ALL, &scn);
        mjr_render(viewport, &scn, &con);

        // read pixels (origin bottom-left)
        mjr_readPixels(rgb.data(), nullptr, viewport, &con);

        // write to ffmpeg
        size_t wrote = std::fwrite(rgb.data(), 1, rgb.size(), ff);
        if (wrote != rgb.size())
        {
            std::fprintf(stderr, "Short write to ffmpeg\n");
            break;
        }
    }

    pclose(ff);
    mjr_freeContext(&con);
    OSMesaDestroyContext(osmesa);
    mjv_freeScene(&scn);
    mj_deleteData(d);
    mj_deleteModel(m);
    std::fprintf(stdout, "Saved video to output.mp4\n");
    return 0;
}
