#!/bin/sh
set -eu
cd "$(dirname "$0")"
python3 -m third_branch.windows_doctor
exec python3 -m launcher "$@"
