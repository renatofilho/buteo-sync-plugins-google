#!/bin/sh

echo "running ${1}..."

export LD_LIBRARY_PATH=/usr/lib/buteo-plugins-qt5:$LD_LIBRARY_PATH

FILE=${1##*/}

cd /opt/tests/buteo-sync-plugins-google

./${1}
RESULT=$?

# Exit with the same code as the test binary
exit $RESULT
