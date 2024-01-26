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

void mock_slot1(linvoke_event_s *event)
{
    (void) event; // Unused
    function_called();
}

void mock_slot2(linvoke_event_s *event)
{
    (void) event; // Unused
    function_called();
}

void mock_slot_with_data(linvoke_event_s *event)
{
    const linvoke_signal signal_id = linvoke_event_get_signal_id(event);
    const char *data = *((const char **)linvoke_event_get_user_data(event));

    // This mock slot is only used for testing the data inside the event, and is only called
    // by one function. Therefore, we can directly test the signal_id and user data here.
    assert_int_equal(signal_id, 36);
    assert_string_equal(data, "Some string data");

    function_called();
}

static void test_one_signal_one_slot(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 signals registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 0);

    const linvoke_signal signal_id = 0;
    linvoke_register_signal(linvoke, signal_id);

    // There should be 1 signal registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 1);

    // The signal should have 0 slots connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal_id), 0);

    linvoke_connect(linvoke, signal_id, mock_slot1);

    // The signal should have 1 slot connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal_id), 1);

    // Since only 1 slot is connected, when we emit the signal, we expect only 1 slot
    expect_function_calls(mock_slot1, 1);

    linvoke_emit(linvoke, signal_id, NULL);

    linvoke_destroy(linvoke);
}

static void test_one_signal_multiple_same_slots(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 signals registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 0);

    const linvoke_signal signal_id = 0;
    linvoke_register_signal(linvoke, signal_id);

    // There should be 1 signal registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 1);

    // The signal should have 0 slots connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal_id), 0);

    linvoke_connect(linvoke, signal_id, mock_slot1);
    linvoke_connect(linvoke, signal_id, mock_slot1); // This one will fail since it's already connected

    // The signal should have 1 slot connected, because the second linvoke_connect
    // should not work, since the slot is already connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal_id), 1);

    // Since only 1 slot is connected, when we emit the signal, we expect only 1 slot
    expect_function_calls(mock_slot1, 1);

    linvoke_emit(linvoke, signal_id, NULL);

    linvoke_destroy(linvoke);
}

static void test_one_signal_multiple_different_slots(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 signals registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 0);

    const linvoke_signal signal_id = 0;
    linvoke_register_signal(linvoke, signal_id);

    // There should be 1 signal registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 1);

    // The signal should have 0 slots connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal_id), 0);

    linvoke_connect(linvoke, signal_id, mock_slot1);
    linvoke_connect(linvoke, signal_id, mock_slot2);

    // The signal should have 2 slot connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal_id), 2);

    // The signal has 2 slots, so we expect 2 slot functions to be called
    expect_function_calls(mock_slot1, 1);
    expect_function_calls(mock_slot2, 1);

    linvoke_emit(linvoke, signal_id, NULL);

    linvoke_destroy(linvoke);
}

static void test_multiple_signals_same_id_one_slot(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 signals registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 0);

    const linvoke_signal signal1_id = 0;
    const linvoke_signal signal2_id = 0;
    linvoke_register_signal(linvoke, signal1_id);
    linvoke_register_signal(linvoke, signal2_id);

    // There should be 1 signal registered, because we tried to register the same signal id twice
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 1);

    // The signals should have 0 slots connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal1_id), 0);
    assert_int_equal(linvoke_get_slot_count(linvoke, signal2_id), 0);

    // This connect call will work, because the signal with id 0 is not connected to the mock_slot1
    linvoke_connect(linvoke, signal1_id, mock_slot1);

    // This connect call will not work, because the signal with id 0 is already connected to the mock_slot1
    linvoke_connect(linvoke, signal2_id, mock_slot1);

    // Since both signals have the same id, and we already registered the signal id 0 and connected
    // the mock_slot1 to that id, we expect the signal with id 0 to have 1 slot connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal1_id), 1);
    assert_int_equal(linvoke_get_slot_count(linvoke, signal2_id), 1);

    // Because both slots have the same id, emitting the signals with id 0
    // (both signal1_id and signal2_id) will call the mock_slot1 twice
    expect_function_calls(mock_slot1, 2);

    linvoke_emit(linvoke, signal1_id, NULL);
    linvoke_emit(linvoke, signal2_id, NULL);

    linvoke_destroy(linvoke);
}

static void test_multiple_signals_same_id_multiple_same_slot(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 signals registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 0);

    const linvoke_signal signal1_id = 0;
    const linvoke_signal signal2_id = 0;
    linvoke_register_signal(linvoke, signal1_id);
    linvoke_register_signal(linvoke, signal2_id);

    // There should be 1 signal registered, because we tried to register the same signal id twice
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 1);

    // The signals should have 0 slots connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal1_id), 0);
    assert_int_equal(linvoke_get_slot_count(linvoke, signal2_id), 0);

    // This connect call will work, because the signal with id 0
    // (signal1_id in this case and signal1_id == signal2_id) is not connected to the mock_slot1
    linvoke_connect(linvoke, signal1_id, mock_slot1);

    // This connect call will not work, because the signal with id 0
    // (signal1_id in this case and signal1_id == signal2_id) is already connected to the mock_slot1
    linvoke_connect(linvoke, signal1_id, mock_slot1);

    // This connect call will not work, because the signal with id 0
    // (signal2_id in this case and signal1_id == signal2_id) is already connected to the mock_slot1
    linvoke_connect(linvoke, signal2_id, mock_slot1);

    // This connect call will not work, because the signal with id 0
    // (signal2_id in this case and signal1_id == signal2_id) is already connected to the mock_slot1
    linvoke_connect(linvoke, signal2_id, mock_slot1);

    // Since both signals have the same id, and we already registered the signal id 0 and connected
    // the mock_slot1 to that id, we expect the signal with id 0 to have 1 slot connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal1_id), 1);
    assert_int_equal(linvoke_get_slot_count(linvoke, signal2_id), 1);

    // Because both slots have the same id, emitting the signals with id 0
    // (both signal1_id and signal2_id) will call the mock_slot1 twice
    expect_function_calls(mock_slot1, 2);

    linvoke_emit(linvoke, signal1_id, NULL);
    linvoke_emit(linvoke, signal2_id, NULL);

    linvoke_destroy(linvoke);
}

static void test_multiple_signals_same_id_multiple_different_slot(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 signals registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 0);

    const linvoke_signal signal1_id = 0;
    const linvoke_signal signal2_id = 0;
    linvoke_register_signal(linvoke, signal1_id);
    linvoke_register_signal(linvoke, signal2_id);

    // There should be 1 signal registered, because we tried to register the same signal id twice
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 1);

    // The signals should have 0 slots connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal1_id), 0);
    assert_int_equal(linvoke_get_slot_count(linvoke, signal2_id), 0);

    /**
     * Signal 1 Connections
     */

    // This connect call will work, because the signal with id 0
    // (signal1_id in this case and signal1_id == signal2_id) is not connected to the mock_slot1
    linvoke_connect(linvoke, signal1_id, mock_slot1);

    // This connect call will not work, because the signal with id 0
    // (signal1_id in this case and signal1_id == signal2_id) is already connected to the mock_slot1
    linvoke_connect(linvoke, signal1_id, mock_slot1);

    // This connect call will work, because the signal with id 0
    // (signal1_id in this case and signal1_id == signal2_id) is not connected to the mock_slot2
    linvoke_connect(linvoke, signal1_id, mock_slot2);

    // This connect call will not work, because the signal with id 0
    // (signal1_id in this case and signal1_id == signal2_id) is already connected to the mock_slot2
    linvoke_connect(linvoke, signal1_id, mock_slot2);

    /**
     * Signal 2 Connections
     */

    // This connect call will work, because the signal with id 0
    // (signal2_id in this case and signal1_id == signal2_id) is not connected to the mock_slot1
    linvoke_connect(linvoke, signal2_id, mock_slot1);

    // This connect call will not work, because the signal with id 0
    // (signal2_id in this case and signal1_id == signal2_id) is already connected to the mock_slot1
    linvoke_connect(linvoke, signal2_id, mock_slot1);

    // This connect call will work, because the signal with id 0
    // (signal2_id in this case and signal1_id == signal2_id) is not connected to the mock_slot2
    linvoke_connect(linvoke, signal2_id, mock_slot2);

    // This connect call will not work, because the signal with id 0
    // (signal2_id in this case and signal1_id == signal2_id) is already connected to the mock_slot2
    linvoke_connect(linvoke, signal2_id, mock_slot2);

    // Since both signals have the same id, and we already registered the signal id 0 and connected
    // the mock_slot1 and mock_slot2 to that id, we expect the signal with id 0
    // (both signal1_id and signal2_id ) to have 2 slots connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal1_id), 2);
    assert_int_equal(linvoke_get_slot_count(linvoke, signal2_id), 2);

    // Because both slots have the same id, emitting the signal with id == signal1_id
    // will call the mock_slot1 and mock_slot2
    expect_function_calls(mock_slot1, 1);
    expect_function_calls(mock_slot2, 1);

    linvoke_emit(linvoke, signal1_id, NULL);

    // Because both slots have the same id, emitting the signal with id == signal2_id
    // will call the mock_slot1 and mock_slot2
    expect_function_calls(mock_slot1, 1);
    expect_function_calls(mock_slot2, 1);

    linvoke_emit(linvoke, signal2_id, NULL);

    linvoke_destroy(linvoke);
}

static void test_multiple_signals_different_id_one_slot(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 signals registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 0);

    const linvoke_signal signal1_id = 0;
    const linvoke_signal signal2_id = 1;
    linvoke_register_signal(linvoke, signal1_id);
    linvoke_register_signal(linvoke, signal2_id);

    // There should be 2 signal registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 2);

    // The signals should have 0 slots connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal1_id), 0);
    assert_int_equal(linvoke_get_slot_count(linvoke, signal2_id), 0);

    // This connect call will work, because the signal with id 0 is not connected to the mock_slot1
    linvoke_connect(linvoke, signal1_id, mock_slot1);

    // This connect call will work, because the signal with id 1 is not connected to the mock_slot1
    linvoke_connect(linvoke, signal2_id, mock_slot1);

    // Both signals are connected to the mock_slot1, meaning both signals have one slot connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal1_id), 1);
    assert_int_equal(linvoke_get_slot_count(linvoke, signal2_id), 1);

    // Emitting both signals will call mock_slot1 one time each, so 2 total
    expect_function_calls(mock_slot1, 2);
    linvoke_emit(linvoke, signal1_id, NULL);
    linvoke_emit(linvoke, signal2_id, NULL);

    linvoke_destroy(linvoke);
}

static void test_multiple_signals_different_id_multiple_same_slot(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 signals registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 0);

    const linvoke_signal signal1_id = 0;
    const linvoke_signal signal2_id = 1;
    linvoke_register_signal(linvoke, signal1_id);
    linvoke_register_signal(linvoke, signal2_id);

    // There should be 2 signal registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 2);

    // The signals should have 0 slots connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal1_id), 0);
    assert_int_equal(linvoke_get_slot_count(linvoke, signal2_id), 0);

    // This connect call will work, because the signal with id 0
    // (signal1_id in this case and signal1_id == signal2_id) is not connected to the mock_slot1
    linvoke_connect(linvoke, signal1_id, mock_slot1);

    // This connect call will not work, because the signal with id 0
    // (signal1_id in this case and signal1_id == signal2_id) is already connected to the mock_slot1
    linvoke_connect(linvoke, signal1_id, mock_slot1);

    // This connect call will work, because the signal with id 1
    // (signal2_id in this case and signal1_id == signal2_id) is not connected to the mock_slot1
    linvoke_connect(linvoke, signal2_id, mock_slot1);

    // This connect call will not work, because the signal with id 1
    // (signal2_id in this case and signal1_id == signal2_id) is already connected to the mock_slot1
    linvoke_connect(linvoke, signal2_id, mock_slot1);

    // The signal with id == signal1_id has one slot connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal1_id), 1);

    // The signal with id == signal2_id has one slot connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal2_id), 1);

    // Emitting both signals will call mock_slot1 one time each, so 2 total
    expect_function_calls(mock_slot1, 2);
    linvoke_emit(linvoke, signal1_id, NULL);
    linvoke_emit(linvoke, signal2_id, NULL);

    linvoke_destroy(linvoke);
}

static void test_multiple_signals_different_id_multiple_different_slot(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 signals registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 0);

    const linvoke_signal signal1_id = 0;
    const linvoke_signal signal2_id = 1;
    linvoke_register_signal(linvoke, signal1_id);
    linvoke_register_signal(linvoke, signal2_id);

    // There should be 2 signal registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 2);

    // The signals should have 0 slots connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal1_id), 0);
    assert_int_equal(linvoke_get_slot_count(linvoke, signal2_id), 0);

    /**
     * Signal 1 Connections
     */

    // This connect call will work, because the signal with id 0
    // (signal1_id in this case and signal1_id == signal2_id) is not connected to the mock_slot1
    linvoke_connect(linvoke, signal1_id, mock_slot1);

    // This connect call will not work, because the signal with id 0
    // (signal1_id in this case and signal1_id == signal2_id) is already connected to the mock_slot1
    linvoke_connect(linvoke, signal1_id, mock_slot1);

    // This connect call will work, because the signal with id 0
    // (signal1_id in this case and signal1_id == signal2_id) is not connected to the mock_slot2
    linvoke_connect(linvoke, signal1_id, mock_slot2);

    // This connect call will not work, because the signal with id 0
    // (signal1_id in this case and signal1_id == signal2_id) is already connected to the mock_slot2
    linvoke_connect(linvoke, signal1_id, mock_slot2);

    /**
     * Signal 2 Connections
     */

    // This connect call will work, because the signal with id 0
    // (signal2_id in this case and signal1_id == signal2_id) is not connected to the mock_slot1
    linvoke_connect(linvoke, signal2_id, mock_slot1);

    // This connect call will not work, because the signal with id 0
    // (signal2_id in this case and signal1_id == signal2_id) is already connected to the mock_slot1
    linvoke_connect(linvoke, signal2_id, mock_slot1);

    // This connect call will work, because the signal with id 0
    // (signal2_id in this case and signal1_id == signal2_id) is not connected to the mock_slot2
    linvoke_connect(linvoke, signal2_id, mock_slot2);

    // This connect call will not work, because the signal with id 0
    // (signal2_id in this case and signal1_id == signal2_id) is already connected to the mock_slot2
    linvoke_connect(linvoke, signal2_id, mock_slot2);

    // The signal with id == signal1_id has one slot connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal1_id), 2);

    // The signal with id == signal2_id has one slot connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal2_id), 2);

    // Emitting signal 1 will call mock_slot1 and mock_slot2
    expect_function_calls(mock_slot1, 1);
    expect_function_calls(mock_slot2, 1);
    linvoke_emit(linvoke, signal1_id, NULL);

    // Emitting signal 2 will call mock_slot1 and mock_slot2
    expect_function_calls(mock_slot1, 1);
    expect_function_calls(mock_slot2, 1);
    linvoke_emit(linvoke, signal2_id, NULL);

    linvoke_destroy(linvoke);
}

static void test_one_signal_one_slot_with_data(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 signals registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 0);

    const linvoke_signal signal_id = 36;
    linvoke_register_signal(linvoke, signal_id);

    // There should be 1 signal registered
    assert_int_equal(linvoke_get_registered_signal_count(linvoke), 1);

    // The signal should have 0 slots connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal_id), 0);

    linvoke_connect(linvoke, signal_id, mock_slot_with_data);

    // The signal should have 1 slot connected
    assert_int_equal(linvoke_get_slot_count(linvoke, signal_id), 1);

    // Since only 1 slot is connected, when we emit the signal, we expect only 1 slot
    expect_function_calls(mock_slot_with_data, 1);

    const char *event_data = "Some string data";
    linvoke_emit(linvoke, signal_id, &event_data);

    linvoke_destroy(linvoke);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_one_signal_one_slot),
        cmocka_unit_test(test_one_signal_one_slot_with_data),
        cmocka_unit_test(test_one_signal_multiple_same_slots),
        cmocka_unit_test(test_one_signal_multiple_different_slots),
        cmocka_unit_test(test_multiple_signals_same_id_one_slot),
        cmocka_unit_test(test_multiple_signals_same_id_multiple_same_slot),
        cmocka_unit_test(test_multiple_signals_same_id_multiple_different_slot),
        cmocka_unit_test(test_multiple_signals_different_id_one_slot),
        cmocka_unit_test(test_multiple_signals_different_id_multiple_same_slot),
        cmocka_unit_test(test_multiple_signals_different_id_multiple_different_slot),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
