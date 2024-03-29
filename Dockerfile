FROM gcc:10.2.0
LABEL version="0.1"

ARG USERNAEM=vscode
ARG USER_UID=1000
ARG USER_GID=${USER_UID}

SHELL ["/bin/bash", "-c"]
RUN chsh -s /bin/bash

RUN apt update && apt install -y clang-format git cmake gdb
ARG NUM_CORES=1
# google-test
WORKDIR /tmp
RUN git clone https://github.com/google/googletest.git -b release-1.10.0 && \
    mkdir -p googletest/_build && cd googletest/_build && \
    cmake ../ && make -j${NUM_CORES} && make install

WORKDIR /workspace

CMD ["/bin/bash"]