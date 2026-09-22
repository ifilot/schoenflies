# Schoenflies

/ˈʃøːnfliːs/

Determine and visualise molecular symmetry.

Versions ≤ 1.1: [original implementation](https://gitlab.com/lkkmpn/schoenflies) by Luuk Kempen (2021)  
Versions ≥ 1.2: maintained as a port by Ivo Filot

## Compilation

### Ubuntu / Debian

Install the required packages

```bash
sudo apt install build-essential cmake qtbase5-dev qtbase5-dev-tools libboost-all-dev libeigen3-dev libfreetype-dev libglm-dev nlohmann-json3-dev
```

and then compile using

```
cd build
cmake ..
make -j
```

and test via

```bash
make test
```

### Windows / MSYS2 MinGW64

Install the required packages

```bash
pacman -S --needed \
  mingw-w64-x86_64-gcc \
  mingw-w64-x86_64-cmake \
  mingw-w64-x86_64-make \
  mingw-w64-x86_64-qt5-base \
  mingw-w64-x86_64-boost \
  mingw-w64-x86_64-eigen3 \
  mingw-w64-x86_64-glm \
  mingw-w64-x86_64-freetype \
  mingw-w64-x86_64-nlohmann-json \
  mingw-w64-x86_64-mesa \
  mingw-w64-x86_64-ninja
```

and then compile using

```bash
cd build
cmake ..
make -j
```

and test via

```bash
cmake --build . --target test
```

## Funding

Development of this project is partially funded by
[the BOOST! program of Eindhoven University of Technology.](https://boost.tue.nl)
