#!/usr/bin/env bash
set -euo pipefail

# ============================
# Configuration
# ============================
APP_NAME="Schoenflies"
APP_EXE="schoenflies.exe"
BUILD_DIR="build"
DIST_ROOT="dist"
DIST_DIR="${DIST_ROOT}/${APP_NAME}"
BUILD_TYPE="Release"

# ============================
# Sanity checks
# ============================
command -v cmake >/dev/null || { echo "cmake not found"; exit 1; }
: "${MINGW_PREFIX:=/mingw64}"
WINDEPLOYQT="${MINGW_PREFIX}/bin/windeployqt.exe"

if [[ ! -x "$WINDEPLOYQT" ]]; then
  echo "windeployqt not found at $WINDEPLOYQT"
  exit 1
fi
command -v makensis >/dev/null || { echo "makensis not found"; exit 1; }

if [[ -z "${MINGW_PREFIX:-}" ]]; then
  echo "MINGW_PREFIX not set (are you in a MinGW shell?)"
  exit 1
fi

echo "[INFO] Using MINGW_PREFIX=${MINGW_PREFIX}"

# ============================
# Clean
# ============================
echo "[INFO] Cleaning previous output"
rm -rf "${DIST_ROOT}"

# ============================
# Configure
# ============================
echo "[INFO] Configuring (${BUILD_TYPE})"
cmake -S . -B "${BUILD_DIR}" \
  -G Ninja \
  -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

# ============================
# Build
# ============================
echo "[INFO] Building"
cmake --build "${BUILD_DIR}"

# ============================
# Stage files
# ============================
echo "[INFO] Staging files"
mkdir -p "${DIST_DIR}"
cp "${BUILD_DIR}/${APP_EXE}" "${DIST_DIR}/"

# ============================
# Deploy Qt
# ============================
echo "[INFO] Running windeployqt"

"${WINDEPLOYQT}" \
  --no-angle \
  --no-opengl-sw \
  "${DIST_DIR}/${APP_EXE}"

echo "[INFO] Bundling MinGW runtime DLLs"

ldd "${DIST_DIR}/${APP_EXE}" | awk '{print $3}' | while read -r path; do
  case "$path" in
    /mingw64/bin/*.dll)
      dll="$(basename "$path")"
      echo "  + $dll"
      cp -n "$path" "${DIST_DIR}/"
      ;;
  esac
done

# ============================
# Build installer
# ============================
echo "[INFO] Building NSIS installer"
makensis installer.nsi

echo
echo "[SUCCESS] Windows installer created"
