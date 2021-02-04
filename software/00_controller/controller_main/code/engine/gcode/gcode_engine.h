#ifndef GCODE_ENGINE_H
#define GCODE_ENGINE_H
 

#include "engine.h"
#include "middleware.h"


void gcode_engine_init(void);
void gcode_engine_once(void);


void gcode_engine_command(char * cmd_line, const burst_rcv_ctx_t * rcv_ctx);




#endif // GCODE_ENGINE_H
