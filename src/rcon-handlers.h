#ifndef RCONHANDLERS_H
#define RCONHANDLERS_H

#include <jansson.h>

#include "mongoose.h"
#include "rcon-server.h"

#define MAX_RCON_HANDLERS 32

typedef int (*handler_func)(struct mg_connection *conn, json_t* jreq, json_t* jrsp);

//For each handler URI, define a struct for it and "add" it to our global handlers.
struct rcon_handler {
	//URI for this handler
	char * uri_path;
	//function to handle this URI, takes JSON (or NULL), returns JSON.
	handler_func handler;
};

int root_handler(struct mg_connection *conn, json_t* jreq, json_t* jrsp);

#endif // RCONHANDLERS_H
