#ifndef ENGINE_H
#define ENGINE_H




#include "system.h"
#include "engine/params/params.h"
#include "engine/gcode/gcode_engine.h"
#include "engine/motion/motion_engine.h"


  
void engine_init(void);
void engine_once(void);

#endif
 
