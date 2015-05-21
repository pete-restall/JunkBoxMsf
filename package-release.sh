#!/bin/sh
#
# Run this script from the project root.
#
RELEASE_DIR=/tmp/JunkBoxMsf-Release

if [ -d "${RELEASE_DIR}" ]; then
	echo "Release directory ${RELEASE_DIR} already exists.";
	exit 1;
fi;

mkdir -p "${RELEASE_DIR}/Hardware";
cp "Schematics/Plots/JunkBoxMsf - Project.pdf" "${RELEASE_DIR}/Hardware/JunkBoxMsf - Schematics.pdf";
cp Schematics/JunkBoxMsf\ \(*.{txt,csv} "${RELEASE_DIR}/Hardware";
cp "Schematics/Plots/JunkBoxMsf - PCB.pdf" "${RELEASE_DIR}/Hardware";
cp Schematics/Plots/*.drl "${RELEASE_DIR}/Hardware";

mkdir -p "${RELEASE_DIR}/Firmware";
cp Firmware/JunkBoxMsf.X/dist/default/production/* "${RELEASE_DIR}/Firmware";

touch "${RELEASE_DIR}/RELEASE.txt";
