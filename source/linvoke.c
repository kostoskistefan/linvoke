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

typedef struct
{
    void *user_data;
    void (*callback)(void *user_data);
} linvoke_node_s;

struct linvoke_event_s
{
    uint32_t id;
    linvoke_node_s *nodes;
    uint32_t number_of_nodes;
    uint32_t node_capacity;
};

struct linvoke_s
{
    linvoke_event_s *events;
    uint32_t number_of_events;
    uint32_t event_capacity;
};

linvoke_s *linvoke_create(void)
{
    linvoke_s *linvoke = calloc(1, sizeof(*linvoke));

    linvoke->events = calloc(LINVOKE_EVENT_CAPACITY, sizeof(*linvoke->events));
    linvoke->number_of_events = 0;
    linvoke->event_capacity = LINVOKE_EVENT_CAPACITY;

    return linvoke;
}

void linvoke_destroy(linvoke_s *const linvoke)
{
    for (uint32_t i = 0; i < linvoke->number_of_events; ++i)
    {
        free(linvoke->events[i].nodes);
    }

    free(linvoke->events);
    free(linvoke);
}

void linvoke_register_event(linvoke_s *const linvoke, uint32_t event_id)
{
    for (uint32_t i = 0; i < linvoke->number_of_events; ++i)
    {
        if (linvoke->events[i].id == event_id)
        {
            fprintf(stderr, "An event with id %u already exists.\n", event_id);
            return;
        }
    }

    if (linvoke->number_of_events == linvoke->event_capacity)
    {
        linvoke->event_capacity += LINVOKE_EVENT_CAPACITY;
        linvoke->events = realloc(linvoke->events, linvoke->event_capacity * sizeof(*linvoke->events));
    }

    linvoke_event_s *const event = &linvoke->events[linvoke->number_of_events];
    event->id = event_id;
    event->nodes = calloc(LINVOKE_NODE_CAPACITY, sizeof(*event->nodes));
    event->number_of_nodes = 0;
    event->node_capacity = LINVOKE_NODE_CAPACITY;

    ++linvoke->number_of_events;
}

void linvoke_connect(
    linvoke_s *const linvoke,
    const uint32_t event_id,
    void (*const callback)(void *const user_data),
    void *const user_data
)
{
    for (uint32_t i = 0; i < linvoke->number_of_events; ++i)
    {
        if (linvoke->events[i].id != event_id)
        {
            continue;
        }

        linvoke_event_s *const event = &linvoke->events[i];

        for (uint32_t j = 0; j < event->number_of_nodes; ++j)
        {
            if (event->nodes[j].callback != callback)
            {
                continue;
            }

            fprintf(stderr, "A callback with the same address already exists.\n");
            return;
        }

        if (event->number_of_nodes == event->node_capacity)
        {
            event->node_capacity += LINVOKE_NODE_CAPACITY;
            event->nodes = realloc(event->nodes, event->node_capacity * sizeof(*event->nodes));
        }

        linvoke_node_s *const new_node = &event->nodes[event->number_of_nodes];
        new_node->callback = callback;
        new_node->user_data = user_data;

        ++event->number_of_nodes;

        return;
    }

    fprintf(stderr, "An event with id %u does not exist.\n", event_id);
}

void linvoke_emit(linvoke_s *const linvoke, const uint32_t event_id)
{
    for (uint32_t i = 0; i < linvoke->number_of_events; ++i)
    {
        if (linvoke->events[i].id != event_id)
        {
            continue;
        }

        linvoke_event_s *const event = &linvoke->events[i];

        for (size_t j = 0; j < event->number_of_nodes; ++j)
        {
            linvoke_node_s *const node = &event->nodes[j];
            node->callback(node->user_data);
        }

        return;
    }

    fprintf(stderr, "An event with id %u does not exist.\n", event_id);
}

uint32_t linvoke_get_registered_event_count(linvoke_s *const linvoke)
{
    return linvoke->number_of_events;
}

uint32_t linvoke_get_node_count(linvoke_s *const linvoke, const uint32_t event_id)
{
    for (uint32_t i = 0; i < linvoke->number_of_events; ++i)
    {
        if (linvoke->events[i].id != event_id)
        {
            continue;
        }

        return linvoke->events[i].number_of_nodes;
    }

    fprintf(stderr, "An event with id %u does not exist.\n", event_id);
    return 0;
}
