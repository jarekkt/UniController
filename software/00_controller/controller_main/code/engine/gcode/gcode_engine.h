#ifndef GCODE_ENGINE_MOTION_H
#define GCODE_ENGINE_MOTION_H
 

#include "engine.h"
#include "middleware.h"





void gcode_engine_init(void);
void gcode_engine_once(void);


void  gcode_engine_command(char * cmd_line, const burst_rcv_ctx_t * rcv_ctx);
float gcode_engine_units(float base_unit);



#endif // GCODE_ENGINE_MOTION_H
