
#include <jansson.h>

#include "rcon-main.h"
#include "rcon-handlers.h"



int root_handler(struct mg_connection *conn, json_t* jreq, json_t* jrsp){
	info("root URI handler called");
	json_object_set_new(jrsp,"hello",json_string("world"));
	json_object_set_new(jrsp,"ConPort",json_integer(conn->remote_port));

	if (jreq){
		json_object_set(jrsp, "requestJSON", jreq);
	}
	else{
		json_object_set_new(jrsp,"requestJSON", json_null());
	}

	return 200;
}
