#!/bin/bash

# run this script from dist/deb

mkdir schoenflies_${CI_COMMIT_TAG}_amd64
cd schoenflies_${CI_COMMIT_TAG}_amd64

mkdir -p DEBIAN usr/bin usr/share/applications usr/share/doc/schoenflies

cp ${CI_PROJECT_DIR}/build/schoenflies usr/bin/schoenflies
cp ${CI_PROJECT_DIR}/dist/schoenflies.desktop usr/share/applications/schoenflies.desktop
cp ${CI_PROJECT_DIR}/dist/copyright usr/share/doc/schoenflies/copyright

for size in {16,22,32,48,64,128,256}; do
    mkdir -p usr/share/icons/hicolor/${size}x${size}/apps
    inkscape -w ${size} -h ${size} ${CI_PROJECT_DIR}/assets/icons/schoenflies.svg -o usr/share/icons/hicolor/${size}x${size}/apps/schoenflies.png > /dev/null 2>&1
done

cp ${CI_PROJECT_DIR}/dist/control DEBIAN/control
sed -i "s/{VERSION}/${CI_COMMIT_TAG}/g" DEBIAN/control
sed -i "s/{SIZE}/$(du -s . | cut -f1)/g" DEBIAN/control

cp ${CI_PROJECT_DIR}/dist/copyright DEBIAN/copyright

cd ..
dpkg-deb --build --root-owner-group schoenflies_${CI_COMMIT_TAG}_amd64
