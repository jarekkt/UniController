#include "common.h"
#include "system.h"
#include "engine.h"
#include "middleware.h"
#include "services.h"
#include "lwip_apps.h"




void system_main_init()
{
  /* Start services, not yet dependent on storage */
  srv_init();
  mwr_init();
  engine_init();

  /* Initial storage initialization - must be last one*/
  srv_nov_init();
  srv_nov_start();

  lwip_apps_init();

  /* Now final phase of services initialization - storage is present now */
  srv_once(); 
  mwr_once();
  engine_once();

  /* Dump storage initialization messages */
  srv_nov_print_info(burst_log_printf);

  /* High level network services */
  lwip_apps_once();

}
