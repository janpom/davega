#!/bin/bash

VERSION=$(grep FW_VERSION davega.ino | cut -d' ' -f3 | tr -d '"')
RELEASE_FILEPATH="dist/davega-$VERSION.zip"
mkdir -p dist/
echo "Creating $RELEASE_FILEPATH"
rm -f ${RELEASE_FILEPATH}
zip ${RELEASE_FILEPATH} *.ino *.cpp *.h LICENSE README.md
