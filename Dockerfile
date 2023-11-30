# Container for Developing Zephyr Applications

# Build development environment
# $ docker build --tag west .

# Launch `west` CLI
# (be sure to place the binary in your local $(pwd) folder)
# $ docker run --device /dev/bus/usb --rm --tty \
#   --volume $(pwd):/host-volume/ west

# Global Argument(s)
ARG HOST_ARCH="x86_64"
ARG UID=1000
ARG USER="vscode"

# Base Image
FROM ubuntu:22.04

# Import Global Argument(s)
ARG HOST_ARCH
ARG UID
ARG USER
ARG ZEPHYR_REMOTE_URL
ARG ZEPHYR_RTOS_VESION

# Local Argument(s)
ARG DEBIAN_FRONTEND="noninteractive"
ARG ZEPHYR_RTOS_VESION=3.5.0
ARG ZEPHYR_REMOTE_URL="https://github.com/zephyrproject-rtos/zephyr"
ARG ZEPHYR_TOOLCHAIN_VERSION=0.16.4

# Environment Variables

# Create Non-Root User
RUN ["bash", "-c", "\
    addgroup \
     --gid ${UID} \
     \"${USER}\" \
 && adduser \
     --disabled-password \
     --gecos \"\" \
     --ingroup \"${USER}\" \
     --uid ${UID} \
     \"${USER}\" \
 && usermod \
     --append \
     --groups \"dialout,plugdev\" \
     \"${USER}\" \
"]

# Establish Environment
# https://docs.zephyrproject.org/latest/develop/getting_started/index.html#install-dependencies
RUN ["bash", "-c", "\
    apt-get update --quiet \
 && apt-get install --assume-yes --no-install-recommends --quiet \
     ccache \
     cmake \
     device-tree-compiler \
     dfu-util \
     file \
     gcc \
     git \
     gperf \
     libmagic1 \
     libsdl2-dev \
     make \
     nano \
     ninja-build \
     python3-dev \
     python3-pip \
     python3-setuptools \
     python3-tk \
     python3-wheel \
     tree \
     wget \
     xz-utils \
"]

# Tailor Environment to `amd64` or `arm64`
RUN ["bash", "-c", "\
    case \"${HOST_ARCH}\" in \
      aarch64) \
        apt-get install --assume-yes --no-install-recommends --quiet \
         g++ \
         g++-multilib-x86-64-linux-gnu \
         gcc-multilib-x86-64-linux-gnu \
        ;; \
      *) \
        apt-get install --assume-yes --no-install-recommends --quiet \
         g++-multilib \
         gcc-multilib \
        ;; \
    esac; \
"]

# Clean `apt-get` Cache
RUN ["bash", "-c", "\
    apt-get clean \
 && apt-get purge \
 && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* \
"]

# Update User and Path
USER ${USER}
ENV PATH="/home/${USER}/.local/bin:${PATH}"

# Install Zephyr and Tools
RUN ["bash", "-c", "\
    pip3 install --user -U west \
 && west init -m ${ZEPHYR_REMOTE_URL} --mr v${ZEPHYR_RTOS_VESION} ~/zephyrproject_v${ZEPHYR_RTOS_VESION} \
 && cd ~/zephyrproject_v${ZEPHYR_RTOS_VESION} \
 && west update \
 && west zephyr-export \
 && pip3 install --user -r ~/zephyrproject_v${ZEPHYR_RTOS_VESION}/zephyr/scripts/requirements.txt \
"]

# Update User and Working Directory
USER root
WORKDIR /usr/local/

# Install Zephyr SDK and Tools
RUN ["bash", "-c", "\
    wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_TOOLCHAIN_VERSION}/zephyr-sdk-${ZEPHYR_TOOLCHAIN_VERSION}_linux-${HOST_ARCH}.tar.xz \
 && wget -O - https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_TOOLCHAIN_VERSION}/sha256.sum | shasum --check --ignore-missing \
 && tar xvf zephyr-sdk-${ZEPHYR_TOOLCHAIN_VERSION}_linux-${HOST_ARCH}.tar.xz \
 && rm zephyr-sdk-${ZEPHYR_TOOLCHAIN_VERSION}_linux-${HOST_ARCH}.tar.xz \
 && cd zephyr-sdk-${ZEPHYR_TOOLCHAIN_VERSION} \
 && printf \"y\\nY\\n\" | ./setup.sh \
"]

# Update Path (gdb and openocd) and Environment Variables
ENV PATH="/usr/local/zephyr-sdk-${ZEPHYR_TOOLCHAIN_VERSION}/arm-zephyr-eabi/bin:${PATH}"
ENV PATH="/usr/local/zephyr-sdk-${ZEPHYR_TOOLCHAIN_VERSION}/sysroots/${HOST_ARCH}-pokysdk-linux/usr/bin:${PATH}"
ENV ZEPHYR_BASE="/home/${USER}/zephyrproject_v${ZEPHYR_RTOS_VESION}/zephyr"
WORKDIR /host-volume

ENTRYPOINT ["west"]
