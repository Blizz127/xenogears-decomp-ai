FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt update; apt install -y git make binutils-mips-linux-gnu cpp-mips-linux-gnu python3 python3-pip python3-venv

COPY requirements.txt /tmp/requirements.txt
RUN python3 -m venv /.venv && . /.venv/bin/activate && python3 -m pip install -r /tmp/requirements.txt
# NB: Don't rename this. Gears depends on this to figure out where the project files
# are located. See find_base_path() in tools/gears/src/file_system.rs
WORKDIR /xenogears-decomp
