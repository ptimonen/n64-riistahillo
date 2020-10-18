#!/bin/bash
set -eu

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd ${DIR}/../tools/cen64
./cen64-debian9-x86_64 pifdata.bin ${DIR}/../riistahillo.n64
