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

void mock_node1(linvoke_event_s *event)
{
    function_called();
}

void mock_node2(linvoke_event_s *event)
{
    function_called();
}

void mock_node_with_data(linvoke_event_s *event)
{
    const uint32_t port_id = linvoke_event_get_port_id(event);
    const char *data = *((const char **)linvoke_event_get_user_data(event));

    // This mock node is only used for testing the data inside the event, and is only called
    // by one function. Therefore, we can directly test the port_id and user data here.
    assert_int_equal(port_id, 36);
    assert_string_equal(data, "Some string data");

    function_called();
}

void mock_node_with_overriden_data(linvoke_event_s *event)
{
    const uint32_t port_id = linvoke_event_get_port_id(event);
    const char *data = *((const char **)linvoke_event_get_user_data(event));

    // This mock node is only used for testing the data inside the event, and is only called
    // by one function. Therefore, we can directly test the port_id and user data here.
    assert_int_equal(port_id, 28);
    assert_string_equal(data, "Some overriden string data");

    function_called();
}

static void test_one_port_one_node(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 0);

    const uint32_t port_id = 0;
    linvoke_register_port(linvoke, port_id);

    // There should be 1 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 1);

    // The port should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, port_id), 0);

    linvoke_connect(linvoke, port_id, mock_node1);

    // The port should have 1 node connected
    assert_int_equal(linvoke_get_node_count(linvoke, port_id), 1);

    // Since only 1 node is connected, when we emit the port, we expect only 1 node
    expect_function_calls(mock_node1, 1);

    linvoke_emit(linvoke, port_id);

    linvoke_destroy(linvoke);
}

static void test_one_port_multiple_same_nodes(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 0);

    const uint32_t port_id = 0;
    linvoke_register_port(linvoke, port_id);

    // There should be 1 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 1);

    // The port should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, port_id), 0);

    linvoke_connect(linvoke, port_id, mock_node1);
    linvoke_connect(linvoke, port_id, mock_node1); // This one will fail since it's already connected

    // The port should have 1 node connected, because the second linvoke_connect
    // should not work, since the node is already connected
    assert_int_equal(linvoke_get_node_count(linvoke, port_id), 1);

    // Since only 1 node is connected, when we emit the port, we expect only 1 node
    expect_function_calls(mock_node1, 1);

    linvoke_emit(linvoke, port_id);

    linvoke_destroy(linvoke);
}

static void test_one_port_multiple_different_nodes(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 0);

    const uint32_t port_id = 0;
    linvoke_register_port(linvoke, port_id);

    // There should be 1 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 1);

    // The port should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, port_id), 0);

    linvoke_connect(linvoke, port_id, mock_node1);
    linvoke_connect(linvoke, port_id, mock_node2);

    // The port should have 2 node connected
    assert_int_equal(linvoke_get_node_count(linvoke, port_id), 2);

    // The port has 2 nodes, so we expect 2 node functions to be called
    expect_function_calls(mock_node1, 1);
    expect_function_calls(mock_node2, 1);

    linvoke_emit(linvoke, port_id);

    linvoke_destroy(linvoke);
}

static void test_multiple_ports_same_id_one_node(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 0);

    const uint32_t port1_id = 0;
    const uint32_t port2_id = 0;
    linvoke_register_port(linvoke, port1_id);
    linvoke_register_port(linvoke, port2_id);

    // There should be 1 port registered, because we tried to register the same port id twice
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 1);

    // The ports should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, port1_id), 0);
    assert_int_equal(linvoke_get_node_count(linvoke, port2_id), 0);

    // This connect call will work, because the port with id 0 is not connected to the mock_node1
    linvoke_connect(linvoke, port1_id, mock_node1);

    // This connect call will not work, because the port with id 0 is already connected to the mock_node1
    linvoke_connect(linvoke, port2_id, mock_node1);

    // Since both ports have the same id, and we already registered the port id 0 and connected
    // the mock_node1 to that id, we expect the port with id 0 to have 1 node connected
    assert_int_equal(linvoke_get_node_count(linvoke, port1_id), 1);
    assert_int_equal(linvoke_get_node_count(linvoke, port2_id), 1);

    // Because both nodes have the same id, emitting the ports with id 0
    // (both port1_id and port2_id) will call the mock_node1 twice
    expect_function_calls(mock_node1, 2);

    linvoke_emit(linvoke, port1_id);
    linvoke_emit(linvoke, port2_id);

    linvoke_destroy(linvoke);
}

static void test_multiple_ports_same_id_multiple_same_node(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 0);

    const uint32_t port1_id = 0;
    const uint32_t port2_id = 0;
    linvoke_register_port(linvoke, port1_id);
    linvoke_register_port(linvoke, port2_id);

    // There should be 1 port registered, because we tried to register the same port id twice
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 1);

    // The ports should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, port1_id), 0);
    assert_int_equal(linvoke_get_node_count(linvoke, port2_id), 0);

    // This connect call will work, because the port with id 0
    // (port1_id in this case and port1_id == port2_id) is not connected to the mock_node1
    linvoke_connect(linvoke, port1_id, mock_node1);

    // This connect call will not work, because the port with id 0
    // (port1_id in this case and port1_id == port2_id) is already connected to the mock_node1
    linvoke_connect(linvoke, port1_id, mock_node1);

    // This connect call will not work, because the port with id 0
    // (port2_id in this case and port1_id == port2_id) is already connected to the mock_node1
    linvoke_connect(linvoke, port2_id, mock_node1);

    // This connect call will not work, because the port with id 0
    // (port2_id in this case and port1_id == port2_id) is already connected to the mock_node1
    linvoke_connect(linvoke, port2_id, mock_node1);

    // Since both ports have the same id, and we already registered the port id 0 and connected
    // the mock_node1 to that id, we expect the port with id 0 to have 1 node connected
    assert_int_equal(linvoke_get_node_count(linvoke, port1_id), 1);
    assert_int_equal(linvoke_get_node_count(linvoke, port2_id), 1);

    // Because both nodes have the same id, emitting the ports with id 0
    // (both port1_id and port2_id) will call the mock_node1 twice
    expect_function_calls(mock_node1, 2);

    linvoke_emit(linvoke, port1_id);
    linvoke_emit(linvoke, port2_id);

    linvoke_destroy(linvoke);
}

static void test_multiple_ports_same_id_multiple_different_node(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 0);

    const uint32_t port1_id = 0;
    const uint32_t port2_id = 0;
    linvoke_register_port(linvoke, port1_id);
    linvoke_register_port(linvoke, port2_id);

    // There should be 1 port registered, because we tried to register the same port id twice
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 1);

    // The ports should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, port1_id), 0);
    assert_int_equal(linvoke_get_node_count(linvoke, port2_id), 0);

    /**
     * Port 1 Connections
     */

    // This connect call will work, because the port with id 0
    // (port1_id in this case and port1_id == port2_id) is not connected to the mock_node1
    linvoke_connect(linvoke, port1_id, mock_node1);

    // This connect call will not work, because the port with id 0
    // (port1_id in this case and port1_id == port2_id) is already connected to the mock_node1
    linvoke_connect(linvoke, port1_id, mock_node1);

    // This connect call will work, because the port with id 0
    // (port1_id in this case and port1_id == port2_id) is not connected to the mock_node2
    linvoke_connect(linvoke, port1_id, mock_node2);

    // This connect call will not work, because the port with id 0
    // (port1_id in this case and port1_id == port2_id) is already connected to the mock_node2
    linvoke_connect(linvoke, port1_id, mock_node2);

    /**
     * Port 2 Connections
     */

    // This connect call will work, because the port with id 0
    // (port2_id in this case and port1_id == port2_id) is not connected to the mock_node1
    linvoke_connect(linvoke, port2_id, mock_node1);

    // This connect call will not work, because the port with id 0
    // (port2_id in this case and port1_id == port2_id) is already connected to the mock_node1
    linvoke_connect(linvoke, port2_id, mock_node1);

    // This connect call will work, because the port with id 0
    // (port2_id in this case and port1_id == port2_id) is not connected to the mock_node2
    linvoke_connect(linvoke, port2_id, mock_node2);

    // This connect call will not work, because the port with id 0
    // (port2_id in this case and port1_id == port2_id) is already connected to the mock_node2
    linvoke_connect(linvoke, port2_id, mock_node2);

    // Since both ports have the same id, and we already registered the port id 0 and connected
    // the mock_node1 and mock_node2 to that id, we expect the port with id 0
    // (both port1_id and port2_id ) to have 2 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, port1_id), 2);
    assert_int_equal(linvoke_get_node_count(linvoke, port2_id), 2);

    // Because both nodes have the same id, emitting the port with id == port1_id
    // will call the mock_node1 and mock_node2
    expect_function_calls(mock_node1, 1);
    expect_function_calls(mock_node2, 1);

    linvoke_emit(linvoke, port1_id);

    // Because both nodes have the same id, emitting the port with id == port2_id
    // will call the mock_node1 and mock_node2
    expect_function_calls(mock_node1, 1);
    expect_function_calls(mock_node2, 1);

    linvoke_emit(linvoke, port2_id);

    linvoke_destroy(linvoke);
}

static void test_multiple_ports_different_id_one_node(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 0);

    const uint32_t port1_id = 0;
    const uint32_t port2_id = 1;
    linvoke_register_port(linvoke, port1_id);
    linvoke_register_port(linvoke, port2_id);

    // There should be 2 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 2);

    // The ports should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, port1_id), 0);
    assert_int_equal(linvoke_get_node_count(linvoke, port2_id), 0);

    // This connect call will work, because the port with id 0 is not connected to the mock_node1
    linvoke_connect(linvoke, port1_id, mock_node1);

    // This connect call will work, because the port with id 1 is not connected to the mock_node1
    linvoke_connect(linvoke, port2_id, mock_node1);

    // Both ports are connected to the mock_node1, meaning both ports have one node connected
    assert_int_equal(linvoke_get_node_count(linvoke, port1_id), 1);
    assert_int_equal(linvoke_get_node_count(linvoke, port2_id), 1);

    // Emitting both ports will call mock_node1 one time each, so 2 total
    expect_function_calls(mock_node1, 2);
    linvoke_emit(linvoke, port1_id);
    linvoke_emit(linvoke, port2_id);

    linvoke_destroy(linvoke);
}

static void test_multiple_ports_different_id_multiple_same_node(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 0);

    const uint32_t port1_id = 0;
    const uint32_t port2_id = 1;
    linvoke_register_port(linvoke, port1_id);
    linvoke_register_port(linvoke, port2_id);

    // There should be 2 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 2);

    // The ports should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, port1_id), 0);
    assert_int_equal(linvoke_get_node_count(linvoke, port2_id), 0);

    // This connect call will work, because the port with id 0
    // (port1_id in this case and port1_id == port2_id) is not connected to the mock_node1
    linvoke_connect(linvoke, port1_id, mock_node1);

    // This connect call will not work, because the port with id 0
    // (port1_id in this case and port1_id == port2_id) is already connected to the mock_node1
    linvoke_connect(linvoke, port1_id, mock_node1);

    // This connect call will work, because the port with id 1
    // (port2_id in this case and port1_id == port2_id) is not connected to the mock_node1
    linvoke_connect(linvoke, port2_id, mock_node1);

    // This connect call will not work, because the port with id 1
    // (port2_id in this case and port1_id == port2_id) is already connected to the mock_node1
    linvoke_connect(linvoke, port2_id, mock_node1);

    // The port with id == port1_id has one node connected
    assert_int_equal(linvoke_get_node_count(linvoke, port1_id), 1);

    // The port with id == port2_id has one node connected
    assert_int_equal(linvoke_get_node_count(linvoke, port2_id), 1);

    // Emitting both ports will call mock_node1 one time each, so 2 total
    expect_function_calls(mock_node1, 2);
    linvoke_emit(linvoke, port1_id);
    linvoke_emit(linvoke, port2_id);

    linvoke_destroy(linvoke);
}

static void test_multiple_ports_different_id_multiple_different_node(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 0);

    const uint32_t port1_id = 0;
    const uint32_t port2_id = 1;
    linvoke_register_port(linvoke, port1_id);
    linvoke_register_port(linvoke, port2_id);

    // There should be 2 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 2);

    // The ports should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, port1_id), 0);
    assert_int_equal(linvoke_get_node_count(linvoke, port2_id), 0);

    /**
     * Port 1 Connections
     */

    // This connect call will work, because the port with id 0
    // (port1_id in this case and port1_id == port2_id) is not connected to the mock_node1
    linvoke_connect(linvoke, port1_id, mock_node1);

    // This connect call will not work, because the port with id 0
    // (port1_id in this case and port1_id == port2_id) is already connected to the mock_node1
    linvoke_connect(linvoke, port1_id, mock_node1);

    // This connect call will work, because the port with id 0
    // (port1_id in this case and port1_id == port2_id) is not connected to the mock_node2
    linvoke_connect(linvoke, port1_id, mock_node2);

    // This connect call will not work, because the port with id 0
    // (port1_id in this case and port1_id == port2_id) is already connected to the mock_node2
    linvoke_connect(linvoke, port1_id, mock_node2);

    /**
     * Port 2 Connections
     */

    // This connect call will work, because the port with id 0
    // (port2_id in this case and port1_id == port2_id) is not connected to the mock_node1
    linvoke_connect(linvoke, port2_id, mock_node1);

    // This connect call will not work, because the port with id 0
    // (port2_id in this case and port1_id == port2_id) is already connected to the mock_node1
    linvoke_connect(linvoke, port2_id, mock_node1);

    // This connect call will work, because the port with id 0
    // (port2_id in this case and port1_id == port2_id) is not connected to the mock_node2
    linvoke_connect(linvoke, port2_id, mock_node2);

    // This connect call will not work, because the port with id 0
    // (port2_id in this case and port1_id == port2_id) is already connected to the mock_node2
    linvoke_connect(linvoke, port2_id, mock_node2);

    // The port with id == port1_id has one node connected
    assert_int_equal(linvoke_get_node_count(linvoke, port1_id), 2);

    // The port with id == port2_id has one node connected
    assert_int_equal(linvoke_get_node_count(linvoke, port2_id), 2);

    // Emitting port 1 will call mock_node1 and mock_node2
    expect_function_calls(mock_node1, 1);
    expect_function_calls(mock_node2, 1);
    linvoke_emit(linvoke, port1_id);

    // Emitting port 2 will call mock_node1 and mock_node2
    expect_function_calls(mock_node1, 1);
    expect_function_calls(mock_node2, 1);
    linvoke_emit(linvoke, port2_id);

    linvoke_destroy(linvoke);
}

static void test_one_port_one_node_with_data(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 0);

    const uint32_t port_id = 36;
    linvoke_register_port(linvoke, port_id);

    // There should be 1 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 1);

    // The port should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, port_id), 0);

    const char *event_data = "Some string data";
    linvoke_connect_with_data(linvoke, port_id, mock_node_with_data, &event_data);

    // The port should have 1 node connected
    assert_int_equal(linvoke_get_node_count(linvoke, port_id), 1);

    // Since only 1 node is connected, when we emit the port, we expect only 1 node
    expect_function_calls(mock_node_with_data, 1);

    linvoke_emit(linvoke, port_id);

    linvoke_destroy(linvoke);
}

static void test_one_port_one_node_with_data_override(void **state)
{
    (void) state; // unused

    linvoke_s *linvoke = linvoke_create();

    // There should be 0 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 0);

    const uint32_t port_id = 28;
    linvoke_register_port(linvoke, port_id);

    // There should be 1 port registered
    assert_int_equal(linvoke_get_registered_port_count(linvoke), 1);

    // The port should have 0 nodes connected
    assert_int_equal(linvoke_get_node_count(linvoke, port_id), 0);

    const char *event_data = "Some string data";
    linvoke_connect_with_data(linvoke, port_id, mock_node_with_overriden_data, &event_data);

    // The port should have 1 node connected
    assert_int_equal(linvoke_get_node_count(linvoke, port_id), 1);

    // Since only 1 node is connected, when we emit the port, we expect only 1 node
    expect_function_calls(mock_node_with_overriden_data, 1);

    const char *event_data_overriden = "Some overriden string data";
    linvoke_emit_with_data(linvoke, port_id, &event_data_overriden);

    linvoke_destroy(linvoke);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_one_port_one_node),
        cmocka_unit_test(test_one_port_one_node_with_data),
        cmocka_unit_test(test_one_port_one_node_with_data_override),
        cmocka_unit_test(test_one_port_multiple_same_nodes),
        cmocka_unit_test(test_one_port_multiple_different_nodes),
        cmocka_unit_test(test_multiple_ports_same_id_one_node),
        cmocka_unit_test(test_multiple_ports_same_id_multiple_same_node),
        cmocka_unit_test(test_multiple_ports_same_id_multiple_different_node),
        cmocka_unit_test(test_multiple_ports_different_id_one_node),
        cmocka_unit_test(test_multiple_ports_different_id_multiple_same_node),
        cmocka_unit_test(test_multiple_ports_different_id_multiple_different_node),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
