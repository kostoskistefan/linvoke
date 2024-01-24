/**
 * @file:      simple_event_with_data.c
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
 * @brief Prints the ID of the port that emitted the event, as well as the user data that was passed
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
    linvoke_connect(linvoke, port, node_callback);

    // Emit the event from the port
    const char *string_data = "Hello, World!";
    linvoke_emit(linvoke, port, &string_data);

    // Destroy the linvoke object to free the used resources
    linvoke_destroy(linvoke);

    return 0;
}
