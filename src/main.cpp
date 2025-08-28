#include <mujoco/mujoco.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main(int argc, char **argv)
{
    // Minimal MJCF model (plane + free body)
    const char *xml = R"(<?xml version='1.0'?>
<mujoco model='minimal'>
  <option timestep='0.005'/>
  <worldbody>
    <geom name='ground' type='plane' size='0 0 1' rgba='0.8 0.9 0.8 1'/>
    <body name='box' pos='0 0 0.3'>
      <freejoint/>
      <geom type='box' size='0.05 0.05 0.05' rgba='0.9 0.2 0.2 1'/>
    </body>
  </worldbody>
</mujoco>
)";

    // Write XML to a temporary file for broad MuJoCo version compatibility
    const char *tmp_path = "/tmp/minimal.xml";
    {
        std::FILE *f = std::fopen(tmp_path, "wb");
        if (!f)
        {
            std::fprintf(stderr, "Failed to open %s for write\n", tmp_path);
            return 1;
        }
        std::fwrite(xml, 1, std::strlen(xml), f);
        std::fclose(f);
    }

    char error[1024] = {0};
    mjModel *m = mj_loadXML(tmp_path, nullptr, error, sizeof(error));
    if (!m)
    {
        std::fprintf(stderr, "Failed to load model: %s\n", error);
        return 1;
    }

    mjData *d = mj_makeData(m);
    if (!d)
    {
        std::fprintf(stderr, "Failed to make mjData\n");
        mj_deleteModel(m);
        return 1;
    }

    // Headless short simulation
    for (int steps = 0; steps < 1000; ++steps)
    {
        mj_step(m, d);
    }
    mj_deleteData(d);
    mj_deleteModel(m);
    return 0;
}
