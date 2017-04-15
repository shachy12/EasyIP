#include <net/device.h>
#include <net/conn.h>
#include <net/utils.h>

CONNECTION_t CONN__connections_list_g[EASY_IP_CONNECTION_COUNT] = {{0}};
RECORDS_ALLOCATOR_t CONN__records_allocator_g = {0};

CONNECTION_t *CONN__create_socket(CONN__type_t type)
{
    CONNECTION_t *connection = (CONNECTION_t *)RECORDS_ALLOCATOR__allocate(&CONN__records_allocator_g);
    ON_NULL_GOTO(connection, Exit);

    IF_FALSE_GOTO(EASY_IP__create_signal(&connection->signal),
                  Cleanup);
    connection->type = type;
    goto Exit;
Cleanup:
    RECORDS_ALLOCATOR__free(connection);
    connection = NULL;
Exit:
    return connection;
}

void CONN__listen(CONNECTION_t *self, uint16_t port)
{
    self->source_port = port;
}

uint16_t CONN__recv(CONNECTION_t *self, uint8_t *buffer, uint16_t length)
{

}

uint16_t CONN__send(CONNECTION_t *self, uint8_t *buffer, uint16_t length)
{

}

uint16_t CONN__sendto(CONNECTION_t *self, uint8_t *buffer, uint16_t length, IP_ADDRESS_t destination_ip)
{

}
