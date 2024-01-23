/**
 * @file:      multi_node_event.c
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
 * @brief Prints the name of the node callback function
 */
void node_callback1(linvoke_event_s *event)
{
    printf("Hello from node_callback1\n");
}

/**
 * @brief Prints the name of the node callback function
 */
void node_callback2(linvoke_event_s *event)
{
    printf("Hello from node_callback2\n");
}

int main(void)
{
    // Create a linvoke object
    linvoke_s *linvoke = linvoke_create();

    // Define unique ID for the port that are going to be registered inside linvoke
    const uint32_t port = 1358;

    // Register the port ID
    linvoke_register_port(linvoke, port);

    // Connect the port to the nodes
    // Order matters here. When the event is emitted from the port, 
    // the nodes will be called in the same order as they were connected
    linvoke_connect(linvoke, port, node_callback1);
    linvoke_connect(linvoke, port, node_callback2);

    // Emit the event from the port
    linvoke_emit(linvoke, port);

    // Destroy the linvoke object to free the used resources
    linvoke_destroy(linvoke);

    return 0;
}
