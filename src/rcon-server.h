#ifndef RCONSERVER_H
#define RCONSERVER_H

#include <stdbool.h>

int ev_handler(struct mg_connection *conn, enum mg_event ev);

void *thread_runner(void* init_data);

void server_start();

void server_stop();

#endif // RCONSERVER_H
