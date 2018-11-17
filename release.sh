#!/bin/bash

VERSION=$(grep FW_VERSION davega.ino | cut -d' ' -f3 | tr -d '"')
RELEASE_DIR="davega-$VERSION"
RELEASE_FILE="davega-$VERSION.zip"

mkdir -p dist/

rm -f dist/${RELEASE_DIR}/* dist/${RELEASE_FILE}
mkdir -p dist/${RELEASE_DIR}/

echo "Copying files to dist/$RELEASE_DIR"
cp *.ino *.cpp *.h LICENSE README.md dist/${RELEASE_DIR}
mv dist/${RELEASE_DIR}/davega.ino dist/${RELEASE_DIR}/davega-${VERSION}.ino

echo "Creating dist/$RELEASE_FILE"
cd dist/
zip -r ${RELEASE_FILE} ${RELEASE_DIR}
