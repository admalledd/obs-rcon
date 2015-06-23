#ifndef RCONMAIN_H
#define RCONMAIN_H

#include <util/threading.h>
#include <obs-module.h>

#include "mongoose.h"

#include "rcon-server.h"

#define blog(level, msg, ...) blog(level, "** obs-rcon: " msg, ##__VA_ARGS__)

#define debug(msg, ...) blog(LOG_DEBUG, msg, ##__VA_ARGS__)
#define info(msg, ...) blog(LOG_INFO, msg, ##__VA_ARGS__)
#define warning(msg, ...) blog(LOG_WARNING, msg, ##__VA_ARGS__)
#define error(msg, ...) blog(LOG_ERROR, msg, ##__VA_ARGS__)


typedef struct
{
	//mongoose server stuff
	struct mg_server *server;
	pthread_t server_thread;
	bool run_thread;

} rcon_data_t;

extern rcon_data_t rcon_data;

#endif // RCONMAIN_H
