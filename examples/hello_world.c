/**
 * @file:      hello_world.c
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

// An example struct that is passed to a callback function
typedef struct
{
    int key;
    float value;
} custom_struct_data;

/**
* @brief Prints "Hello World!" and ignores user_data
*/
void print_callback(void *user_data)
{
    (void) user_data; // user_data is not used

    printf("Hello World!\n");
}

/**
* @brief Prints a string passed as the user_data pointer
*/
void print_string_callback(void *user_data)
{
    const char *data = *((const char **) user_data);
    printf("String data: %s\n", data);
}

/**
* @brief Prints an integer passed as the user_data pointer
*/
void print_int_callback(void *user_data)
{
    int data = *((int *) user_data);
    printf("Int data: %d\n", data);
}

/**
* @brief Prints a custom struct passed as the user_data pointer
*/
void print_struct_callback(void *user_data)
{
    custom_struct_data *data = (custom_struct_data *) user_data;
    printf("Struct data: key: %d value: %f\n", data->key, data->value);
}

int main(void)
{
    // Create a linvoke object
    linvoke_s *linvoke = linvoke_create();

    // Define a unique IDs for registering events inside linvoke
    const uint32_t print_empty_event_id = 123;
    const uint32_t print_string_event_id = 12;
    const uint32_t print_int_event_id = 0;
    const uint32_t print_struct_event_id = 7658;
    const uint32_t multi_callback_event_id = 4444;

    // Register the previously defined event IDs
    linvoke_register_event(linvoke, print_empty_event_id);
    linvoke_register_event(linvoke, print_string_event_id);
    linvoke_register_event(linvoke, print_int_event_id);
    linvoke_register_event(linvoke, print_struct_event_id);
    linvoke_register_event(linvoke, multi_callback_event_id);

    // Connect the event with id == print_empty_event_id (id 123) to the print_callback function
    linvoke_connect(linvoke, print_empty_event_id, print_callback, NULL); // No user data is passed

    // Connect the event with id == print_string_event_id (id 12) to the print_string_callback function
    const char *string_data = "Hey string!";
    linvoke_connect(linvoke, print_string_event_id, print_string_callback, (void *) &string_data); // String user data

    // Connect the event with id == print_int_event_id (id 0) to the print_int_callback function
    const int int_data = 15;
    linvoke_connect(linvoke, print_int_event_id, print_int_callback, (void *) &int_data); // Int user data

    // Connect the event with id == print_struct_event_id (id 7658) to the print_struct_callback function
    const custom_struct_data struct_data = { 46, 3.14f };
    linvoke_connect(linvoke, print_struct_event_id, print_struct_callback, (void *) &struct_data); // Struct user data
    
    // Connect the event with id == multi_callback_event_id (id 4444) to multiple callback functions
    const char *multi_string_data = "Hello from multi callback!";
    const int multi_int_data = 168;
    const custom_struct_data multi_struct_data = { 156, 0.369f };
    linvoke_connect(linvoke, multi_callback_event_id, print_string_callback, (void *) &multi_string_data);
    linvoke_connect(linvoke, multi_callback_event_id, print_int_callback, (void *) &multi_int_data);
    linvoke_connect(linvoke, multi_callback_event_id, print_struct_callback, (void *) &multi_struct_data);

    // Emit the events
    linvoke_emit(linvoke, print_empty_event_id);
    linvoke_emit(linvoke, print_string_event_id);
    linvoke_emit(linvoke, print_int_event_id);
    linvoke_emit(linvoke, print_struct_event_id);
    linvoke_emit(linvoke, multi_callback_event_id);

    // Destroy the linvoke object to free the used resources
    linvoke_destroy(linvoke);

    return 0;
}
