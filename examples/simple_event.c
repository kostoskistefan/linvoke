/**
 * @file:      simple_event.c
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
    printf("ID of the port that emitted this event: %u\n", port_id);
}

int main(void)
{
    // Create a linvoke object
    linvoke_s *linvoke = linvoke_create();

    // Define unique ID for the port that are going to be registered inside linvoke
    const uint32_t port = 1358;

    // Register the port ID
    linvoke_register_port(linvoke, port);

    // Connect the port to the node
    linvoke_connect(linvoke, port, node_callback);

    // Emit the event from the port
    linvoke_emit(linvoke, port, NULL);

    // Destroy the linvoke object to free the used resources
    linvoke_destroy(linvoke);

    return 0;
}
