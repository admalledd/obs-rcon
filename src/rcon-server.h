#ifndef RCONSERVER_H
#define RCONSERVER_H

#include <stdbool.h>
#include <jansson.h>

typedef int (handler_func)(struct mg_connection *conn, json_t* jreq, json_t* jrsp);

int ev_handler(struct mg_connection *conn, enum mg_event ev);

bool cmpjstr(json_t* jstr, const char* cstr);

void *thread_runner(void* init_data);

void server_start();

void server_stop();

#endif // RCONSERVER_H
