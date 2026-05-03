#!/usr/bin/env bash
# Full local / CI pipeline (aligned with HandsOn_13_consol: format, cppcheck, CMake builds, ctest, Doxygen).
# Extra steps for this project: lcov coverage on src/, HTML report, 90% line threshold.

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

echo "===== Step 1: Check formatting (clang-format) ====="
find src tests \( -name '*.cpp' -o -name '*.h' \) -exec clang-format -i {} +
if ! git diff --quiet -- src tests; then
    echo "ERROR: C/C++ sources under src/ or tests/ need formatting:"
    git diff --name-only -- src tests
    git diff -- src tests
    exit 1
fi
echo "Formatting check passed."

echo ""
echo "===== Step 2: Configure Debug (compile_commands for cppcheck) ====="
cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo ""
echo "===== Step 3: Static analysis (cppcheck) ====="
cppcheck --project=build/debug/compile_commands.json \
    --enable=all \
    --suppress=missingIncludeSystem \
    --suppress=unusedFunction \
    --suppress=useStlAlgorithm \
    --suppress=constVariablePointer \
    --inline-suppr \
    --error-exitcode=1
echo "Static analysis passed."

echo ""
echo "===== Step 4: Build Debug ====="
cmake --build build/debug -j"$(nproc)"
echo "Debug build succeeded."

echo ""
echo "===== Step 5: Build Release ====="
cmake -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release -j"$(nproc)"
echo "Release build succeeded."

echo ""
echo "===== Step 6: Run unit tests (CTest / Google Test) ====="
ctest --test-dir build/debug --output-on-failure
echo "All tests passed."

echo ""
echo "===== Step 7: Coverage (lcov): capture, filter to src/, threshold 90% ====="
rm -f coverage.info
lcov --capture --directory build/debug --output-file coverage.info \
    --ignore-errors mismatch --ignore-errors inconsistent
lcov --extract coverage.info '*/src/*' --output-file coverage.info --ignore-errors unused
COVERAGE="$(lcov --summary coverage.info 2>&1 | grep 'lines' | sed 's/.*: //' | sed 's/%.*//')"
echo "Code coverage (lines, src/): ${COVERAGE}%"
if (( $(echo "${COVERAGE} < 90" | bc -l) )); then
    echo "ERROR: Code coverage ${COVERAGE}% is below 90% threshold"
    exit 1
fi
genhtml coverage.info --output-directory coverage
echo "Coverage HTML: coverage/index.html"

echo ""
echo "===== Step 8: Generate documentation (Doxygen) ====="
doxygen Doxyfile
echo "Documentation generated in docs/html/"

echo ""
echo "===== All steps completed successfully! ====="
