
#include <jansson.h>

#include "rcon-main.h"
#include "rcon-handlers.h"


//include last to force-redifine of version strings
#undef OBS_VERSION
#undef OBS_PLUGIN_DESTINATION
#include <obsconfig.h>


int handle_version(struct mg_connection *conn, json_t* jreq, json_t* jrsp){

	//mongoose connection data:
	{
		json_t* mgj = json_object();
		json_object_set_new(mgj,"remote_port",json_integer(conn->remote_port));
		json_object_set_new(mgj,"local_port",json_integer(conn->local_port));
		json_object_set_new(mgj,"remote_ip",json_string(conn->remote_ip));
		json_object_set_new(mgj,"local_ip",json_string(conn->local_ip));
		json_object_set_new(mgj,"uri",json_string(conn->uri));
		json_object_set_new(mgj,"method",json_string(conn->request_method));

		json_object_set_new(jrsp,"http_info",mgj);
	}

	//OBS data, TODO: find out linked version? (see OBSApp()->GetVersionString()?)
	{
		json_t* obsj = json_object();
		json_object_set_new(obsj,"compiled_against",json_string(OBS_VERSION));

		json_object_set_new(jrsp,"obs_info",obsj);
	}

	//obs-rcon info
	{
		json_t* rinj = json_object();
		json_object_set_new(rinj,"version",json_string("0.0.1"));

		json_t* ractions = json_array();
		json_array_append_new(ractions,json_string("version"));

		//these are actually "split" and handled by sub-handlers.
		//user should query sub-hanlders (or check version number) for compat
		json_array_append_new(ractions,json_string("api/output"));
		//json_array_append_new(ractions,json_string("api/hotkey"));
		json_array_append_new(ractions,json_string("plugin"));

		json_object_set_new(rinj,"actions_supported",ractions);

		//now for registered plugins plz
		json_t* rplugins = json_array();
		for (size_t i=0; i < rcon_data.plugin_handlers.num; i++){
			struct rcon_handler* hndlr = darray_item(sizeof(struct rcon_handler),&rcon_data.plugin_handlers,i);
			json_array_append_new(rplugins,json_string(hndlr->action));
		}
		json_object_set_new(rinj,"plugin_actions_supported",rplugins);

		json_object_set_new(jrsp,"rcon",rinj);
	}

	if (jreq){
		json_object_set(jrsp, "requestJSON", jreq);
	}
	else{
		json_object_set_new(jrsp,"requestJSON", json_null());
	}

	return 200;
}

int handle_hotkey(json_t* jreq, json_t* jrsp){
	UNUSED_PARAMETER(jrsp);

	json_t* jkey = json_object_get(jreq,"key");

	if (!jkey || !json_is_string(jkey)){
		json_object_set_new(jrsp,"error",json_string("no key/modifiers or invalid type of key/modifiers"));
		return 400;
	}

	obs_key_combination_t combo;
	combo.key = obs_key_from_name(json_string_value(jkey));
	combo.modifiers = 0;

	//handle modifiers
	if (json_is_true(json_object_get(jreq,"shift"))){
		combo.modifiers |= INTERACT_SHIFT_KEY;
	}
	if (json_is_true(json_object_get(jreq,"control"))){
		combo.modifiers |= INTERACT_CONTROL_KEY;
	}
	if (json_is_true(json_object_get(jreq,"alt"))){
		combo.modifiers |= INTERACT_ALT_KEY;
	}
	if (json_is_true(json_object_get(jreq,"command"))){
		combo.modifiers |= INTERACT_COMMAND_KEY;
	}
	blog(LOG_INFO, "handle_hotkey: key:'%s' keyid:'0x%x' mods:'0x%x'",
		 json_string_value(jkey), combo.key, combo.modifiers);
	obs_hotkey_inject_event(combo,true);
	return 200;
}

int handle_output(json_t* jreq, json_t* jrsp){
	UNUSED_PARAMETER(jreq);
	UNUSED_PARAMETER(jrsp);
	blog(LOG_WARNING, "action 'api/output' TODO");
	return 500;
}

