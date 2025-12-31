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

RUN cd /divkc_build/cppddnnf && \
    g++ gen.cpp -o gen && \
    ./gen && \
    ninja clean && \
    ninja build/appmc build/sampler build/rsampler && \
    cd ..

RUN cd /divkc_build/splitter && \
    g++ gen.cpp -o gen && \
    ./gen && \
    ninja clean && \
    ninja && \
    cd ..

RUN cd /divkc_build/projection && \
    g++ gen.cpp -o gen && \
    ./gen && \
    ninja clean && \
    ninja && \
    cd ..

RUN cd /divkc_build/wrapper && \
    make clean && make -j && cd ..

RUN cd /divkc_build/d4 && \
    make clean && make -j && cd ..

WORKDIR /divkc

RUN cp /divkc_build/cppddnnf/build/appmc ./ && \
    cp /divkc_build/cppddnnf/build/sampler ./ && \
    cp /divkc_build/cppddnnf/build/rsampler ./ && \
    cp /divkc_build/splitter/build/splitter ./ && \
    cp /divkc_build/projection/build/projection ./ && \
    cp /divkc_build/wrapper/wrap ./ && \
    cp /divkc_build/d4/d4 ./ && \
    rm -rf /divkc_build
