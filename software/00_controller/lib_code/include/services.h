#ifndef SERVICES_H
#define SERVICES_H

#include "services/i2c/i2c_srv.h"
#include "services/watchdog/watchdog_srv.h"
#include "services/serial_pt/serial_srv.h"
#include "services/serial_485/serial_485_srv.h"
#include "services/gpio/gpio_srv.h"
#include "services/timer/timer_srv.h"
#include "services/adc/adc_srv.h"
#include "services/nov/nov_srv.h"
#include "services/sermon/sermon_srv.h"
#include "services/crc32/crc32.h"



void srv_init(void);
void srv_once(void);


#endif
 
