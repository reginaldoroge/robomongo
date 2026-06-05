# robomongo-shell v4.2 macOS ARM64 patch

The MongoDB shell source used by the legacy backend is downloaded under
`build/deps/robomongo-shell-v4.2`, which is outside the main repository index.

Run this from the main dino-robomongo repository after fetching the shell
source:

```bash
bin/patch-robomongo-shell-v42-macos-arm64
```

The patch makes the MongoDB shell 4.2 source compile on macOS ARM64 with C++17:

- fixes `ExclusiveData.h` move construction for current compilers;
- adjusts zlib's legacy `MACOS` branch so modern Apple builds use Unix paths;
- adds Darwin `aarch64` signal context handling to SpiderMonkey;
- creates `src/third_party/mozjs-60/platform/aarch64/macOS` from existing
  SpiderMonkey platform files and switches `JS_CODEGEN_ARM64` on.

The shell build command used for the macOS ARM64 release was:

```bash
python3 -m SCons mongo MONGO_VERSION=4.2.0 --release -j8 \
  --link-model=object --disable-warnings-as-errors --cxx-std=17 \
  CCFLAGS='-mmacosx-version-min=11.0 -D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION -Wno-enum-constexpr-conversion' \
  LINKFLAGS='-mmacosx-version-min=11.0'
```
