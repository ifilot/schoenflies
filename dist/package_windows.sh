#!/bin/bash

# run this script from dist/win

echo "Getting requirements..."
wine ${CI_PROJECT_DIR}/build/schoenflies.exe 2> winedebug.log &
sleep 30
wineserver -k

echo "Copying requirements..."
python3 ${CI_PROJECT_DIR}/dist/copy_win_dlls.py winedebug.log
rm winedebug.log

echo "Digitally signing files..."
python3 ${CI_PROJECT_DIR}/dist/sign.py --cert "${CI_PROJECT_DIR}/cert.p12" --pass "${CERT_KEY_PASSWORD}"

echo "Creating zip file..."
zip -9 -r schoenflies-windows-${CI_COMMIT_TAG}.zip .

echo "Creating installer..."
cp ${CI_PROJECT_DIR}/dist/schoenflies.nsi .
makensis schoenflies.nsi

echo "Digitally signing installer..."
python3 ${CI_PROJECT_DIR}/dist/sign.py --cert "${CI_PROJECT_DIR}/cert.p12" --pass "${CERT_KEY_PASSWORD}" schoenflies-windows-installer-*.exe
