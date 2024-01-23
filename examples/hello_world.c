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

/**
 * @struct custom_struct_data
 * @brief  An example structure that contains an integer and a floating point number
 */
typedef struct
{
    int key;
    float value;
} custom_struct_data;

/**
 * @brief Prints the ID of the port that emitted the event
 */
void print_empty_node(linvoke_event_s *event)
{
    const uint32_t port_id = linvoke_event_get_port_id(event);
    const void *data = linvoke_event_get_user_data(event);  // The data here is NULL
    printf("PORT: %u\tDATA: %s\n", port_id, (char *) data); // Convert the void* to char* to print "(null)"
}

/**
 * @brief Prints the ID of the port that emitted the event and custom user data of type string
 */
void print_string_node(linvoke_event_s *event)
{
    const uint32_t port_id = linvoke_event_get_port_id(event);
    const char *data = *((const char **) linvoke_event_get_user_data(event));
    printf("PORT: %u\tDATA: %s\n", port_id, data);
}

/**
 * @brief Prints the ID of the port that emitted the event and custom user data of type int
 */
void print_int_node(linvoke_event_s *event)
{
    const uint32_t port_id = linvoke_event_get_port_id(event);
    const int data = *((int *) linvoke_event_get_user_data(event));
    printf("PORT: %u\tDATA: %d\n", port_id, data);
}

/**
 * @brief Prints the ID of the port that emitted the event and custom user data of type custom_struct_data
 */
void print_struct_node(linvoke_event_s *event)
{
    const uint32_t port_id = linvoke_event_get_port_id(event);
    const custom_struct_data data = *((custom_struct_data *) linvoke_event_get_user_data(event));
    printf("PORT: %u\tDATA: key = %d, value = %f\n", port_id, data.key, data.value);
}

int main(void)
{
    // Create a linvoke object
    linvoke_s *linvoke = linvoke_create();

    // Define unique IDs for the ports that are going to be registered inside linvoke
    const uint32_t print_empty_port = 123;
    const uint32_t print_string_port = 12;
    const uint32_t print_int_port = 80;
    const uint32_t print_struct_port = 7658;
    const uint32_t multi_node_port = 4444;

    // Register the port IDs
    linvoke_register_port(linvoke, print_empty_port);
    linvoke_register_port(linvoke, print_string_port);
    linvoke_register_port(linvoke, print_int_port);
    linvoke_register_port(linvoke, print_struct_port);
    linvoke_register_port(linvoke, multi_node_port);

    // Connect the port with id == print_empty_port (id 123) to the print_node
    linvoke_connect(linvoke, print_empty_port, print_empty_node, NULL); // No user data is passed

    // Connect the port with id == print_string_port (id 12) to the print_string_node
    const char *string_data = "Hey string!";
    linvoke_connect(linvoke, print_string_port, print_string_node, (void *) &string_data); // String user data

    // Connect the port with id == print_int_port (id 0) to the print_int_node
    const int int_data = 15;
    linvoke_connect(linvoke, print_int_port, print_int_node, (void *) &int_data); // Int user data

    // Connect the port with id == print_struct_port (id 7658) to the print_struct_node
    const custom_struct_data struct_data = { 46, 3.14f };
    linvoke_connect(linvoke, print_struct_port, print_struct_node, (void *) &struct_data); // Struct user data

    // Connect the port with id == multi_node_port (id 4444) to multiple nodes
    const char *multi_string_data = "Hello from multi node!";
    const int multi_int_data = 168;
    const custom_struct_data multi_struct_data = { 156, 0.369f };
    linvoke_connect(linvoke, multi_node_port, print_string_node, (void *) &multi_string_data);
    linvoke_connect(linvoke, multi_node_port, print_int_node, (void *) &multi_int_data);
    linvoke_connect(linvoke, multi_node_port, print_struct_node, (void *) &multi_struct_data);

    // Emit the ports
    linvoke_emit(linvoke, print_empty_port);
    linvoke_emit(linvoke, print_string_port);
    linvoke_emit(linvoke, print_int_port);
    linvoke_emit(linvoke, print_struct_port);
    linvoke_emit(linvoke, multi_node_port);

    // Destroy the linvoke object to free the used resources
    linvoke_destroy(linvoke);

    return 0;
}
