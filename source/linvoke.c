/**
 * @file:      linvoke.c
 *
 * @date:      22 January 2024
 *
 * @author:    Kostoski Stefan
 *
 * @copyright: Copyright (c) 2024 Kostoski Stefan.
 *             This work is licensed under the terms of the MIT license.
 *             For a copy, see <https://opensource.org/license/MIT>.
 */

#include "../include/linvoke.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef LINVOKE_PORT_ARRAY_BLOCK_SIZE
#define LINVOKE_PORT_ARRAY_BLOCK_SIZE 8
#endif

#ifndef LINVOKE_NODE_ARRAY_BLOCK_SIZE
#define LINVOKE_NODE_ARRAY_BLOCK_SIZE 8
#endif

/**
 * @struct linvoke_event_s
 * @brief Structure that holds the data for an event
 * @var port_id The ID of the port that emitted the event
 * @var user_data The user data that was passed when the event was emitted
 */
struct linvoke_event_s
{
    uint32_t port_id;
    void *user_data;
};

/**
 * @struct linvoke_port_s
 * @brief Structure that holds information about a port
 * @var id The ID of the port
 * @var nodes An array of pointers to nodes that are connected to the port
 * @var connected_node_count The number of nodes that are connected to the port
 * @var node_capacity The maximum capacity of the nodes array
 */
struct linvoke_port_s
{
    uint32_t id;
    linvoke_node_pointer *nodes;
    uint32_t connected_node_count;
    uint32_t node_capacity;
};

/**
 * @struct linvoke_s
 * @brief Structure that holds information about a linvoke object
 * @var ports An array holding the registered ports
 * @var registered_port_count The number of ports that are registered within the linvoke object
 * @var port_capacity The maximum capacity of the ports array
 */
struct linvoke_s
{
    linvoke_port_s *ports;
    uint32_t registered_port_count;
    uint32_t port_capacity;
};

/**
 * @brief Finds a port with a given ID if it exists
 * @param linvoke Pointer to a linvoke object
 * @param port_id The ID of the port to find
 * @return A pointer to the port with the given ID or NULL if no such port was found
 */
linvoke_port_s *linvoke_find_port(linvoke_s *const linvoke, uint32_t port_id);

linvoke_s *linvoke_create(void)
{
    linvoke_s *linvoke = malloc(sizeof(*linvoke));

    if (linvoke == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for the linvoke object.\n");
        return NULL;
    }

    linvoke->ports = malloc(LINVOKE_PORT_ARRAY_BLOCK_SIZE * sizeof(*linvoke->ports));

    if (linvoke->ports == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for the linvoke ports.\n");
        free(linvoke);
        return NULL;
    }

    linvoke->registered_port_count = 0;
    linvoke->port_capacity = LINVOKE_PORT_ARRAY_BLOCK_SIZE;

    return linvoke;
}

void linvoke_destroy(linvoke_s *const linvoke)
{
    for (uint32_t i = 0; i < linvoke->registered_port_count; ++i)
    {
        free(linvoke->ports[i].nodes);
    }

    free(linvoke->ports);
    free(linvoke);
}

void linvoke_register_port(linvoke_s *const linvoke, uint32_t port_id)
{
    // Check if there is an existing port with the same ID
    if (linvoke_find_port(linvoke, port_id) != NULL)
    {
        fprintf(stderr, "A port with id %u already exists.\n", port_id);
        return;
    }

    // Reallocate the ports array memory if the capacity is full
    if (linvoke->registered_port_count == linvoke->port_capacity)
    {
        linvoke->port_capacity += LINVOKE_PORT_ARRAY_BLOCK_SIZE;
        linvoke_port_s *reallocated_ports = realloc(linvoke->ports, linvoke->port_capacity * sizeof(*linvoke->ports));

        if (reallocated_ports == NULL)
        {
            fprintf(stderr, "Failed to reallocate memory for the ports array.\n");
            return;
        }

        linvoke->ports = reallocated_ports;
    }

    // Register the new port
    linvoke_port_s *const port = &linvoke->ports[linvoke->registered_port_count];
    port->id = port_id;
    port->connected_node_count = 0;
    port->node_capacity = LINVOKE_NODE_ARRAY_BLOCK_SIZE;
    port->nodes = malloc(LINVOKE_NODE_ARRAY_BLOCK_SIZE * sizeof(*port->nodes));

    if (port->nodes == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for the port nodes.\n");
        return;
    }

    ++linvoke->registered_port_count;
}

void linvoke_connect(linvoke_s *const linvoke, const uint32_t port_id, linvoke_node_pointer node)
{
    // Find the port with the given ID
    linvoke_port_s *const port = linvoke_find_port(linvoke, port_id);

    // Port not found
    if (port == NULL)
    {
        fprintf(stderr, "A port with id %u does not exist.\n", port_id);
        return;
    }

    // Check if the callback is already connected
    for (uint32_t j = 0; j < port->connected_node_count; ++j)
    {
        if (port->nodes[j] == node)
        {
            fprintf(stderr, "The callback function is already connected to port %d\n", port_id);
            return;
        }
    }

    // Reallocate the nodes array memory if the capacity is full
    if (port->connected_node_count == port->node_capacity)
    {
        port->node_capacity += LINVOKE_NODE_ARRAY_BLOCK_SIZE;

        linvoke_node_pointer *reallocated_nodes = realloc(port->nodes, port->node_capacity * sizeof(*port->nodes));

        if (reallocated_nodes == NULL)
        {
            fprintf(stderr, "Failed to reallocate memory for the nodes array.\n");
            return;
        }

        port->nodes = reallocated_nodes;
    }

    // Connect the node
    port->nodes[port->connected_node_count] = node;

    ++port->connected_node_count;
}

void linvoke_emit(linvoke_s *const linvoke, const uint32_t port_id, void *user_data)
{
    // Find the port with the given ID
    linvoke_port_s *const port = linvoke_find_port(linvoke, port_id);

    // Port not found
    if (port == NULL)
    {
        fprintf(stderr, "A port with id %u does not exist.\n", port_id);
        return;
    }

    linvoke_event_s event = { .port_id = port_id, .user_data = user_data };

    // Call the callback function for all nodes connected to the port and override the user data
    for (size_t j = 0; j < port->connected_node_count; ++j)
    {
        port->nodes[j](&event);
    }
}

linvoke_port_s *linvoke_find_port(linvoke_s *const linvoke, uint32_t port_id)
{
    for (uint32_t i = 0; i < linvoke->registered_port_count; ++i)
    {
        if (linvoke->ports[i].id == port_id)
        {
            return &linvoke->ports[i];
        }
    }

    return NULL;
}

uint32_t linvoke_get_registered_port_count(linvoke_s *const linvoke)
{
    return linvoke->registered_port_count;
}

uint32_t linvoke_get_node_count(linvoke_s *const linvoke, const uint32_t port_id)
{
    // Find the port with the given ID
    linvoke_port_s *const port = linvoke_find_port(linvoke, port_id);

    // Port not found
    if (port == NULL)
    {
        fprintf(stderr, "Port with id %u not found.\n", port_id);
        return 0;
    }

    return port->connected_node_count;
}

uint32_t linvoke_event_get_port_id(linvoke_event_s *const event)
{
    return event->port_id;
}

void *linvoke_event_get_user_data(linvoke_event_s *const event)
{
    return event->user_data;
}
