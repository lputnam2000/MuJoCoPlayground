FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive \
    MUJOCO_VERSION=3.3.5 \
    MUJOCO_PREFIX=/opt/mujoco

# Base toolchain
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    ca-certificates tzdata wget git \
    build-essential clang gdb make ninja-build cmake \
    python3 python3-pip python-is-python3 \
    && rm -rf /var/lib/apt/lists/*

# Runtime deps for MuJoCo + offscreen rendering
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    libgl1-mesa-dev \
    libosmesa6 libosmesa6-dev \
    libx11-dev libxrandr-dev libxi-dev libxinerama-dev libxcursor-dev libxxf86vm-dev \
    ffmpeg \
    && rm -rf /var/lib/apt/lists/*

# Install MuJoCo prebuilt binaries (Linux AArch64)
RUN wget -q https://github.com/google-deepmind/mujoco/releases/download/${MUJOCO_VERSION}/mujoco-${MUJOCO_VERSION}-linux-aarch64.tar.gz -O /tmp/mujoco.tar.gz \
    && mkdir -p ${MUJOCO_PREFIX} \
    && tar -xzf /tmp/mujoco.tar.gz -C ${MUJOCO_PREFIX} --strip-components=1 \
    && rm -f /tmp/mujoco.tar.gz \
    && cp -r ${MUJOCO_PREFIX}/include/mujoco /usr/local/include/ \
    && cp -P ${MUJOCO_PREFIX}/lib/*.so* /usr/local/lib/ \
    && echo "/usr/local/lib" > /etc/ld.so.conf.d/usr-local.conf \
    && ldconfig \
    && test -f /usr/local/lib/libmujoco.so

WORKDIR /work