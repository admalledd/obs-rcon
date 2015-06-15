
#include <util/threading.h>

#include "rcon-main.h"
#include "rcon-server.h"
#include "mongoose.h"

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



static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
  switch (ev) {
    case MG_AUTH: return MG_TRUE;
    case MG_REQUEST:
      mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);
      info("Hello! Requested URI is [%s]", conn->uri);
      return MG_TRUE;
    default: return MG_FALSE;
  }
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
