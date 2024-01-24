/**
 * @file:      multi_slot_event.c
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
 * @brief Prints the name of the slot
 */
void slot1(linvoke_event_s *event)
{
    printf("Hello from slot1\n");
}

/**
 * @brief Prints the name of the slot
 */
void slot2(linvoke_event_s *event)
{
    printf("Hello from slot2\n");
}

/**
 * @brief Prints the name of the slot
 */
void slot3(linvoke_event_s *event)
{
    printf("Hello from slot3\n");
}

int main(void)
{
    // Create a linvoke object
    linvoke_s *linvoke = linvoke_create();

    // Define a unique ID for the signal that is going to be registered
    const uint32_t signal = 1358;

    // Register the signal ID
    linvoke_register_signal(linvoke, signal);

    // Connect the signal to the slots
    // Order does play a role here. When the event is emitted, the
    // slots will be called in the same order as they were connected
    linvoke_connect(linvoke, signal, slot1);
    linvoke_connect(linvoke, signal, slot2);
    linvoke_connect(linvoke, signal, slot3);

    // Emit an event from the signal
    linvoke_emit(linvoke, signal, NULL);

    // Destroy the linvoke object to free the used resources
    linvoke_destroy(linvoke);

    return 0;
}
