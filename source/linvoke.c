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
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef LINVOKE_PORT_CAPACITY
#define LINVOKE_PORT_CAPACITY 8
#endif

#ifndef LINVOKE_NODE_CAPACITY
#define LINVOKE_NODE_CAPACITY 16
#endif

struct linvoke_event_s
{
    uint32_t port_id;
    void *user_data;
};

struct linvoke_node_s
{
    void *user_data;
    void (*callback)(linvoke_event_s *event);
};

struct linvoke_port_s
{
    uint32_t id;
    linvoke_node_s *nodes;
    uint32_t number_of_connected_nodes;
    uint32_t node_capacity;
};

struct linvoke_s
{
    linvoke_port_s *ports;
    uint32_t number_of_registered_ports;
    uint32_t port_capacity;
};

linvoke_s *linvoke_create(void)
{
    linvoke_s *linvoke = calloc(1, sizeof(*linvoke));

    linvoke->ports = calloc(LINVOKE_PORT_CAPACITY, sizeof(*linvoke->ports));
    linvoke->number_of_registered_ports = 0;
    linvoke->port_capacity = LINVOKE_PORT_CAPACITY;

    return linvoke;
}

void linvoke_destroy(linvoke_s *const linvoke)
{
    for (uint32_t i = 0; i < linvoke->number_of_registered_ports; ++i)
    {
        free(linvoke->ports[i].nodes);
    }

    free(linvoke->ports);
    free(linvoke);
}

void linvoke_register_port(linvoke_s *const linvoke, uint32_t port_id)
{
    for (uint32_t i = 0; i < linvoke->number_of_registered_ports; ++i)
    {
        if (linvoke->ports[i].id == port_id)
        {
            fprintf(stderr, "An port with id %u already exists.\n", port_id);
            return;
        }
    }

    if (linvoke->number_of_registered_ports == linvoke->port_capacity)
    {
        linvoke->port_capacity += LINVOKE_PORT_CAPACITY;
        linvoke->ports = realloc(linvoke->ports, linvoke->port_capacity * sizeof(*linvoke->ports));
    }

    linvoke_port_s *const port = &linvoke->ports[linvoke->number_of_registered_ports];
    port->id = port_id;
    port->nodes = calloc(LINVOKE_NODE_CAPACITY, sizeof(*port->nodes));
    port->number_of_connected_nodes = 0;
    port->node_capacity = LINVOKE_NODE_CAPACITY;

    ++linvoke->number_of_registered_ports;
}

void linvoke_connect(
    linvoke_s *const linvoke,
    const uint32_t port_id,
    void (*const callback)(linvoke_event_s *event),
    void *const user_data
)
{
    for (uint32_t i = 0; i < linvoke->number_of_registered_ports; ++i)
    {
        if (linvoke->ports[i].id != port_id)
        {
            continue;
        }

        linvoke_port_s *const port = &linvoke->ports[i];

        for (uint32_t j = 0; j < port->number_of_connected_nodes; ++j)
        {
            if (port->nodes[j].callback != callback)
            {
                continue;
            }

            fprintf(stderr, "A callback with the same address already exists.\n");
            return;
        }

        if (port->number_of_connected_nodes == port->node_capacity)
        {
            port->node_capacity += LINVOKE_NODE_CAPACITY;
            port->nodes = realloc(port->nodes, port->node_capacity * sizeof(*port->nodes));
        }

        linvoke_node_s *const node = &port->nodes[port->number_of_connected_nodes];
        node->callback = callback;
        node->user_data = user_data;

        ++port->number_of_connected_nodes;

        return;
    }

    fprintf(stderr, "An port with id %u does not exist.\n", port_id);
}

void linvoke_emit(linvoke_s *const linvoke, const uint32_t port_id)
{
    for (uint32_t i = 0; i < linvoke->number_of_registered_ports; ++i)
    {
        if (linvoke->ports[i].id != port_id)
        {
            continue;
        }

        linvoke_port_s *const port = &linvoke->ports[i];

        for (size_t j = 0; j < port->number_of_connected_nodes; ++j)
        {
            linvoke_node_s *const node = &port->nodes[j];
            linvoke_event_s event = { .port_id = port_id, .user_data = node->user_data };
            node->callback(&event);
        }

        return;
    }

    fprintf(stderr, "An port with id %u does not exist.\n", port_id);
}

uint32_t linvoke_get_registered_port_count(linvoke_s *const linvoke)
{
    return linvoke->number_of_registered_ports;
}

uint32_t linvoke_get_node_count(linvoke_s *const linvoke, const uint32_t port_id)
{
    for (uint32_t i = 0; i < linvoke->number_of_registered_ports; ++i)
    {
        if (linvoke->ports[i].id != port_id)
        {
            continue;
        }

        return linvoke->ports[i].number_of_connected_nodes;
    }

    fprintf(stderr, "An port with id %u does not exist.\n", port_id);
    return 0;
}

uint32_t linvoke_event_get_port_id(linvoke_event_s *const event)
{
    return event->port_id;
}

void *linvoke_event_get_user_data(linvoke_event_s *const event)
{
    return event->user_data;
}
