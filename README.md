# Linvoke

[![License](https://img.shields.io/badge/license-MIT-blue.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)

**Linvoke** is a lightweight C library that provides a simple observer pattern implementation. It allows users to register signals, connect multiple slots (callback functions) to those signals, and emit events with optional user data.

## Example projects

There are several example projects included in this repository that demonstrate how to use Linvoke. They are located in the `examples` directory.

To compile the included example projects along with the Linvoke library, see step 2 in the [build instructions](#build-instructions).

| Project File             | Description                                                                  | Executable Name                        |
| ---                      | ---                                                                          | ---                                    |
| simple_event.c           | Registers a signal, connects a slot to the signal and emits an event.        | ./build/linvoke-simple-event           |
| simple_event_with_data.c | Same as simple_event.c, but passes custom user data when emitting the event. | ./build/linvoke-simple-event-with-data |
| multi_slot_event.c       | Same as simple_event.c, but connects multiple slots to the signal.           | ./build/linvoke-multi-slot-event       |

## Build Instructions

### Requirements

 * C compiler (ex. GCC)
 * Meson

### Step-by-step guide

 1. Prepare the build direction: `meson setup build --prefix=/usr --libdir=/usr/lib`
 2. (Optional) Enable compilation of the included example project: `meson configure build -Dcompile_examples=true`
 3. Compile the library: `meson compile -C build`
 4. (Optional) Install the library on the system: `meson install -C build`

If you choose to skip step 4, the compiled library file will be located inside the folder called `build` in the root directory of this project, which can then be linked to your application.

## Testing

This library uses the [CMocka](https://cmocka.org/) unit testing framework to test its functionality. The tests can be run after completing steps 1 through 3 of the build instructions. Use the following command to run the tests: `meson test -C build`

## Contributing

Contributions are most welcome! Feel free to submit bug reports, feature requests, or pull requests.
