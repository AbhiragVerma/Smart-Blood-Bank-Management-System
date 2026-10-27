
#!/usr/bin/env bash
# RaktSetu Mini OS - Demonstration Script

set -e
cd "$(dirname "$0")"

if [[ ! -x ./raktsetu ]]; then
    echo "Simulator not found. Building first..."
    ./build.sh
fi

echo "===================================="
echo "   RAKTSETU BLOOD BANK OS DEMO"
echo "===================================="

./raktsetu <<'COMMANDS'
help
blood
request DemoPatient O+ 2 1
ps
logs
exit
COMMANDS
