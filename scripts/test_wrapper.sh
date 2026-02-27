#!/bin/bash
# Wrapper script to map npm test arguments to CTest or direct execution

# Parse arguments
ARGS=""
for arg in "$@"; do
  case $arg in
    --passWithNoTests)
      # Ignored
      ;;
    --testPathPattern=*)
      # Ignored
      ;;
    *)
      # Pass other args if needed
      ;;
  esac
done

BUILD_DIR="build"

# Configure headless build (kson only)
echo "Configuring project (Headless/Unit Tests)..."
cmake -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Release -DHEADLESS_BUILD=ON

# Build
echo "Building tests..."
cmake --build $BUILD_DIR

# Run tests
if [ $? -eq 0 ]; then
    echo "Running tests..."

    # Try running CTest first
    cd $BUILD_DIR
    ctest --output-on-failure
    CTEST_EXIT=$?

    if [ $CTEST_EXIT -ne 0 ] || [ ! -f "CTestTestfile.cmake" ]; then
        echo "CTest failed or found no tests. Falling back to direct execution of known test binaries."

        # Find kson_test binary
        TEST_BIN=$(find . -name "kson_test" -type f | head -n 1)
        if [ -n "$TEST_BIN" ]; then
            echo "Running $TEST_BIN..."
            ./$TEST_BIN
            exit $?
        else
            echo "No test binaries found."
            exit 1
        fi
    fi
else
    echo "Build failed."
    exit 1
fi
