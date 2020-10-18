#!/bin/bash
set -eo

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd ${DIR}/../src/models/${1}/
${DIR}/../tools/objn64/objn64 -f ${1}.obj -s 100
