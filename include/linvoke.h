/**
 * @file:      linvoke.h
 *
 * @date:      22 January 2024
 *
 * @author:    Kostoski Stefan
 *
 * @copyright: Copyright (c) 2024 Kostoski Stefan.
 *             This work is licensed under the terms of the MIT license.
 *             For a copy, see <https://opensource.org/license/MIT>.
 */

#pragma once

#include <stdint.h>

typedef struct linvoke_s linvoke_s;
typedef struct linvoke_port_s linvoke_port_s;
typedef struct linvoke_event_s linvoke_event_s;
typedef void (*linvoke_node_pointer)(linvoke_event_s *event);

/**
 * @brief Creates a new linvoke object
 * @return Pointer to the created linvoke object
 */
linvoke_s *linvoke_create(void);

/**
 * @brief Destroys a linvoke object
 * @param linvoke Pointer to a linvoke object
 */
void linvoke_destroy(linvoke_s *const linvoke);

/**
 * @brief Registers a new port with a given ID
 * @param linvoke Pointer to a linvoke object
 * @param port_id The ID of the port that will be registered
 */
void linvoke_register_port(linvoke_s *const linvoke, uint32_t port_id);

/**
 * @brief Connects a new node to an port. The callback functions will be called in the order they were connected
 * @param linvoke Pointer to a linvoke object
 * @param port_id The ID of the port to which the node will be connected
 * @param node The node that will be called when an event is emitted
 */
void linvoke_connect(linvoke_s *const linvoke, const uint32_t port_id, linvoke_node_pointer node);

/**
 * @brief Emits an event from a given port with given data
 * @param linvoke Pointer to a linvoke object
 * @param port_id The ID of the port which will emit an event
 * @param user_data The user data that will be passed to the connected nodes. Can be NULL
 */
void linvoke_emit(linvoke_s *const linvoke, const uint32_t port_id, void *user_data);

/**
 * @brief Get the number of registered ports
 * @param linvoke Pointer to a linvoke object
 * @return The number of registered ports
 */
uint32_t linvoke_get_registered_port_count(linvoke_s *const linvoke);

/**
 * @brief Get the number of nodes that are connected to a port with a given ID
 * @param linvoke Pointer to a linvoke object
 * @param port_id The ID of the port to which the nodes are connected
 * @return The number of nodes connected to the port
 */
uint32_t linvoke_get_node_count(linvoke_s *const linvoke, const uint32_t port_id);

/**
 * @brief Get the ID of the port that emitted an event
 * @return The ID of the port that emitted an event
 */
uint32_t linvoke_event_get_port_id(linvoke_event_s *const event);

/**
 * @brief Get the user data of the event
 * @return The user data of the event
 */
void *linvoke_event_get_user_data(linvoke_event_s *const event);
