#include "system.h" 
#include "common.h" 
#include "engine.h" 


void engine_init(void)
{
	params_init();
	gcode_engine_init();
	motion_engine_init();
}

void engine_once(void)
{ 
	params_once();
	gcode_engine_once();
	motion_engine_once();
}
