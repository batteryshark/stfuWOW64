# stfu_wow64

A minimal `wow64log.dll` shim that stops 32-bit processes from starting on
64-bit Windows. When WOW64 initializes a 32-bit process, it looks for a DLL
named `wow64log.dll` and calls its logging entrypoints. This project supplies
that DLL, and its `DllMain` calls `ExitProcess` the moment the loader maps it,
so the host process never finishes starting.

Originally a 2019 Windows internals spike, kept as a small, documented reference.

> [!WARNING]
> Installed system-wide, this blocks every 32-bit application from launching.
> Run it only in a disposable lab environment where that is the point. Loader
> behavior varies across Windows versions and patch levels.

## How it works

1. A 32-bit process starts on 64-bit Windows.
2. WOW64 initialization reaches its logging-DLL boundary and loads `wow64log.dll`.
3. The shim exports the four symbols WOW64 expects, so it satisfies the dependency.
4. `DllMain` calls `ExitProcess(0)` during load, terminating the process.

The exported stubs (`Wow64LogInitialize`, `Wow64LogMessageArgList`,
`Wow64LogSystemService`, `Wow64LogTerminate`) each return `STATUS_SUCCESS`. They
exist to match the expected export names; in practice the process exits in
`DllMain` before any of them run.

Terminating a process from `DllMain` is deliberately abnormal. It works here
because that termination *is* the behavior. Do not copy the pattern into ordinary
DLL code.

Constraints that matter:

- The output name must be exactly `wow64log.dll`.
- The DLL must be 64-bit (PE32+). A 32-bit build will not satisfy a 64-bit loader.
- Replacing the `ExitProcess` call with a normal `return TRUE` turns the blocker
  into a no-op logger stub.

The shim does not hook processes, patch system binaries, install a driver, or
persist itself. The only boundary it crosses is the normal DLL load path for
`wow64log.dll`.

## Build

Requires Windows on x64, CMake 3.15+, and a 64-bit Windows C compiler
(MSVC, clang-cl, or MinGW-w64).

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The DLL lands in the CMake `bin` directory.

Two options tune the MSVC build:

- `STFU_WOW64_MSVC_NO_C_RUNTIME` (default `ON`) bypasses the CRT startup and
  enters directly at `DllMain`. The shim uses no CRT services, and fewer runtime
  dependencies keep a loader-boundary experiment easy to reason about. With this
  on, the import table contains only `KERNEL32.dll!ExitProcess`. Set it `OFF` to
  build through the normal MSVC DLL runtime startup.
- `STFU_WOW64_STATIC_RUNTIME` (default `ON`) prefers a static C runtime when a
  runtime is used. Set it `OFF` to link the runtime dynamically.

## Validate

Confirm the DLL is PE32+ and exports the WOW64 logging symbols:

```powershell
llvm-objdump -p build\bin\wow64log.dll
```

Expected exports: `Wow64LogInitialize`, `Wow64LogMessageArgList`,
`Wow64LogSystemService`, `Wow64LogTerminate`.

## Layout

- `src/stfuwow64.c` — the exported stubs and the `DllMain` termination.
- `CMakeLists.txt` — builds the shim as `wow64log.dll`.
- `LICENSE.md` — MIT.

## License

MIT. See [LICENSE.md](LICENSE.md).
