
#include "rcon-main.h"

rcon_data_t rcon_data = {0};


OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-rcon", "en-US")


bool obs_module_load(void)
{
	info("Loading RCON plugin...");
	server_start();
	return true;
}

void obs_module_unload(void)
{
	info("Unloading RCON plugin...");
	server_stop();
}
