#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "portmacro.h"


#include "system/debug/debug.h"



#ifndef DIM
#define DIM(x_)                         (sizeof(x_)/sizeof(x_[0]))
#endif

#define SYS_VAR_NAME_LENGTH   32


#define ATTRIBUTE_IN_RAM __attribute__  ((section (".fast")))


#endif
