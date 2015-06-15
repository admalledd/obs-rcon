#ifndef RCONSERVER_H
#define RCONSERVER_H


void server_start();

void server_stop();

static int ev_handler(struct mg_connection *conn, enum mg_event ev);

void *thread_runner(void* init_data);

#endif // RCONSERVER_H
