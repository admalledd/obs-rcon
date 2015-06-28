
#include <strings.h>

#include "mongoose.h"
#include "rcon-main.h"
#include "rcon-server.h"
#include "rcon-handlers.h"

//local helper defines
bool cmpuri(const char* uri1, const char* uri2);

bool cmpjstr(json_t* jstr, const char* cstr);

int root_handler(struct mg_connection *conn, json_t* jreq, json_t* jrsp);

//Hanlde "all" requests to/from server, pass off to child funcs in
// rcon-handlers.c for specific API hooking
int root_handler(struct mg_connection *conn, json_t* jreq, json_t* jrsp){
	//Get current "action" request
	json_t* action = json_object_get(jreq,"action");
	if (!action){
		//no action found, set error handling
		json_object_set_new(jrsp,"error",json_string("no 'action' key in request (please POST)"));
		return 400;
	}

	//test handler, echos back some basic data about the request
	if(cmpjstr(action,"version")){
		return handle_version(conn,jreq,jrsp);
	}
	else if(cmpjstr(action,"api/hotkey")){
		return handle_hotkey(jreq,jrsp);
	}
	else if(cmpjstr(action,"api/output")){
		return handle_output(jreq,jrsp);
	}
	else if(cmpjstr(action,"plugin")){
		return handle_plugin(conn,jreq,jrsp);
	}
	else {
		json_object_set_new(jrsp,"error",json_string("no matching 'action' found for request."));

		if (jreq){
			json_object_set(jrsp, "requestJSON", jreq);
		}
		else{
			json_object_set_new(jrsp,"requestJSON", json_null());
		}
		return 400;
	}
}


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

			status = root_handler(conn, jobj_req, jobj_resp);

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

//TODO: make a handler func array such that if a 404 is
// returned pass on and try the next handler.
/*
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
*/

//Simplify strcmp usage, normally for URIs
bool cmpuri(const char* uri1, const char* uri2){
	if (strcasecmp(uri1,uri2)==0){
		return true;
	}
	else {
		return false;
	}
}

// Compare a json_t* string to cstr
bool cmpjstr(json_t* jstr, const char* cstr){
	//Start out with some sanity checks:
	if (!json_is_string(jstr)){
		return false;
	}
	if (!cstr){
		return false;
	}
	//create a tmp json_string for cstr, compare, then decref
	json_t* jcstr = json_string(cstr);

	int ret = json_equal(jstr,jcstr);
	json_decref(jcstr);

	//do a bool like thingy
	if (ret){
		return true;
	}
	else{
		return false;
	}
}


void server_start()
{
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

