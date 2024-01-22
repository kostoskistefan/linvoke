# Linvoke

<a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/license-MIT-blue.svg?style=for-the-badge"/ height="25px"></a>

**Linvoke** is a lightweight C library that provides a simple observer pattern implementation. It allows users to register custom events, connect multiple callbacks (nodes) to these events, and emit events with optional user data.

## Example usage

An example **_hello world_** project can be found in the `examples` directory. The following features are covered in the example:

 * Registering custom events
 * Connecting one event to one node
 * Connecting one node to multiple event
 * Connecting callback nodes with and without passing user data
 * Emitting events

To build the example project, see step 2 in the [build instructions](#build-instructions).
To run the example project, run the following command: `./build/linvoke-hello-world`

## Build Instructions

### Requirements

 * C compiler (ex. GCC)
 * Meson

### Step-by-step guide

 1. Prepare the build direction: `meson setup build --prefix=/usr --libdir=/usr/lib`
 2. (Optional) Enable compilation of the included example project: `meson configure build -Dcompile_examples=true`
 3. Compile the library: `meson compile -C build`
 4. (Optional) Install the library on the system: `meson install -C build`

If you chose to skip step 4, you'll find the library inside a folder called `build` in the root directory of this project, which can then be linked to your application.

## Contributing

Contributions are most welcome! Feel free to submit bug reports, feature requests, or pull requests.