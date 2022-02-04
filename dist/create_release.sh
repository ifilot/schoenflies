#!/bin/bash

# run this script from /

PACKAGE_REGISTRY_URL=${CI_API_V4_URL}/projects/${CI_PROJECT_ID}/packages/generic/schoenflies

WIN_INST_FILE=$(find dist/win -iname schoenflies-windows-installer-*.exe -printf "%f")
WIN_ZIP_FILE=$(find dist/win -iname schoenflies-windows-*.zip -printf "%f")
DEB_FILE=$(find dist/deb -iname schoenflies_*.deb -printf "%f")

echo "Uploading to package registry..."
curl --header "Job-Token: ${CI_JOB_TOKEN}" --upload-file dist/win/${WIN_INST_FILE} ${PACKAGE_REGISTRY_URL}/${CI_COMMIT_TAG}/${WIN_INST_FILE}
curl --header "Job-Token: ${CI_JOB_TOKEN}" --upload-file dist/win/${WIN_ZIP_FILE} ${PACKAGE_REGISTRY_URL}/${CI_COMMIT_TAG}/${WIN_ZIP_FILE}
curl -w "\n" --header "Job-Token: ${CI_JOB_TOKEN}" --upload-file dist/deb/${DEB_FILE} ${PACKAGE_REGISTRY_URL}/${CI_COMMIT_TAG}/${DEB_FILE}

echo "Parsing changelog..."
python3 dist/parse_changelog.py ${CI_COMMIT_TAG} > description.txt

echo "Creating release..."
./release-cli create --name "Schoenflies v${CI_COMMIT_TAG}" --description description.txt --tag-name ${CI_COMMIT_TAG} \
    --assets-link "{\"name\": \"${DEB_FILE}\", \"url\": \"${PACKAGE_REGISTRY_URL}/${CI_COMMIT_TAG}/${DEB_FILE}\", \"filepath\": \"/${DEB_FILE}\", \"link_type\": \"package\"}" \
    --assets-link "{\"name\": \"${WIN_INST_FILE}\", \"url\": \"${PACKAGE_REGISTRY_URL}/${CI_COMMIT_TAG}/${WIN_INST_FILE}\", \"filepath\": \"/${WIN_INST_FILE}\", \"link_type\": \"package\"}" \
    --assets-link "{\"name\": \"${WIN_ZIP_FILE}\", \"url\": \"${PACKAGE_REGISTRY_URL}/${CI_COMMIT_TAG}/${WIN_ZIP_FILE}\", \"filepath\": \"/${WIN_ZIP_FILE}\", \"link_type\": \"package\"}" \
