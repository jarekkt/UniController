#ifndef GCODE_ENGINE_H
#define GCODE_ENGINE_H
 

#include "engine.h"
#include "middleware.h"


int32_t   gcode_engine_motion_G0G1(const burst_rcv_ctx_t * rcv_ctx,const gcode_command_t *	cmd);
int32_t   gcode_engine_motion_G92(const burst_rcv_ctx_t * rcv_ctx,const gcode_command_t *	cmd);



#endif // GCODE_ENGINE_H
