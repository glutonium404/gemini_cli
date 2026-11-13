# Gemini CLI

A small C++ command-line client for the Google Gemini API.

## Requirements

- CMake 3.14 or newer
- A C++20 compiler
- OpenSSL development libraries
- An API key with access to the Gemini API

## Configuration

Set the required API key:

```bash
export GEMINI_API_KEY="your-api-key"
```

You can provide one or more fallback models with `:` separators:

```bash
export GEMINI_CLI_MODELS="gemini-3-flash-preview:gemini-2.5-flash"
```

Alternatively, select a model for one invocation with `--model`.

## Build

### Linux

Install OpenSSL development files, then configure and build:

```bash
sudo apt install libssl-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### Windows

The project can be built with MSVC or MinGW. When using MinGW, install a Windows-targeted OpenSSL package and configure CMake with the repository's toolchain file:

```bash
cmake -S . -B build-mingw \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic \
  -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="$PWD/cmake/toolchains/mingw-w64-x86_64.cmake" \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build-mingw --parallel
```

## Usage

```bash
gemini_cli "Explain what a compiler does"
echo "Summarize this context" | gemini_cli
```

Useful options:

```text
-h, --help          Show help
-m, --markdown      Allow Markdown output
-l, --long         Allow longer responses
-a, --ansi          Enable terminal formatting
-v, --verbose       Show models being tried
-q, --quiet         Suppress intermediate request errors
-t, --timeout N     Set request timeout in seconds (5-60)
--model MODEL       Use a specific model
```

When multiple models are configured, the CLI tries them in order until one succeeds. Use `--` to treat following arguments as prompt text rather than options.
