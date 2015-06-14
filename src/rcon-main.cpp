
#include <obs-module.h>
#include <util/threading.h>

#include "mongoose.h"


#define blog(level, msg, ...) blog(level, "obs-rcon: " msg, ##__VA_ARGS__)

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-rcon", "en-US")

struct rcon_data_t
{
    mg_server *server;
    pthread_t server_thread;
};

struct rcon_data_t rcon_data = {};


static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
  switch (ev) {
    case MG_AUTH: return MG_TRUE;
    case MG_REQUEST:
      mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);
      blog(LOG_WARNING,"Hello! Requested URI is [%s]", conn->uri);
      return MG_TRUE;
    default: return MG_FALSE;
  }
}

void *thread_runner(void* init_data)
{
    UNUSED_PARAMETER(init_data);

    blog(LOG_WARNING,"inside thread!");
    while (1)
    {
        //poll for 1 second, allows server closing to happen 1/second therefore
        mg_poll_server(rcon_data.server, 1000);

        //NO PROCESSING IN/AFTER LOOP! pthread might kill/cancel this!
    }
    return NULL;
}


bool obs_module_load(void)
{
    blog(LOG_WARNING, "Loading RCON plugin...");

    rcon_data.server = mg_create_server(NULL, ev_handler);
    mg_set_option(rcon_data.server, "listening_port", "8080");

    pthread_create(&rcon_data.server_thread, NULL, thread_runner, NULL);

    return true;
}

void obs_module_unload(void)
{
    blog(LOG_WARNING, "Unloading RCON plugin...");
    pthread_cancel(rcon_data.server_thread);
    pthread_join(rcon_data.server_thread,NULL);
    blog(LOG_WARNING, "RCON threads exited/joined...");
    mg_destroy_server(&rcon_data.server);
}
