# Convenience wrapper for CMake (requires cmake; uses platform default generator).
.PHONY: all test bench clean
all:
	cmake -S . -B build
	cmake --build build --config Release

test: all
	ctest --test-dir build -C Release --output-on-failure

bench: all
	./build/benchmark 4 100000

clean:
	cmake -E rm -rf build
