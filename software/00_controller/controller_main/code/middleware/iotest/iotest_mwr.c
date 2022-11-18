 /*!
    \file iotest_mwr.c
*/


#include "services.h"
#include "middleware.h"
#include "main.h"

#define MWR_IOTEST_THREAD_PERIOD 50


typedef struct
{
	uint32_t ovr_on;
	uint32_t ovr_off;
	uint32_t ovr_stat;
	uint32_t ovr_blink;
	uint32_t ovr_blink_bit;
	uint32_t ovr_hw_step_blink;
	uint32_t ovr_blink_cntr;
	uint32_t ovr_blink_toggle;
}mwr_iotest_type;


typedef struct
{  
    GPIO_TypeDef * gpio_set;
    uint32_t       gpio_pin_set;
    uint32_t       mode;
}mwr_gpio_sequence_t;



#define GPIO_SEQ_NONE()           0,0
#define GPIO_SEQ_PIN_PORT(name_)  name_##_GPIO_Port
#define GPIO_SEQ_PIN_BIT(name_)   name_##_Pin
#define GPIO_SEQ_PIN(name_)       GPIO_SEQ_PIN_PORT(name_),GPIO_SEQ_PIN_BIT(name_)

const mwr_gpio_sequence_t mwr_test_pins[] = 
{

  /* 00 */ { GPIO_SEQ_PIN(OUT_CPU5)},
  /* 01 */ { GPIO_SEQ_PIN(OUT_CPU6)},
  /* 02 */ { GPIO_SEQ_PIN(OUT_CPU7)},
  /* 03 */ { GPIO_SEQ_PIN(OUT_CPU8)},

  /* 04 */ { GPIO_SEQ_PIN(OUT_CPU9)},
  /* 05 */ { GPIO_SEQ_PIN(OUT_DIR1)},
  /* 06 */ { GPIO_SEQ_PIN(OUT_DIR2)},
  /* 07 */ { GPIO_SEQ_PIN(OUT_DIR3)},

  /* 08 */ { GPIO_SEQ_PIN(OUT_STEP4)},
  /* 09 */ { GPIO_SEQ_PIN(OUT_DIR4)},
  /* 10 */ { GPIO_SEQ_PIN(OUT_STEP5)},
  /* 11 */ { GPIO_SEQ_PIN(OUT_DIR5)},

  /* 12 */ { GPIO_SEQ_PIN(OUT_STEP6)},
  /* 13 */ { GPIO_SEQ_PIN(OUT_DIR6)},
  /* 14 */ { GPIO_SEQ_PIN(OUT_STEP7)},
  /* 15 */ { GPIO_SEQ_PIN(OUT_DIR7)},

  /* 16 */ { GPIO_SEQ_PIN(OUT_RS485_RE)},
  /* 17 */ { GPIO_SEQ_PIN(LED) },
  /* 18 */ { GPIO_SEQ_PIN(IO_CPU1)},
  /* 19 */ { GPIO_SEQ_PIN(IO_CPU2)},

  /* 20 */ { GPIO_SEQ_PIN(IO_CPU3)},
  /* 21 */ { GPIO_SEQ_PIN(IO_CPU4)},
  /* 22 */ { GPIO_SEQ_PIN(IO_CPU5)},
  /* 23 */ { GPIO_SEQ_PIN(IO_CPU6)},

  /* 24 */ { GPIO_SEQ_PIN(IO_CPU7)},
  /* 25 */ { GPIO_SEQ_PIN(IO_CPU8)},
  /* 26 */ { GPIO_SEQ_PIN(IO_CPU9)},


};



mwr_iotest_type  iot;


const var_ptable_t   iotest_mwr_var_ptable[] SERMON_ATTR =
{
  { "iotest_ovr_on", 	  	 &iot.ovr_on            ,E_VA_UINT_FREE    },
  { "iotest_ovr_off",      	 &iot.ovr_off           ,E_VA_UINT_FREE    },
  { "iotest_ovr_blink",  	 &iot.ovr_blink         ,E_VA_UINT_FREE    },
  { "iotest_ovr_blink_bit",  &iot.ovr_blink_bit     ,E_VA_UINT_FREE    },
};



static void mwr_iotest_thread(void);


/*!
    \brief Backgorund iotest init function
    
*/

void mwr_iotest_init(void)
{
    memset(&iot,0,sizeof(iot));


    srv_sermon_register(iotest_mwr_var_ptable,sizeof(iotest_mwr_var_ptable));
    mwr_periodic_low_register(mwr_iotest_thread,MWR_IOTEST_THREAD_PERIOD);
  
    mwr_iotest_io_init();

}

void mwr_iotest_once(void)
{

}




static void mwr_iotest_set(uint32_t mask,uint32_t val)
{
  int ii;


  for (ii = 0; ii < DIM(mwr_test_pins);ii++)
  {
      if( (mask & ( 1<<ii)) != 0 )
      {
          if(val != 0)
          {   
            HAL_GPIO_WritePin(mwr_test_pins[ii].gpio_set,mwr_test_pins[ii].gpio_pin_set,GPIO_PIN_SET);
          }
          else
          {
            HAL_GPIO_WritePin(mwr_test_pins[ii].gpio_set,mwr_test_pins[ii].gpio_pin_set,GPIO_PIN_RESET);
          }
      }
  }
}

void mwr_iotest_test_task(void)
{

   if(iot.ovr_on != 0 )
   {
      GPIO_TestMode(1);  
      mwr_iotest_set( 0xFFFFFFFF,GPIO_PIN_RESET);

      // Enable
      iot.ovr_stat |= iot.ovr_on;
      mwr_iotest_set( iot.ovr_on,GPIO_PIN_SET); 
      iot.ovr_on  = 0;
   }

   if(iot.ovr_off != 0 )
   {
      // Disable
      iot.ovr_stat &= ~ iot.ovr_off;
      mwr_iotest_set( iot.ovr_off,GPIO_PIN_RESET); 
      iot.ovr_off  = 0;
   }

   if(iot.ovr_blink_bit != 0 )
   {
	   iot.ovr_blink 		= ( 1 << (iot.ovr_blink_bit-1));
	   iot.ovr_blink_bit 	= 0;
   }

   if(iot.ovr_blink != 0 )
   {
	   iot.ovr_blink_cntr++;

	   if( (iot.ovr_blink_cntr % (50/MWR_IOTEST_THREAD_PERIOD)) == 0)
	   {
		   iot.ovr_blink_toggle = !iot.ovr_blink_toggle;

		   if(iot.ovr_blink_toggle != 0)
		   {
			   mwr_iotest_set(iot.ovr_blink,iot.ovr_blink);

			   if(iot.ovr_hw_step_blink & 0x01)
			   {
				   TMR_TIRGGER_X();
			   }

			   if(iot.ovr_hw_step_blink & 0x02)
			   {
				   TMR_TIRGGER_Y();
			   }

			   if(iot.ovr_hw_step_blink & 0x04)
			   {
				   TMR_TIRGGER_Z();
			   }
		   }
		   else
		   {
			   mwr_iotest_set(iot.ovr_blink,0);
		   }
	   }
   }
}




/*!
    \brief Background led task thread function
    
*/
static void mwr_iotest_thread(void)
{

    mwr_iotest_io_task();
    mwr_iotest_test_task();
}




