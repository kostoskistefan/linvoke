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

/**
 * @def LINVOKE_SIGNAL_ARRAY_BLOCK_SIZE
 * @brief The default block size for the signal array.
 *        Used for allocating and reallocating memory for the signal array.
 *        Smaller value will use less memory, but it will result in more frequent reallocations.
 *        Bigger value will use more memory, but it will result in less frequent reallocations.
 */
#ifndef LINVOKE_SIGNAL_ARRAY_BLOCK_SIZE
#define LINVOKE_SIGNAL_ARRAY_BLOCK_SIZE 8
#endif

/**
 * @def LINVOKE_SLOT_ARRAY_BLOCK_SIZE
 * @brief The default block size for the slot array.
 *        Used for allocating and reallocating memory for the slots array in each connected signal.
 *        Smaller value will use less memory, but it will result in more frequent reallocations.
 *        Bigger value will use more memory, but it will result in less frequent reallocations.
 */
#ifndef LINVOKE_SLOT_ARRAY_BLOCK_SIZE
#define LINVOKE_SLOT_ARRAY_BLOCK_SIZE 8
#endif

/**
 * @struct linvoke_event_s
 * @brief Structure that holds the data for an event
 * @var signal_id The ID of the signal that emitted the event
 * @var user_data The user data that was passed when the event was emitted
 */
struct linvoke_event_s
{
    uint32_t signal_id;
    void *user_data;
};

/**
 * @struct linvoke_signal_s
 * @brief Structure that holds information about a signal
 * @var id The ID of the signal
 * @var slots An array of pointers to slots that are connected to the signal
 * @var connected_slot_count The number of slots that are currently connected to the signal
 * @var slot_capacity The maximum capacity of the slots array
 */
struct linvoke_signal_s
{
    uint32_t id;
    linvoke_slot_pointer *slots;
    uint32_t connected_slot_count;
    uint32_t slot_capacity;
};

/**
 * @struct linvoke_s
 * @brief Structure that holds information about a linvoke object
 * @var signals An array of registered signals
 * @var registered_signal_count The number of signals that are registered within the linvoke object
 * @var signal_capacity The maximum capacity of the signals array
 */
struct linvoke_s
{
    linvoke_signal_s *signals;
    uint32_t registered_signal_count;
    uint32_t signal_capacity;
};

/**
 * @brief Finds a signal with a given ID if it exists
 * @param linvoke Pointer to a linvoke object
 * @param signal_id The ID of the signal to find
 * @return A pointer to the signal with the given ID or NULL if no such signal was found
 */
linvoke_signal_s *linvoke_find_signal(linvoke_s *const linvoke, uint32_t signal_id);

linvoke_s *linvoke_create(void)
{
    linvoke_s *linvoke = malloc(sizeof(*linvoke));

    if (linvoke == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for the linvoke object.\n");
        return NULL;
    }

    linvoke->signals = malloc(LINVOKE_SIGNAL_ARRAY_BLOCK_SIZE * sizeof(*linvoke->signals));

    if (linvoke->signals == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for the linvoke signals.\n");
        free(linvoke);
        return NULL;
    }

    linvoke->registered_signal_count = 0;
    linvoke->signal_capacity = LINVOKE_SIGNAL_ARRAY_BLOCK_SIZE;

    return linvoke;
}

void linvoke_destroy(linvoke_s *const linvoke)
{
    for (uint32_t i = 0; i < linvoke->registered_signal_count; ++i)
    {
        free(linvoke->signals[i].slots);
    }

    free(linvoke->signals);
    free(linvoke);
}

void linvoke_register_signal(linvoke_s *const linvoke, uint32_t signal_id)
{
    // Check if there is an existing signal with the same ID
    if (linvoke_find_signal(linvoke, signal_id) != NULL)
    {
        fprintf(stderr, "A signal with id %u already exists.\n", signal_id);
        return;
    }

    // Reallocate the signals array memory if the capacity is full
    if (linvoke->registered_signal_count == linvoke->signal_capacity)
    {
        linvoke->signal_capacity += LINVOKE_SIGNAL_ARRAY_BLOCK_SIZE;
        linvoke_signal_s *reallocated_signals = realloc(linvoke->signals, linvoke->signal_capacity * sizeof(*linvoke->signals));

        if (reallocated_signals == NULL)
        {
            fprintf(stderr, "Failed to reallocate memory for the signals array.\n");
            return;
        }

        linvoke->signals = reallocated_signals;
    }

    // Register the new signal
    linvoke_signal_s *const signal = &linvoke->signals[linvoke->registered_signal_count];
    signal->id = signal_id;
    signal->connected_slot_count = 0;
    signal->slot_capacity = LINVOKE_SLOT_ARRAY_BLOCK_SIZE;
    signal->slots = malloc(LINVOKE_SLOT_ARRAY_BLOCK_SIZE * sizeof(*signal->slots));

    if (signal->slots == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for the signal slots.\n");
        return;
    }

    ++linvoke->registered_signal_count;
}

void linvoke_connect(linvoke_s *const linvoke, const uint32_t signal_id, linvoke_slot_pointer slot)
{
    // Find the signal with the given ID
    linvoke_signal_s *const signal = linvoke_find_signal(linvoke, signal_id);

    // Signal not found
    if (signal == NULL)
    {
        fprintf(stderr, "A signal with id %u does not exist.\n", signal_id);
        return;
    }

    // Check if the callback is already connected
    for (uint32_t j = 0; j < signal->connected_slot_count; ++j)
    {
        if (signal->slots[j] == slot)
        {
            fprintf(stderr, "The callback function is already connected to signal %d\n", signal_id);
            return;
        }
    }

    // Reallocate the slots array memory if the capacity is full
    if (signal->connected_slot_count == signal->slot_capacity)
    {
        signal->slot_capacity += LINVOKE_SLOT_ARRAY_BLOCK_SIZE;

        linvoke_slot_pointer *reallocated_slots = realloc(signal->slots, signal->slot_capacity * sizeof(*signal->slots));

        if (reallocated_slots == NULL)
        {
            fprintf(stderr, "Failed to reallocate memory for the slots array.\n");
            return;
        }

        signal->slots = reallocated_slots;
    }

    // Connect the slot
    signal->slots[signal->connected_slot_count] = slot;

    ++signal->connected_slot_count;
}

void linvoke_emit(linvoke_s *const linvoke, const uint32_t signal_id, void *user_data)
{
    // Find the signal with the given ID
    linvoke_signal_s *const signal = linvoke_find_signal(linvoke, signal_id);

    // Signal not found
    if (signal == NULL)
    {
        fprintf(stderr, "A signal with id %u does not exist.\n", signal_id);
        return;
    }

    linvoke_event_s event = { .signal_id = signal_id, .user_data = user_data };

    // Call the callback function for all slots connected to the signal and override the user data
    for (size_t j = 0; j < signal->connected_slot_count; ++j)
    {
        signal->slots[j](&event);
    }
}

linvoke_signal_s *linvoke_find_signal(linvoke_s *const linvoke, uint32_t signal_id)
{
    for (uint32_t i = 0; i < linvoke->registered_signal_count; ++i)
    {
        if (linvoke->signals[i].id == signal_id)
        {
            return &linvoke->signals[i];
        }
    }

    return NULL;
}

uint32_t linvoke_get_registered_signal_count(linvoke_s *const linvoke)
{
    return linvoke->registered_signal_count;
}

uint32_t linvoke_get_slot_count(linvoke_s *const linvoke, const uint32_t signal_id)
{
    // Find the signal with the given ID
    linvoke_signal_s *const signal = linvoke_find_signal(linvoke, signal_id);

    // Signal not found
    if (signal == NULL)
    {
        fprintf(stderr, "Signal with id %u not found.\n", signal_id);
        return 0;
    }

    return signal->connected_slot_count;
}

uint32_t linvoke_event_get_signal_id(linvoke_event_s *const event)
{
    return event->signal_id;
}

void *linvoke_event_get_user_data(linvoke_event_s *const event)
{
    return event->user_data;
}
