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

/**
 * @struct linvoke_s
 * @brief Structure that holds information about a linvoke object
 */
typedef struct linvoke_s linvoke_s;

/**
 * @struct linvoke_event_s
 * @brief Structure that holds the data for an event
 */
typedef struct linvoke_event_s linvoke_event_s;

/**
 * @typedef linvoke_slot_pointer
 * @brief Pointer to a function that will be called when an event is emitted
 */
typedef void (*linvoke_slot_pointer)(linvoke_event_s *event);

/**
 * @typedef linvoke_signal
 * @brief The ID of a signal
 */
typedef uint32_t linvoke_signal;

/**
 * @fn linvoke_create
 * @brief Creates a new linvoke object
 * @return Pointer to the created linvoke object
 */
linvoke_s *linvoke_create(void);

/**
 * @fn linvoke_destroy
 * @brief Destroys a linvoke object
 * @param linvoke Pointer to a linvoke object
 */
void linvoke_destroy(linvoke_s *const linvoke);

/**
 * @fn linvoke_register_signal
 * @brief Registers a new signal with a given ID
 * @param linvoke Pointer to a linvoke object
 * @param signal_id The ID of the signal that will be registered
 */
void linvoke_register_signal(linvoke_s *const linvoke, const linvoke_signal signal_id);

/**
 * @fn linvoke_connect
 * @brief Connects a new slot to an signal. The callback functions will be called in the order they were connected
 * @param linvoke Pointer to a linvoke object
 * @param signal_id The ID of the signal to which the slot will be connected
 * @param slot The slot that will be called when an event is emitted
 */
void linvoke_connect(linvoke_s *const linvoke, const linvoke_signal signal_id, linvoke_slot_pointer slot);

/**
 * @fn linvoke_emit
 * @brief Emits an event from a given signal with given data
 * @param linvoke Pointer to a linvoke object
 * @param signal_id The ID of the signal which will emit an event
 * @param user_data The user data that will be passed to the connected slots. Can be NULL
 */
void linvoke_emit(linvoke_s *const linvoke, const linvoke_signal signal_id, void *user_data);

/**
 * @fn linvoke_get_registered_signal_count
 * @brief Get the number of registered signals
 * @param linvoke Pointer to a linvoke object
 * @return The number of registered signals
 */
uint32_t linvoke_get_registered_signal_count(linvoke_s *const linvoke);

/**
 * @fn linvoke_get_slot_count
 * @brief Get the number of slots that are connected to a signal with a given ID
 * @param linvoke Pointer to a linvoke object
 * @param signal_id The ID of the signal to which the slots are connected
 * @return The number of slots connected to the signal
 */
uint32_t linvoke_get_slot_count(linvoke_s *const linvoke, const linvoke_signal signal_id);

/**
 * @fn linvoke_get_signal_id
 * @brief Get the ID of the signal that emitted an event
 * @return The ID of the signal that emitted an event
 */
linvoke_signal linvoke_event_get_signal_id(linvoke_event_s *const event);

/**
 * @fn linvoke_event_get_user_data
 * @brief Get the user data of the event
 * @return The user data of the event
 */
void *linvoke_event_get_user_data(linvoke_event_s *const event);
