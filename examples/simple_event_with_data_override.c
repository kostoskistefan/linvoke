/**
 * @file:      simple_event_with_data_override.c
 *
 * @date:      22 January 2024
 *
 * @author:    Kostoski Stefan
 *
 * @copyright: Copyright (c) 2024 Kostoski Stefan.
 *             This work is licensed under the terms of the MIT license.
 *             For a copy, see <https://opensource.org/license/MIT>.
 */

#include <stdio.h>
#include <linvoke.h>

/**
 * @brief Prints the ID of the port that emitted the event
 */
void node_callback(linvoke_event_s *event)
{
    const uint32_t port_id = linvoke_event_get_port_id(event);
    const char *data = *((const char **) linvoke_event_get_user_data(event));
    printf("Port ID: %u\tData: %s\n", port_id, data);
}

int main(void)
{
    // Create a linvoke object
    linvoke_s *linvoke = linvoke_create();

    // Define unique ID for the port that are going to be registered inside linvoke
    const uint32_t port = 123;

    // Register the port ID
    linvoke_register_port(linvoke, port);

    // Connect the port to the node and add some user data
    const char *string_data = "Hello, World!";
    linvoke_connect_with_data(linvoke, port, node_callback, &string_data);

    // Emit the event from the port and pass that will override the already connected user data
    const char *other_string_data = "Hello from the overriden data!";
    linvoke_emit_with_data(linvoke, port, &other_string_data);

    // Destroy the linvoke object to free the used resources
    linvoke_destroy(linvoke);

    return 0;
}
