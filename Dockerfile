FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    zlib1g-dev \
    libboost-dev \
    libgmp-dev \
    libgmpxx4ldbl \
    ninja-build \
    libboost-program-options-dev \
    libboost-random-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /divkc_build

COPY cppddnnf/ ./cppddnnf/
COPY splitter/ ./splitter/
COPY projection/ ./projection/
COPY D4/d4/ ./d4/
COPY D4/wrapper/ ./wrapper/

WORKDIR /divkc_build/cppddnnf
RUN g++ gen.cpp -o gen && \
    ./gen && \
    ninja clean && \
    ninja build/appmc build/sampler build/rsampler

WORKDIR /divkc_build/splitter
RUN g++ gen.cpp -o gen && \
    ./gen && \
    ninja clean && \
    ninja

WORKDIR /divkc_build/projection
RUN g++ gen.cpp -o gen && \
    ./gen && \
    ninja clean && \
    ninja

WORKDIR /divkc_build/wrapper
RUN make clean && make -j

WORKDIR /divkc_build/d4
RUN make clean && make -j

WORKDIR /divkc

RUN cp /divkc_build/cppddnnf/build/appmc ./ && \
    cp /divkc_build/cppddnnf/build/sampler ./ && \
    cp /divkc_build/cppddnnf/build/rsampler ./ && \
    cp /divkc_build/splitter/build/splitter ./ && \
    cp /divkc_build/projection/build/projection ./ && \
    cp /divkc_build/wrapper/wrap ./ && \
    cp /divkc_build/d4/d4 ./ && \
    rm -rf /divkc_build
