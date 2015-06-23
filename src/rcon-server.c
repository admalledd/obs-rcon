
#include <strings.h>

#include "mongoose.h"
#include "rcon-main.h"
#include "rcon-server.h"

#include "rcon-handlers.h"


struct rcon_handler _handlers[MAX_RCON_HANDLERS] = {{0}};


int ev_handler(struct mg_connection *conn, enum mg_event ev) {
	switch (ev) {
		case MG_AUTH: return MG_TRUE;
		case MG_REQUEST:
		{
			//mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);
			info("Requested URI:REQ is [%s]:[%s]", conn->uri, conn->request_method);

			mg_send_header(conn, "Content-Type", "application/json");

			json_t* jobj_req = NULL;
			json_error_t jerror;
			if (cmpuri(conn->request_method,"POST")){
				jobj_req = json_loadb(conn->content, conn->content_len, 0, &jerror);
				if (!jobj_req){
					mg_printf_data(conn,"json error on line %d: %s\n", jerror.line, jerror.text);
					mg_send_status(conn, 400);
					info("error parsing json POST, json error on line %d: %s\n", jerror.line, jerror.text);
					return MG_TRUE;
				}
			}

			json_t* jobj_resp = json_object();
			int status = 404; //allow handler to choose response status
			for (int i=0; i < MAX_RCON_HANDLERS; i++){
				if (_handlers[i].uri_path){
					if (cmpuri(_handlers[i].uri_path,conn->uri)){
						status = _handlers[i].handler(conn, jobj_req, jobj_resp);
						break; //break on first handler found
					}
				}
			}

			//free request
			if (jobj_req){
				json_decref(jobj_req);
				jobj_req = NULL;
			}

			//dump response
			char * jstr = json_dumps(jobj_resp, 0);
			json_decref(jobj_resp);
			jobj_resp = NULL;

			if (status == 404){
				char* errmsg = "{\"error\":\"No handler for URI found.\"}";
				mg_send_data(conn, errmsg, strlen(errmsg));
				mg_send_status(conn, 404);
			}
			else {
				mg_send_data(conn, jstr, strlen(jstr));
				mg_send_status(conn, status);
			}
			free(jstr);

			return MG_TRUE;
		}
		default: return MG_FALSE;
	}
}


void rcon_add_handler(char * uri, handler_func handler){
	for (int i=0; i < MAX_RCON_HANDLERS; i++){
		if (_handlers[i].uri_path){
			//check for dup'd URI, ignore on that case.
			if (cmpuri(uri,_handlers[i].uri_path)){
				warning("duplicate handler for URI [%s] was ignored.", uri);
				return;
			}
		}
		if (!_handlers[i].uri_path){
			info("adding handler for URI [%s]",uri);
			_handlers[i].uri_path = uri;
			_handlers[i].handler = handler;
			return;
		}
	}
	//if we got here, maxed out handlers?
	error("could not add new handler, are they all taken?");
}

bool cmpuri(const char* uri1, const char* uri2){
	if (strcasecmp(uri1,uri2)==0){
		return true;
	}
	else {
		return false;
	}
}



void server_start()
{
	//first add handler(s)

	rcon_add_handler("/",root_handler);

	rcon_data.server = mg_create_server(NULL, ev_handler);
	mg_set_option(rcon_data.server, "listening_port", "8080");

	rcon_data.run_thread = true;

	pthread_create(&rcon_data.server_thread, NULL, thread_runner, NULL);
}

void server_stop()
{
	rcon_data.run_thread = false;
	pthread_join(rcon_data.server_thread,NULL);
	mg_destroy_server(&rcon_data.server);
}


void *thread_runner(void* init_data)
{
	UNUSED_PARAMETER(init_data);

	info("inside thread!");
	while (rcon_data.run_thread)
	{
		//poll for 1 second, allows server closing to happen once per second therefore
		mg_poll_server(rcon_data.server, 1000);
	}
	info("closing thread_runner");
	return NULL;
}

