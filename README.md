# dino-robomongo 2.0.0

dino-robomongo is an effort to bring Robo 3T back as a fast, native, cross-platform MongoDB desktop client.

The original Robo 3T was valuable because it was small, direct, and shell-centric. This fork keeps that spirit, removes stale promotional surfaces, and starts modernizing the build so the app can run on current machines again.

## Why This Exists

Robo 3T still solves a real problem: many teams need a lightweight MongoDB UI that opens quickly, exposes the database clearly, and does not force a heavy commercial workflow.

The gap is compatibility. The original app embeds the MongoDB 4.2 shell/client. That is useful for older servers such as MongoDB 3.6, but it is not a complete answer for modern MongoDB 7 and 8.

This revival keeps the legacy path alive while preparing the project for a second, modern driver path.

## Current Status

Working now:

- Native macOS ARM64 build.
- Qt 5 UI running on Apple Silicon.
- Embedded MongoDB shell/client 4.2 compiled for ARM64.
- Legacy MongoDB connections, including older MongoDB 3.6 targets.
- Basic MongoDB 8 connectivity for currently tested flows.
- Driver status indicator in the connection test UI.
- Modern `mongocxx` compatibility probe wired into the main app.
- The old welcome/promotional tab was replaced by the dino-robomongo screen.

Known limitations:

- MongoDB 8 support is not complete yet.
- The current runtime still uses the legacy MongoDB 4.2 client.
- Some MongoDB 8 operations can still hit removed legacy commands or old protocol assumptions.
- Windows and Linux builds need their own dependency builds.

## Driver Strategy

The project is moving toward two MongoDB backends:

| Backend | Purpose | Status |
| --- | --- | --- |
| Legacy driver | Keep MongoDB 3.6 and older deployments usable | Active |
| Modern driver | Proper MongoDB 7/8 support | Compatibility probe active |

The app should choose the backend by server capability instead of forcing one driver to handle every MongoDB generation.

That matters because MongoDB 8 can accept basic connections from the old client, but compatibility is not the same as correctness. Commands like `getLastError` no longer exist on modern servers, and auth, TLS, URI handling, write concern, and newer server features need a current driver.

## Build Status

The verified development target is currently macOS ARM64.

The main app has been built as an ARM64 binary and launched successfully. The embedded MongoDB shell/client was rebuilt from the Robo 3T Mongo shell fork with ARM64 fixes for macOS.

Linux x64 should be the next most practical target. Windows x64 is possible, but it requires a separate MSVC/Qt/dependency build. Linux ARM64 is possible but will likely need architecture-specific dependency work similar to macOS ARM64.

## Build Outline

The build has two major stages:

1. Build the embedded Robo Mongo shell.
2. Build the Robo 3T Qt application against that shell and the local dependencies.

For macOS ARM64, the current build expects:

- Xcode command line tools.
- CMake.
- Python 3.
- Qt 5.
- OpenSSL.
- A compiled Robo Mongo shell 4.2 tree.

Example app build shape:

```sh
cmake -S . -B build/main-arm-v42 \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64

cmake --build build/main-arm-v42 --target robomongo -j8
```

The exact dependency paths depend on the local machine, so keep build configuration explicit rather than relying on global state.

## Testing MongoDB 8 Locally

A temporary MongoDB 8 server can be run with Docker:

```sh
docker run --rm -d \
  --name robo3t-mongo8-temp \
  -p 27018:27017 \
  public.ecr.aws/docker/library/mongo:8
```

Use this URI in dino-robomongo:

```text
mongodb://localhost:27018
```

The connection settings test now shows which driver family is being used. Today that is:

```text
Legacy driver 4.2
```

## Product Direction

The goal is not to turn dino-robomongo into a large database platform. The goal is to revive the compact native client:

- Fast startup.
- Clear database explorer.
- Direct shell workflow.
- Modern server compatibility.
- No promotional startup screen.
- Cross-platform builds.
- Small, maintainable feature modules.

The modernization should stay pragmatic. Keep the old backend where it is still valuable, add a modern backend where it is required, and avoid rewriting unrelated UI until it blocks compatibility or maintainability.

## Roadmap

Near term:

- Stabilize the macOS ARM64 build.
- Add more MongoDB 8 smoke tests.
- Replace more legacy write checks that depend on removed commands.
- Keep the connection UI honest about driver family and version.

Next:

- Introduce a modern MongoDB driver backend.
- Route connections by server version or wire protocol capability.
- Package macOS ARM64 builds.
- Bring up Linux x64.

Later:

- Bring up Windows x64.
- Evaluate Linux ARM64.
- Remove old network/promotional code that no longer belongs in the app.

## License

This project descends from Robo 3T and remains under the GNU General Public License version 3.

Original copyright belongs to the Robo 3T / 3T Software Labs contributors.
