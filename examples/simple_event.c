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
 * @brief Prints the ID of the signal that emitted the event
 */
void slot(linvoke_event_s *event)
{
    (void) event; // Unused
    printf("Hello, World!\n");
}

int main(void)
{
    // Create a linvoke object
    linvoke_s *linvoke = linvoke_create();

    // Define a unique ID for the signal that is going to be registered
    const linvoke_signal signal = 1358;

    // Register the signal ID
    linvoke_register_signal(linvoke, signal);

    // Connect the signal to the slot
    linvoke_connect(linvoke, signal, slot);

    // Emit an event from the signal
    linvoke_emit(linvoke, signal, NULL);

    // Destroy the linvoke object to free the used resources
    linvoke_destroy(linvoke);

    return 0;
}
