# Robo 3T ARM Port

## Current status

The legacy Robo 3T application still depends on the embedded `robomongo-shell`
MongoDB fork. That path is blocked for native ARM because it uses an old
Python 2/SCons build and links a fixed list of MongoDB shell objects, including
`mozjs-60` paths under `x86_64/macOS`.

The first ARM-native replacement path is in place:

- Qt remains the desktop UI layer.
- Modern MongoDB access starts moving to the Homebrew `mongo-cxx-driver`.
- Legacy MongoDB access uses `mongo-c-driver 1.19.2`, built locally for ARM.
- `robo3t_driver_probe` and `robo3t_legacy_driver_probe` build as native
  `arm64` executables.

Compatibility evidence:

- `mongo-cxx-driver 4.3.1` rejects `aspinator.local` because its server reports
  wire version 5 and the driver requires at least wire version 8.
- `mongo-c-driver 1.22.1` also rejects that server because it requires at least
  wire version 6.
- `mongo-c-driver 1.19.2` accepts wire version 5 and successfully pings
  `aspinator.local:27017`.

## Build

```bash
bin/build-arm-driver-probe
```

Run the probe:

```bash
build/arm-driver-probe/src/robo3t-modern/robo3t_driver_probe
```

Run a real MongoDB ping when a server is available:

```bash
build/arm-driver-probe/src/robo3t-modern/robo3t_driver_probe mongodb://localhost:27017 --ping
```

Run the legacy compatibility probe:

```bash
build/arm-driver-probe/src/robo3t-modern/robo3t_legacy_driver_probe \
  'mongodb://aspinator.local:27017/?serverSelectionTimeoutMS=5000'
```

## Next cut

Replace legacy `core/mongodb/MongoClient` operations incrementally with driver
adapters:

1. URI parsing and connection validation.
2. `ping`, `buildInfo`, and server version detection.
3. List databases and collections.
4. Query documents and render BSON through the existing views.
5. Create/update/delete operations.

Use a router behind the app-facing MongoDB interface:

- `mongo-c-driver 1.19.2` for MongoDB servers with wire version 5, including
  the `aspinator.local` target.
- `mongo-cxx-driver 4.3.1` for modern MongoDB servers.

Keep the old shell path isolated until equivalent driver-backed behavior exists.
