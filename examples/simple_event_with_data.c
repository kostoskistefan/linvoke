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
 * @brief Prints the ID of the signal that emitted the event, as well as the included user data
 */
void slot(linvoke_event_s *event)
{
    const uint32_t signal_id = linvoke_event_get_signal_id(event);
    const char *data = *((const char **) linvoke_event_get_user_data(event));
    printf("Signal ID: %u, Data: %s\n", signal_id, data);
}

int main(void)
{
    // Create a linvoke object
    linvoke_s *linvoke = linvoke_create();

    // Define a unique ID for the signal that is going to be registered
    const uint32_t signal = 123;

    // Register the signal ID
    linvoke_register_signal(linvoke, signal);

    // Connect the signal to the slot
    linvoke_connect(linvoke, signal, slot);

    // Emit an event from the signal with custom user data
    const char *string_data = "This data doesn't have to be of type string. It can be anything you want!";
    linvoke_emit(linvoke, signal, &string_data);

    // Destroy the linvoke object to free the used resources
    linvoke_destroy(linvoke);

    return 0;
}
