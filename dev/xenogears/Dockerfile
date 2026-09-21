FROM ubuntu:24.04

ARG UID=1000
ARG GID=1000
ENV DEBIAN_FRONTEND=noninteractive VIRTUAL_ENV=/opt/venv PATH=/opt/venv/bin:$PATH

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential gcc g++ cmake ninja-build make pkg-config git curl \
    python3 python3-pip python3-venv gdb binutils binutils-mips-linux-gnu cpp-mips-linux-gnu \
    xvfb xauth xdotool libsdl2-dev libopenal-dev libgl1-mesa-dev libglu1-mesa-dev libssl-dev \
    ca-certificates && rm -rf /var/lib/apt/lists/* && \
    (getent group "${GID}" >/dev/null || groupadd --gid "${GID}" blizz) && \
    (getent passwd "${UID}" >/dev/null || useradd --uid "${UID}" --gid "${GID}" --create-home blizz) && \
    python3 -m venv /opt/venv

COPY requirements.txt /tmp/requirements.txt
RUN /opt/venv/bin/pip install --no-cache-dir -r /tmp/requirements.txt

USER ${UID}:${GID}
WORKDIR /home/blizz/Projects/xenogears-decomp
