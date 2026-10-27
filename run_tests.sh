
#!/usr/bin/env bash
# RaktSetu Mini OS - Test Runner

set -e
cd "$(dirname "$0")"

echo "===================================="
echo "     RAKTSETU TEST RUNNER"
echo "===================================="

echo "[1/2] Compiling blood tests..."
gcc -Wall -Wextra -std=c99 -Iinclude \
    tests/test_blood.c src/blood.c \
    -o test_blood

echo "[2/2] Compiling scheduler tests..."
gcc -Wall -Wextra -std=c99 -Iinclude \
    tests/test_scheduler.c \
    src/scheduler.c src/blood.c src/logger.c \
    -pthread -o test_scheduler

echo
echo "===================================="
echo "       RUNNING BLOOD TESTS"
echo "===================================="
./test_blood

echo
echo "===================================="
echo "      RUNNING SCHEDULER TESTS"
echo "===================================="
./test_scheduler

echo
echo "All test programs completed successfully."

