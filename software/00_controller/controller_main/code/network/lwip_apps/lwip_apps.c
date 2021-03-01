
#include "lwip.h"
#include "lwip/apps/httpd.h"
#include "lwip/apps/netbiosns.h"
#include "tcpconn/tcpconn.h"
#include "lwip_apps.h"


void lwip_apps_init()
{
   netbiosns_init();
   netbiosns_set_name("uni_controller");

   httpd_init();

   tcpconn_init();
}
