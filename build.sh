#!/bin/bash
set -e

echo "=== ATM Build Script ==="

# Step 1: Format check
echo ""
echo "--- Step 1: Checking code formatting ---"
FORMAT_DIFF=$(find src tests -name '*.h' -o -name '*.cpp' | xargs clang-format --dry-run --Werror 2>&1 || true)
if [ -n "$FORMAT_DIFF" ]; then
    echo "WARNING: Some files are not formatted. Run: find src tests -name '*.h' -o -name '*.cpp' | xargs clang-format -i"
else
    echo "All files formatted correctly."
fi

# Step 2: Build Debug
echo ""
echo "--- Step 2: Building Debug mode ---"
mkdir -p build/debug
cd build/debug
cmake ../.. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
cd ../..

# Step 3: Build Release
echo ""
echo "--- Step 3: Building Release mode ---"
mkdir -p build/release
cd build/release
cmake ../.. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cd ../..

# Step 4: Build documentation
echo ""
echo "--- Step 4: Building Doxygen documentation ---"
if command -v doxygen &> /dev/null; then
    doxygen Doxyfile
    echo "Documentation generated in docs/html/"
else
    echo "Doxygen not found, skipping documentation."
fi

# Step 5: Run tests with coverage
echo ""
echo "--- Step 5: Running unit tests with coverage ---"
cd build/debug
./atm_tests --gtest_output=xml:test_results.xml
echo ""

# Step 6: Generate coverage report
echo "--- Step 6: Generating coverage report ---"
lcov --capture --directory . --output-file coverage.info --no-external 2>/dev/null
lcov --remove coverage.info '*/tests/*' '*/gtest/*' --output-file coverage.info 2>/dev/null
genhtml coverage.info --output-directory ../../coverage 2>/dev/null
cd ../..

echo ""
echo "=== Build complete ==="
echo "  Debug binary:    build/debug/atm_app"
echo "  Release binary:  build/release/atm_app"
echo "  Test results:    build/debug/test_results.xml"
echo "  Coverage report: coverage/index.html"
echo "  Documentation:   docs/html/index.html"
