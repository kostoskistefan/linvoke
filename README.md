# Linvoke

[![License](https://img.shields.io/badge/license-MIT-blue.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)

**Linvoke** is a lightweight C library that provides a simple observer pattern implementation. It allows users to register ports, connect multiple nodes (callback functions) to those ports, and emit events from the ports with optional user data.

## Example projects

Linvoke comes with several example projects that show how to use Linvoke. They are located in the `examples` directory of this repository.

To compile the included example projects along with the Linvoke library, see step 2 in the [build instructions](#build-instructions).

| Project File                      | Description                                                                                                                     | Executable Name                                 |
| ---                               | ---                                                                                                                             | ---                                             |
| simple_event.c                    | Registers a port, connects a node to the port, and emits an event from the port.                                                | ./build/linvoke-simple-event                    |
| simple_event_with_data.c          | Same as simple_event.c, but connects custom user data to the node.                                                              | ./build/linvoke-simple-event-with-data          |
| simple_event_with_data_override.c | Same as simple_event_with_data.c, but overrides the connected user data when emitting the event.                                | ./build/linvoke-simple-event-with-data-override |
| multi_node_event.c                | Registers a port, connects multiple nodes to the port, and emits an event from the port.                                        | ./build/linvoke-multi-node-event                |
| multi_port_multi_node_event.c     | Registers multiple ports, connects multiple nodes to each port (with and without user data), and emits an event from each port. | ./build/linvoke-multi-port-multi-node-event     |

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
