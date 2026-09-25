#!/usr/bin/env bash
set -euo pipefail

APP_NAME="Schoenflies"
APP_VERSION="${APP_VERSION:-1.3.0}"
BUILD_DIR="${BUILD_DIR:-build-macos}"
DIST_DIR="${DIST_DIR:-dist/macos}"
APP_BUNDLE="${BUILD_DIR}/${APP_NAME}.app"
STAGED_APP="${DIST_DIR}/${APP_NAME}.app"
DMG_PATH="${DIST_DIR}/${APP_NAME}-${APP_VERSION}-macOS-$(uname -m).dmg"
BUILD_TYPE="${BUILD_TYPE:-Release}"

for tool in cmake ninja macdeployqt hdiutil otool codesign; do
    command -v "${tool}" >/dev/null || {
        echo "ERROR: Required tool not found: ${tool}" >&2
        exit 1
    }
done

if [[ -z "${CMAKE_PREFIX_PATH:-}" ]]; then
    command -v brew >/dev/null || {
        echo "ERROR: Set CMAKE_PREFIX_PATH or install Homebrew." >&2
        exit 1
    }
    CMAKE_PREFIX_PATH="$(brew --prefix qt);$(brew --prefix eigen@3)"
fi

echo "[INFO] Configuring ${APP_NAME} ${APP_VERSION} for $(uname -m)"
cmake -S . -B "${BUILD_DIR}" -G Ninja \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}"

echo "[INFO] Building"
cmake --build "${BUILD_DIR}" --parallel

echo "[INFO] Testing"
ctest --test-dir "${BUILD_DIR}" --output-on-failure

[[ -d "${APP_BUNDLE}" ]] || {
    echo "ERROR: Application bundle was not produced: ${APP_BUNDLE}" >&2
    exit 1
}

echo "[INFO] Staging application bundle"
mkdir -p "${DIST_DIR}"
find "${DIST_DIR}" -mindepth 1 -maxdepth 1 -exec rm -rf {} +
cp -R "${APP_BUNDLE}" "${STAGED_APP}"

echo "[INFO] Deploying Qt frameworks and plugins"
macdeployqt "${STAGED_APP}" -always-overwrite -verbose=2

echo "[INFO] Applying an ad-hoc signature"
codesign --force --deep --sign - "${STAGED_APP}"
codesign --verify --deep --strict --verbose=2 "${STAGED_APP}"

echo "[INFO] Auditing dynamic-library references"
unexpected_references=""
while IFS= read -r -d '' binary; do
    if file "${binary}" | grep -q "Mach-O"; then
        binary_name="$(basename "${binary}")"
        leaked=""
        while IFS= read -r reference; do
            [[ -n "${reference}" ]] || continue
            # A dylib/framework may retain an absolute self-ID. It is metadata,
            # not a dependency; consumers are audited separately below.
            if [[ "$(basename "${reference}")" == "${binary_name}" ]]; then
                continue
            fi
            case "${reference}" in
                /opt/homebrew/*|/usr/local/*|/Users/runner/*)
                    leaked+="${reference}"$'\n'
                    ;;
            esac
        done < <(otool -L "${binary}" | tail -n +2 | awk '{print $1}')
        if [[ -n "${leaked}" ]]; then
            unexpected_references+="${binary}:"$'\n'"${leaked}"$'\n'
        fi
    fi
done < <(find "${STAGED_APP}" -type f -print0)

if [[ -n "${unexpected_references}" ]]; then
    echo "ERROR: The app bundle still references build-machine libraries:" >&2
    printf '%s' "${unexpected_references}" >&2
    exit 1
fi

echo "[INFO] Creating disk image"
DMG_ROOT="$(mktemp -d)"
trap 'rm -rf "${DMG_ROOT}"' EXIT
cp -R "${STAGED_APP}" "${DMG_ROOT}/"
ln -s /Applications "${DMG_ROOT}/Applications"
hdiutil create \
    -volname "${APP_NAME} ${APP_VERSION}" \
    -srcfolder "${DMG_ROOT}" \
    -ov \
    -format UDZO \
    "${DMG_PATH}"

echo "[SUCCESS] macOS application: ${STAGED_APP}"
echo "[SUCCESS] macOS disk image: ${DMG_PATH}"
