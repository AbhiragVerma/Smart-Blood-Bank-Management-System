
#!/usr/bin/env bash

# RaktSetu Mini OS - Build Script
# Builds the existing C simulator using the Makefile.

set -e

echo "=================================="
echo "   RaktSetu Simulator Build"
echo "=================================="

# Ensure the script is run from the project root.
cd "$(dirname "$0")"

# Check that the Makefile exists.
if [[ ! -f makefile && ! -f Makefile ]]; then
    echo "Error: Makefile not found."
    exit 1
fi

# Build the simulator.
make -f makefile

echo "Build completed successfully."
