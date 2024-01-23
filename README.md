# Linvoke

[![License](https://img.shields.io/badge/license-MIT-blue.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)

**Linvoke** is a lightweight C library that provides a simple observer pattern implementation. It allows users to register ports, connect multiple nodes (callback functions) to those ports, and emit events from the ports with optional user data.

## Example Projects

Example projects are included in this repository and can be found in the `examples` directory.

To compile the included example projects, see step 2 in the [build instructions](#build-instructions).

#### simple_event.c

The following features are covered in this example:

 * Register a port
 * Connect the port to a node
 * Emit an event from the port

To run this project, run the following command after compilation: `./build/linvoke-simple-event`

#### simple_event_with_data.c

The following features are covered in this example:

 * Register a port
 * Connect the port to a node with custom user data provided
 * Emit an event from the port

To run this project, run the following command after compilation: `./build/linvoke-simple-event-with-data`

#### simple_event_with_data_override.c

The following features are covered in this example:

 * Register a port
 * Connect the port to a node with custom user data provided
 * Emit an event from the port and override the connected user data with new data

To run this project, run the following command after compilation: `./build/linvoke-simple-event-with-data-override`

#### multi_node_event.c

The following features are covered in this example:

 * Register a port
 * Connect the port to multiple nodes
 * Emit an event from the port that will trigger all connected nodes

To run this example project, run the following command after compilation: `./build/linvoke-multi-node-event`

#### multi_port_multi_node_event.c

The following features are covered in this example:

 * Register a port
 * Connect multiple ports to multiple nodes
 * Emit an events from the registered ports that will trigger all connected nodes

To run this example project, run the following command after compilation: `./build/linvoke-multi-port-multi-node-event`

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

## Testing

This library uses the [CMocka](https://cmocka.org/) unit testing framework to test its functionality. The tests can be run after completing steps 1 through 3 of the build instructions. Use the following command to run the tests: `meson test -C build`

## Contributing

Contributions are most welcome! Feel free to submit bug reports, feature requests, or pull requests.
