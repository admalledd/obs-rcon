#ifndef RCONHANDLERS_H
#define RCONHANDLERS_H

#include <jansson.h>
#include "mongoose.h"

int handle_version(struct mg_connection *conn, json_t* jreq, json_t* jrsp);

int handle_hotkey(json_t* jreq, json_t* jrsp);

int handle_plugin(struct mg_connection *conn, json_t* jreq, json_t* jrsp);

#endif // RCONHANDLERS_H
