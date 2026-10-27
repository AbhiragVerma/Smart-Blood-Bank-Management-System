
#!/usr/bin/env bash
# RaktSetu Mini OS - Blood Inventory Report

set -e
cd "$(dirname "$0")"

echo "===================================="
echo "   RAKTSETU INVENTORY REPORT"
echo "===================================="

if [[ ! -x ./raktsetu ]]; then
    echo "Simulator not found. Building first..."
    ./build.sh
fi

./raktsetu <<'COMMANDS'
blood
exit
COMMANDS
