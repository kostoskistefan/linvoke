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

#ifndef LINVOKE_EVENT_CAPACITY
#define LINVOKE_EVENT_CAPACITY 8
#endif 

#ifndef LINVOKE_NODE_CAPACITY
#define LINVOKE_NODE_CAPACITY  16
#endif

typedef struct linvoke_event_s linvoke_event_s;
typedef struct linvoke_s linvoke_s;

/**
 * @brief Creates a new linvoke
 * @return Pointer to the created linvoke
 */
linvoke_s *linvoke_create(void);

/**
 * @brief Destroys a linvoke
 * @param linvoke[in] Pointer to a linvoke object
 */
void linvoke_destroy(linvoke_s *const linvoke);

/**
 * @brief Registers an event
 * @param linvoke[in] Pointer to a linvoke object
 * @param event_id[in] The ID of the event that will be registered
 */
void linvoke_register_event(linvoke_s *const linvoke, uint32_t event_id);

/**
 * @brief Connects a new node to an event. The callback functions will be called in the order they were connected
 * @param linvoke[in] Pointer to a linvoke object
 * @param event_id[in] The ID of the event to which the node will be connected
 * @param callback[in] The callback that will be called when the event is emitted
 * @param user_data[in] The user data that will be passed to the callback function
 */
void linvoke_connect(
    linvoke_s *const linvoke,
    const uint32_t event_id,
    void (*const callback)(void *const user_data),
    void *const user_data
);

/**
 * @brief Emits an event
 * @param linvoke[in] Pointer to a linvoke object
 * @param event_id[in] The ID of the event which will be emitted
 */
void linvoke_emit(linvoke_s *const linvoke, const uint32_t event_id);

/**
* @brief Get the number of registered events
* @param linvoke[in] Pointer to a linvoke object
* @return The number of registered events
*/
uint32_t linvoke_get_registered_event_count(linvoke_s *const linvoke);

/**
* @brief Get the number of nodes that are connected to an event 
* @param linvoke[in] Pointer to a linvoke object
* @param event_id[in] The ID of the event to which the nodes are be connected
* @return The number of nodes connected to the event with id `event_id`
*/
uint32_t linvoke_get_node_count(linvoke_s *const linvoke, const uint32_t event_id);
