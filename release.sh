#!/bin/bash

VERSION=$(grep FW_VERSION roxie.ino | cut -d' ' -f3 | tr -d '"')
RELEASE_DIR="roxie-$VERSION"
RELEASE_FILE="roxie-$VERSION.zip"

mkdir -p dist/

rm -f dist/${RELEASE_DIR}/* dist/${RELEASE_FILE}
mkdir -p dist/${RELEASE_DIR}/

echo "Copying files to dist/$RELEASE_DIR"
cp *.ino *.cpp *.h LICENSE README.md dist/${RELEASE_DIR}
mv dist/${RELEASE_DIR}/roxie.ino dist/${RELEASE_DIR}/roxie-${VERSION}.ino

echo "Creating dist/$RELEASE_FILE"
cd dist/
zip -r ${RELEASE_FILE} ${RELEASE_DIR}
