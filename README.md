# dino-robomongo 2.0.0

dino-robomongo is a revival of Robo 3T focused on keeping the original app fast, native, and shell-centric while making it useful again with both legacy and modern MongoDB servers.

The main goal of this fork is compatibility without turning the app into a heavy database platform. Older MongoDB installations still matter, but modern MongoDB 7/8 servers require a newer driver path. This project now carries both ideas forward.

## What Changed

- Rebranded the app as **dino-robomongo 2.0.0**.
- Replaced the old promotional welcome screen with a project revival screen.
- Added a native macOS ARM64 build path.
- Rebuilt the embedded Robo Mongo shell/client 4.2 for current macOS ARM machines.
- Added a modern `mongocxx` driver path for compatibility probing and modern-server operations.
- Kept the legacy MongoDB 4.2 client path for older deployments.
- Added driver family/version feedback in the connection test UI.
- Added database, collection, and index size refresh in the explorer.
- Added result export from query output to JSON, JSON Lines, and CSV.
- Added export scope controls, including visible results, a custom export limit, and all query results.
- Added a new splash screen and refreshed app icons.

## Dual Driver Compatibility

dino-robomongo is designed to support two MongoDB generations:

| Driver path | Target servers | Purpose |
| --- | --- | --- |
| Legacy MongoDB 4.2 client | MongoDB 3.6 and legacy deployments | Preserve compatibility with old databases and shell behavior |
| Modern `mongocxx` driver | MongoDB 7/8 and newer deployments | Avoid removed legacy commands and support current server behavior |

This matters because a modern MongoDB server may still accept a basic connection from the old client, but that does not make the old driver fully compatible. Newer MongoDB versions removed commands such as `getLastError`, changed expectations around server discovery, and require modern handling for several operations.

The app now exposes driver information instead of hiding this detail. When testing a connection, the UI shows whether the active path is legacy or modern and reports the driver family/version.

## MongoDB Compatibility Goals

Current practical targets:

- MongoDB 3.6 through the legacy path.
- MongoDB 4.x through the existing embedded shell/client.
- MongoDB 7/8 through the modern-driver modernization work.

The long-term rule is simple: keep old MongoDB support working, but do not force the legacy driver to pretend it can correctly handle modern servers.

## New Features

### Driver Status in Connection Test

Connection testing now reports which driver family is active. This is important while the project supports both old and new MongoDB server generations.

### Explorer Sizes

The toolbar includes a **Sizes** action that refreshes storage sizes in the left explorer tree.

It can show:

- database size
- collection size
- index size

Sizes are refreshed only when requested, so large databases are not scanned constantly while browsing.

### Result Export

Query results can be exported directly from the result header.

Supported formats:

- JSON
- JSON Lines
- CSV

Supported scopes:

- visible results only
- query results with a custom export limit
- all results from the query

The export path is separated from the visible result page, so exporting a larger set does not change the currently displayed page.

### Revival UI

The previous promotional startup tab was removed. The new first screen explains the dino-robomongo revival and the driver compatibility direction.

### Splash and Icons

The app now uses dino-robomongo branding for the splash screen and application icons.

## Build Status

The verified development target is currently:

- macOS ARM64
- Windows x64
- Qt 5
- embedded MongoDB shell/client 4.2
- modern `mongocxx`/`bsoncxx` libraries available locally

The project has been built and launched successfully on Apple Silicon and Windows x64.

Linux builds are still a planned target. The codebase keeps cross-platform support in mind, but each platform needs its own dependency build and packaging work.

Qt WebEngine was removed from the revived welcome/dashboard path. That makes Windows packaging simpler because the app no longer needs Chromium helper processes or WebEngine runtime DLLs just to render the startup tab.

## Windows x64 Build and Release Notes

The Windows build required a full bring-up of the legacy Robo 3T stack on a modern Windows toolchain. This was not a simple rebuild: the original project depends on an embedded MongoDB shell/client, older C++ code, Qt 5, OpenSSL, and native MongoDB driver libraries that all need to line up at link time and runtime.

The current Windows package was built with:

- Windows x64
- Visual Studio Build Tools 2022
- Qt 5.12.8 for MSVC x64
- OpenSSL from vcpkg
- MongoDB C driver and C++ driver from vcpkg
- embedded Robo Mongo shell/client 4.2 objects
- Inno Setup for the final installer

The final installer is:

```text
dino-robomongo-2.0.0-windows-x64-setup.exe
SHA256: c986546d284b9d0215add1fda78328fa5d82233ad60796e8683d0eda706129e0
```

VirusTotal report:

```text
https://www.virustotal.com/gui/file/c986546d284b9d0215add1fda78328fa5d82233ad60796e8683d0eda706129e0?nocache=1
```

At the time of publishing, the installer had one generic machine-learning detection out of 71 security vendors. The file is an unsigned Inno Setup executable containing the application, Qt runtime DLLs, OpenSSL runtime DLLs, MongoDB driver runtime DLLs, and Microsoft Visual C++ runtime DLLs. New unsigned Windows installers can trigger reputation-based or heuristic detections even when the remaining vendors report the file as clean.

The installer creates:

- the application installation directory
- a Start Menu shortcut
- a Desktop shortcut
- an uninstaller entry

For public redistribution, the preferred next hardening step is code signing: sign both `dino-robomongo.exe` and the generated installer, then republish the Windows asset.

## Cross-Platform Build Strategy

The project should stay on one main codebase. Windows work should happen in short-lived bring-up branches, then merge back after the platform-specific fixes are guarded.

Use these rules when adding platform code:

- Prefer CMake platform checks for build wiring: `if(WIN32)`, `if(APPLE)`, `if(UNIX AND NOT APPLE)`.
- Use `#ifdef _WIN32` only around code that truly has to call Windows APIs or use Windows paths.
- `_WIN32` is defined for both 32-bit and 64-bit Windows. This project targets Windows x64 only.
- Use `_WIN64` only when the code relies on a 64-bit Windows ABI detail.
- Keep shared logic outside platform guards and isolate OS-specific code in small functions/files.

The top-level CMake file now fails early for 32-bit Windows generators. A Windows build should use a 64-bit Visual Studio generator/toolchain.

## Build Outline

The build has two main parts:

1. Build or provide the embedded Robo Mongo shell/client 4.2.
2. Build the Qt desktop application against the shell, Qt, OpenSSL, and MongoDB C/C++ driver dependencies.

Example macOS ARM64 shape:

```sh
cmake -S . -B build/main-arm-v42 \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64

cmake --build build/main-arm-v42 --target robomongo -j8
```

Expected Windows x64 shape:

```bat
cmake -S . -B build\windows-x64 ^
  -G "Visual Studio 17 2022" ^
  -A x64 ^
  -DCMAKE_BUILD_TYPE=Release

cmake --build build\windows-x64 --config Release --target robomongo
```

The Windows build needs its local dependency set wired first: Qt 5 for MSVC x64, OpenSSL, the legacy Robo Mongo shell/client 4.2 objects, and the modern MongoDB C/C++ driver libraries.

Once the application is built, install the runtime layout and build the Windows installer:

```bat
cmake --install build\windows-x64 --config Release
iscc installer\windows\dino-robomongo.iss
```

Unit tests can be built and run with:

```sh
cmake --build build/main-arm-v42 --target robo_unit_tests -j8
build/main-arm-v42/src/robomongo-unit-tests/robo_unit_tests
```

## Testing MongoDB 8 Locally

A temporary MongoDB 8 server can be started with Docker:

```sh
docker run --rm -d \
  --name robo3t-mongo8-temp \
  -p 27018:27017 \
  public.ecr.aws/docker/library/mongo:8
```

Then connect using:

```text
mongodb://localhost:27018
```

## Roadmap

Near term:

- Expand modern-driver coverage for find, insert, update, remove, and database commands.
- Keep replacing legacy checks that depend on removed MongoDB commands.
- Add more MongoDB 8 smoke tests.
- Keep driver selection and driver status visible to users.

Next:

- Package macOS ARM64 builds.
- Bring up Linux x64.
- Add Windows x64 CI/build packaging.

Later:

- Improve automatic routing between legacy and modern driver paths.
- Reduce old code paths that only exist for removed promotional or update flows.
- Keep the UI small, direct, and suitable for repeated operational work.

## Design Principles

- Keep the app native and fast.
- Preserve the direct shell workflow.
- Support old MongoDB deployments without blocking modern compatibility.
- Prefer explicit driver behavior over hidden fallback logic.
- Add focused features without turning the app into a large platform.

## License

This project descends from Robo 3T and remains under the GNU General Public License version 3.

Original copyright belongs to the Robo 3T / 3T Software Labs contributors.

## Support

This revival took a lot of build, compatibility, and packaging work, especially to keep the old MongoDB shell path alive while adding a modern MongoDB driver path and a working Windows installer.

If this project helps you, please consider buying me a coffee through PayPal:

```text
reginaldoroge@gmail.com
```
