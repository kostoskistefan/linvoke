/**
 * @file:      main.c
 *
 * @date:      22 January 2024
 *
 * @author:    Kostoski Stefan
 *
 * @copyright: Copyright (c) 2024 Kostoski Stefan.
 *             This work is licensed under the terms of the MIT license.
 *             For a copy, see <https://opensource.org/license/MIT>.
 */

#include <linvoke.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

void mock_callback1(void *user_data)
{
    function_called();
}

void mock_callback2(void *user_data)
{
    function_called();
}

static void test_one_event_one_callback(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 0);

    const uint32_t event_id = 0;
    linvoke_register_event(linvoke, event_id);

    // There should be 1 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 1);

    // The event should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, event_id), 0);

    linvoke_connect(linvoke, event_id, mock_callback1, NULL);

    // The event should have 1 node connected
    assert_int_equal(linvoke_get_node_count(linvoke, event_id), 1);

    // Since only 1 node is connected, when we emit the event, we expect only 1 callback
    expect_function_calls(mock_callback1, 1);

    linvoke_emit(linvoke, event_id);

    linvoke_destroy(linvoke);
}

static void test_one_event_multiple_same_callbacks(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 0);

    const uint32_t event_id = 0;
    linvoke_register_event(linvoke, event_id);

    // There should be 1 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 1);

    // The event should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, event_id), 0);

    linvoke_connect(linvoke, event_id, mock_callback1, NULL);
    linvoke_connect(linvoke, event_id, mock_callback1, NULL); // This one will fail since it's already connected

    // The event should have 1 node connected, because the second linvoke_connect
    // should not work, since the callback is already connected
    assert_int_equal(linvoke_get_node_count(linvoke, event_id), 1);

    // Since only 1 node is connected, when we emit the event, we expect only 1 callback
    expect_function_calls(mock_callback1, 1);

    linvoke_emit(linvoke, event_id);

    linvoke_destroy(linvoke);
}

static void test_one_event_multiple_different_callbacks(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 0);

    const uint32_t event_id = 0;
    linvoke_register_event(linvoke, event_id);

    // There should be 1 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 1);

    // The event should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, event_id), 0);

    linvoke_connect(linvoke, event_id, mock_callback1, NULL);
    linvoke_connect(linvoke, event_id, mock_callback2, NULL);

    // The event should have 2 node connected
    assert_int_equal(linvoke_get_node_count(linvoke, event_id), 2);

    // The event has 2 callbacks, so we expect 2 callback functions to be called
    expect_function_calls(mock_callback1, 1);
    expect_function_calls(mock_callback2, 1);

    linvoke_emit(linvoke, event_id);

    linvoke_destroy(linvoke);
}

static void test_multiple_events_same_id_one_callback(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 0);

    const uint32_t event1_id = 0;
    const uint32_t event2_id = 0;
    linvoke_register_event(linvoke, event1_id);
    linvoke_register_event(linvoke, event2_id);

    // There should be 1 event registered, because we tried to register the same event id twice
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 1);

    // The events should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, event1_id), 0);
    assert_int_equal(linvoke_get_node_count(linvoke, event2_id), 0);

    // This connect call will work, because the event with id 0 is not connected to the mock_callback1
    linvoke_connect(linvoke, event1_id, mock_callback1, NULL);

    // This connect call will not work, because the event with id 0 is already connected to the mock_callback1
    linvoke_connect(linvoke, event2_id, mock_callback1, NULL);

    // Since both events have the same id, and we already registered the event id 0 and connected
    // the mock_callback1 to that id, we expect the event with id 0 to have 1 node connected
    assert_int_equal(linvoke_get_node_count(linvoke, event1_id), 1);
    assert_int_equal(linvoke_get_node_count(linvoke, event2_id), 1);

    // Because both nodes have the same id, emitting the events with id 0
    // (both event1_id and event2_id) will call the mock_callback1 twice
    expect_function_calls(mock_callback1, 2);

    linvoke_emit(linvoke, event1_id);
    linvoke_emit(linvoke, event2_id);

    linvoke_destroy(linvoke);
}

static void test_multiple_events_same_id_multiple_same_callback(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 0);

    const uint32_t event1_id = 0;
    const uint32_t event2_id = 0;
    linvoke_register_event(linvoke, event1_id);
    linvoke_register_event(linvoke, event2_id);

    // There should be 1 event registered, because we tried to register the same event id twice
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 1);

    // The events should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, event1_id), 0);
    assert_int_equal(linvoke_get_node_count(linvoke, event2_id), 0);

    // This connect call will work, because the event with id 0
    // (event1_id in this case and event1_id == event2_id) is not connected to the mock_callback1
    linvoke_connect(linvoke, event1_id, mock_callback1, NULL);

    // This connect call will not work, because the event with id 0
    // (event1_id in this case and event1_id == event2_id) is already connected to the mock_callback1
    linvoke_connect(linvoke, event1_id, mock_callback1, NULL);

    // This connect call will not work, because the event with id 0
    // (event2_id in this case and event1_id == event2_id) is already connected to the mock_callback1
    linvoke_connect(linvoke, event2_id, mock_callback1, NULL);

    // This connect call will not work, because the event with id 0
    // (event2_id in this case and event1_id == event2_id) is already connected to the mock_callback1
    linvoke_connect(linvoke, event2_id, mock_callback1, NULL);

    // Since both events have the same id, and we already registered the event id 0 and connected
    // the mock_callback1 to that id, we expect the event with id 0 to have 1 node connected
    assert_int_equal(linvoke_get_node_count(linvoke, event1_id), 1);
    assert_int_equal(linvoke_get_node_count(linvoke, event2_id), 1);

    // Because both nodes have the same id, emitting the events with id 0
    // (both event1_id and event2_id) will call the mock_callback1 twice
    expect_function_calls(mock_callback1, 2);

    linvoke_emit(linvoke, event1_id);
    linvoke_emit(linvoke, event2_id);

    linvoke_destroy(linvoke);
}

static void test_multiple_events_same_id_multiple_different_callback(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 0);

    const uint32_t event1_id = 0;
    const uint32_t event2_id = 0;
    linvoke_register_event(linvoke, event1_id);
    linvoke_register_event(linvoke, event2_id);

    // There should be 1 event registered, because we tried to register the same event id twice
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 1);

    // The events should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, event1_id), 0);
    assert_int_equal(linvoke_get_node_count(linvoke, event2_id), 0);

    /**
     * Event 1 Connections
     */

    // This connect call will work, because the event with id 0
    // (event1_id in this case and event1_id == event2_id) is not connected to the mock_callback1
    linvoke_connect(linvoke, event1_id, mock_callback1, NULL);

    // This connect call will not work, because the event with id 0
    // (event1_id in this case and event1_id == event2_id) is already connected to the mock_callback1
    linvoke_connect(linvoke, event1_id, mock_callback1, NULL);

    // This connect call will work, because the event with id 0
    // (event1_id in this case and event1_id == event2_id) is not connected to the mock_callback2
    linvoke_connect(linvoke, event1_id, mock_callback2, NULL);

    // This connect call will not work, because the event with id 0
    // (event1_id in this case and event1_id == event2_id) is already connected to the mock_callback2
    linvoke_connect(linvoke, event1_id, mock_callback2, NULL);

    /**
     * Event 2 Connections
     */

    // This connect call will work, because the event with id 0
    // (event2_id in this case and event1_id == event2_id) is not connected to the mock_callback1
    linvoke_connect(linvoke, event2_id, mock_callback1, NULL);

    // This connect call will not work, because the event with id 0
    // (event2_id in this case and event1_id == event2_id) is already connected to the mock_callback1
    linvoke_connect(linvoke, event2_id, mock_callback1, NULL);

    // This connect call will work, because the event with id 0
    // (event2_id in this case and event1_id == event2_id) is not connected to the mock_callback2
    linvoke_connect(linvoke, event2_id, mock_callback2, NULL);

    // This connect call will not work, because the event with id 0
    // (event2_id in this case and event1_id == event2_id) is already connected to the mock_callback2
    linvoke_connect(linvoke, event2_id, mock_callback2, NULL);

    // Since both events have the same id, and we already registered the event id 0 and connected
    // the mock_callback1 and mock_callback2 to that id, we expect the event with id 0
    // (both event1_id and event2_id ) to have 2 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, event1_id), 2);
    assert_int_equal(linvoke_get_node_count(linvoke, event2_id), 2);

    // Because both nodes have the same id, emitting the event with id == event1_id
    // will call the mock_callback1 and mock_callback2
    expect_function_calls(mock_callback1, 1);
    expect_function_calls(mock_callback2, 1);

    linvoke_emit(linvoke, event1_id);

    // Because both nodes have the same id, emitting the event with id == event2_id
    // will call the mock_callback1 and mock_callback2
    expect_function_calls(mock_callback1, 1);
    expect_function_calls(mock_callback2, 1);

    linvoke_emit(linvoke, event2_id);

    linvoke_destroy(linvoke);
}

static void test_multiple_events_different_id_one_callback(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 0);

    const uint32_t event1_id = 0;
    const uint32_t event2_id = 1;
    linvoke_register_event(linvoke, event1_id);
    linvoke_register_event(linvoke, event2_id);

    // There should be 2 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 2);

    // The events should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, event1_id), 0);
    assert_int_equal(linvoke_get_node_count(linvoke, event2_id), 0);

    // This connect call will work, because the event with id 0 is not connected to the mock_callback1
    linvoke_connect(linvoke, event1_id, mock_callback1, NULL);

    // This connect call will work, because the event with id 1 is not connected to the mock_callback1
    linvoke_connect(linvoke, event2_id, mock_callback1, NULL);

    // Both events are connected to the mock_callback1, meaning both events have one node connected
    assert_int_equal(linvoke_get_node_count(linvoke, event1_id), 1);
    assert_int_equal(linvoke_get_node_count(linvoke, event2_id), 1);

    // Emitting both events will call mock_callback1 one time each, so 2 total
    expect_function_calls(mock_callback1, 2);
    linvoke_emit(linvoke, event1_id);
    linvoke_emit(linvoke, event2_id);

    linvoke_destroy(linvoke);
}

static void test_multiple_events_different_id_multiple_same_callback(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 0);

    const uint32_t event1_id = 0;
    const uint32_t event2_id = 1;
    linvoke_register_event(linvoke, event1_id);
    linvoke_register_event(linvoke, event2_id);

    // There should be 2 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 2);

    // The events should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, event1_id), 0);
    assert_int_equal(linvoke_get_node_count(linvoke, event2_id), 0);

    // This connect call will work, because the event with id 0
    // (event1_id in this case and event1_id == event2_id) is not connected to the mock_callback1
    linvoke_connect(linvoke, event1_id, mock_callback1, NULL);

    // This connect call will not work, because the event with id 0
    // (event1_id in this case and event1_id == event2_id) is already connected to the mock_callback1
    linvoke_connect(linvoke, event1_id, mock_callback1, NULL);

    // This connect call will work, because the event with id 1
    // (event2_id in this case and event1_id == event2_id) is not connected to the mock_callback1
    linvoke_connect(linvoke, event2_id, mock_callback1, NULL);

    // This connect call will not work, because the event with id 1
    // (event2_id in this case and event1_id == event2_id) is already connected to the mock_callback1
    linvoke_connect(linvoke, event2_id, mock_callback1, NULL);

    // The event with id == event1_id has one node connected
    assert_int_equal(linvoke_get_node_count(linvoke, event1_id), 1);

    // The event with id == event2_id has one node connected
    assert_int_equal(linvoke_get_node_count(linvoke, event2_id), 1);

    // Emitting both events will call mock_callback1 one time each, so 2 total
    expect_function_calls(mock_callback1, 2);
    linvoke_emit(linvoke, event1_id);
    linvoke_emit(linvoke, event2_id);

    linvoke_destroy(linvoke);
}

static void test_multiple_events_different_id_multiple_different_callback(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 0);

    const uint32_t event1_id = 0;
    const uint32_t event2_id = 1;
    linvoke_register_event(linvoke, event1_id);
    linvoke_register_event(linvoke, event2_id);

    // There should be 2 event registered
    assert_int_equal(linvoke_get_registered_event_count(linvoke), 2);

    // The events should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, event1_id), 0);
    assert_int_equal(linvoke_get_node_count(linvoke, event2_id), 0);

    /**
     * Event 1 Connections
     */

    // This connect call will work, because the event with id 0
    // (event1_id in this case and event1_id == event2_id) is not connected to the mock_callback1
    linvoke_connect(linvoke, event1_id, mock_callback1, NULL);

    // This connect call will not work, because the event with id 0
    // (event1_id in this case and event1_id == event2_id) is already connected to the mock_callback1
    linvoke_connect(linvoke, event1_id, mock_callback1, NULL);

    // This connect call will work, because the event with id 0
    // (event1_id in this case and event1_id == event2_id) is not connected to the mock_callback2
    linvoke_connect(linvoke, event1_id, mock_callback2, NULL);

    // This connect call will not work, because the event with id 0
    // (event1_id in this case and event1_id == event2_id) is already connected to the mock_callback2
    linvoke_connect(linvoke, event1_id, mock_callback2, NULL);

    /**
     * Event 2 Connections
     */

    // This connect call will work, because the event with id 0
    // (event2_id in this case and event1_id == event2_id) is not connected to the mock_callback1
    linvoke_connect(linvoke, event2_id, mock_callback1, NULL);

    // This connect call will not work, because the event with id 0
    // (event2_id in this case and event1_id == event2_id) is already connected to the mock_callback1
    linvoke_connect(linvoke, event2_id, mock_callback1, NULL);

    // This connect call will work, because the event with id 0
    // (event2_id in this case and event1_id == event2_id) is not connected to the mock_callback2
    linvoke_connect(linvoke, event2_id, mock_callback2, NULL);

    // This connect call will not work, because the event with id 0
    // (event2_id in this case and event1_id == event2_id) is already connected to the mock_callback2
    linvoke_connect(linvoke, event2_id, mock_callback2, NULL);

    // The event with id == event1_id has one node connected
    assert_int_equal(linvoke_get_node_count(linvoke, event1_id), 2);

    // The event with id == event2_id has one node connected
    assert_int_equal(linvoke_get_node_count(linvoke, event2_id), 2);

    // Emitting event 1 will call mock_callback1 and mock_callback2
    expect_function_calls(mock_callback1, 1);
    expect_function_calls(mock_callback2, 1);
    linvoke_emit(linvoke, event1_id);

    // Emitting event 2 will call mock_callback1 and mock_callback2
    expect_function_calls(mock_callback1, 1);
    expect_function_calls(mock_callback2, 1);
    linvoke_emit(linvoke, event2_id);

    linvoke_destroy(linvoke);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_one_event_one_callback),
        cmocka_unit_test(test_one_event_multiple_same_callbacks),
        cmocka_unit_test(test_one_event_multiple_different_callbacks),
        cmocka_unit_test(test_multiple_events_same_id_one_callback),
        cmocka_unit_test(test_multiple_events_same_id_multiple_same_callback),
        cmocka_unit_test(test_multiple_events_same_id_multiple_different_callback),
        cmocka_unit_test(test_multiple_events_different_id_one_callback),
        cmocka_unit_test(test_multiple_events_different_id_multiple_same_callback),
        cmocka_unit_test(test_multiple_events_different_id_multiple_different_callback),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
