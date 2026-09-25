#!/usr/bin/env bash
set -euo pipefail

# ============================
# Configuration
# ============================
APP_NAME="Schoenflies"
APP_EXE="schoenflies.exe"
APP_VERSION="1.3.0"
BUILD_DIR="build"
DIST_ROOT="dist"
DIST_DIR="${DIST_ROOT}/${APP_NAME}"
BUILD_TYPE="Release"

# ============================
# Sanity checks
# ============================
command -v cmake >/dev/null || { echo "cmake not found"; exit 1; }
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
mkdir -p "${DIST_DIR}"
find "${DIST_DIR}" -mindepth 1 -delete

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
cp "${BUILD_DIR}/${APP_EXE}" "${DIST_DIR}/"

# ============================
# Locate windeployqt
# ============================
: "${MINGW_PREFIX:=/mingw64}"

if command -v windeployqt >/dev/null 2>&1; then
  WINDEPLOYQT="$(command -v windeployqt)"
elif [[ -x "$MINGW_PREFIX/bin/windeployqt-qt6.exe" ]]; then
  WINDEPLOYQT="$MINGW_PREFIX/bin/windeployqt-qt6.exe"
else
  echo "ERROR: Qt 6 windeployqt not found"
  exit 1
fi

: "${MINGW_PREFIX:=/mingw64}"

"$WINDEPLOYQT" "${DIST_DIR}/${APP_EXE}"

echo "[INFO] Bundling MinGW runtime DLLs"

# Follow dependencies recursively: windeployqt handles Qt plugins, while this
# loop also captures transitive MinGW libraries (for example ICU and Brotli).
while true; do
  dependency_list="$(mktemp)"
  find "${DIST_DIR}" -type f \( -iname '*.exe' -o -iname '*.dll' \) -print0 |
    while IFS= read -r -d '' binary; do
      ldd "$binary" 2>/dev/null || true
    done |
    awk -v prefix="${MINGW_PREFIX}/bin/" '$3 ~ "^" prefix && tolower($3) ~ /\.dll$/ { print $3 }' |
    sort -u > "${dependency_list}"

  copied=0
  while IFS= read -r path; do
    [[ -n "$path" ]] || continue
    dll="$(basename "$path")"
    if [[ ! -f "${DIST_DIR}/${dll}" ]]; then
      echo "  + $dll"
      cp "$path" "${DIST_DIR}/"
      copied=1
    fi
  done < "${dependency_list}"
  rm -f "${dependency_list}"

  [[ "$copied" -eq 1 ]] || break
done

echo "[INFO] Verifying deployed dependencies"
missing_dependencies="$(
  find "${DIST_DIR}" -type f \( -iname '*.exe' -o -iname '*.dll' \) -print0 |
    while IFS= read -r -d '' binary; do
      ldd "$binary" 2>/dev/null | awk -v binary="$binary" '/=> not found/ { print binary ": " $1 }'
    done
)"
if [[ -n "$missing_dependencies" ]]; then
  echo "ERROR: Missing runtime dependencies:"
  echo "$missing_dependencies"
  exit 1
fi

# ============================
# Build installer
# ============================
echo "[INFO] Building NSIS installer"
makensis installer.nsi

echo
echo "[SUCCESS] Windows package staged in ${DIST_DIR}"
echo "[SUCCESS] Windows installer created: ${APP_NAME}-${APP_VERSION}-Setup.exe"
