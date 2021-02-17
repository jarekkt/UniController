#include "services.h"
#include "version.h"


void srv_init(void)
{
  srv_wdg_init();
  srv_gpio_init();
  srv_serial_init();
  srv_timer_init();
  srv_serial_485_init();
  srv_sermon_init();
  srv_serial_init();
  srv_i2c_init();
  srv_adc_init();
}
 
void srv_send_id(void)
{
  srv_serial_puts(SRV_SERIAL_DEBUG,version_string,strlen(version_string));
  srv_serial_puts(SRV_SERIAL_DEBUG,version_string_git,strlen(version_string_git));
  srv_serial_puts(SRV_SERIAL_DEBUG,"\r\r",2);

}



void srv_once(void)
{
  srv_wdg_once();
  srv_gpio_once();
  srv_serial_once();
  srv_send_id();
  srv_timer_once();
  srv_serial_485_once();
  srv_sermon_once();
  srv_serial_once();
  srv_i2c_once();
  srv_adc_once();

}
