#include <mujoco/mujoco.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <cmath>
#include <GL/osmesa.h>

static int find_actuator_id(const mjModel *m, const char *name)
{
    if (!name)
        return -1;
    int id = mj_name2id(m, mjOBJ_ACTUATOR, name);
    return id;
}

static int find_joint_dofadr(const mjModel *m, const char *name)
{
    if (!name)
        return -1;
    int jnt_id = mj_name2id(m, mjOBJ_JOINT, name);
    if (jnt_id < 0)
        return -1;
    return m->jnt_dofadr[jnt_id];
}

int main(int argc, char **argv)
{
    // Prefer OSMesa backend for headless rendering
    putenv(const_cast<char *>("MUJOCO_GL=osmesa"));

    // Load model: argv[1] or default to Unitree G1 scene from menagerie
    const char *model_path = argc > 1 ? argv[1] : "models/menagerie/unitree_g1/scene.xml";
    char error[1024] = {0};
    mjModel *m = mj_loadXML(model_path, nullptr, error, sizeof(error));
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

    // Simple control: try actuator named "motor"; else apply torque to joint named "hinge"
    const int motor_id = find_actuator_id(m, "motor");
    const int hinge_dofadr = find_joint_dofadr(m, "hinge");

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
        // drive signal (sinusoid)
        double u = 0.5 * std::sin(2.0 * M_PI * (double)frame / 60.0);

        for (int k = 0; k < steps_per_frame; ++k)
        {
            if (motor_id >= 0)
            {
                d->ctrl[motor_id] = (mjtNum)u;
            }
            else if (hinge_dofadr >= 0)
            {
                // apply torque directly to hinge dof
                d->qfrc_applied[hinge_dofadr] = (mjtNum)u;
            }
            mj_step(m, d);
            // clear applied forces after step if used
            if (hinge_dofadr >= 0)
            {
                d->qfrc_applied[hinge_dofadr] = 0;
            }
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
