#ifndef RCONMAIN_H
#define RCONMAIN_H

#include <util/threading.h>
#include <util/darray.h>
#include <obs-module.h>

#include "mongoose.h"

#include "rcon-server.h"

#define blog(level, msg, ...) blog(level, "** obs-rcon: " msg, ##__VA_ARGS__)

#define debug(msg, ...) blog(LOG_DEBUG, msg, ##__VA_ARGS__)
#define info(msg, ...) blog(LOG_INFO, msg, ##__VA_ARGS__)
#define warning(msg, ...) blog(LOG_WARNING, msg, ##__VA_ARGS__)
#define error(msg, ...) blog(LOG_ERROR, msg, ##__VA_ARGS__)


struct rcon_handler {
	handler_func* handle_func;
	char* action;
};

struct rcon_data_t {
	//mongoose server stuff
	struct mg_server *server;
	pthread_t server_thread;
	bool run_thread;

	DARRAY(struct rcon_handler)	plugin_handlers;
};

extern struct rcon_data_t rcon_data;

#endif // RCONMAIN_H
