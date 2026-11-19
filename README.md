# Safe Memory Reclamation Starter

This starter implements a **leaky** Harris–Michael ordered linked list, plus a placeholder reclamation interface. It does **not** yet implement EBR or hazard pointers.

## Windows (Visual Studio / VS Code with MSVC)
Open the folder in VS Code with the CMake Tools extension and Visual Studio C++ Build Tools installed, or open the folder directly in Visual Studio 2022.

From a **Developer PowerShell for VS 2022**:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
.\build\Debug\benchmark.exe 4 100000
```

If using a single-config Ninja generator, the executable may instead be `build/benchmark.exe`.

## Linux

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/benchmark 4 100000
```

## Caveats
- Removed nodes are intentionally never freed; this is the leaky baseline.
- `find()` is currently only safe under the leaky reclaimer. Do **not** plug in EBR/HP without adapting traversal protection and validation.
- This starter is not a verified production-grade lock-free implementation; test thoroughly under contention.
- Run tests in Debug builds (C/C++ `assert` checks can be disabled in Release).
- The Makefile is a convenience wrapper for Unix-like shells; use CMake commands directly on Windows.
