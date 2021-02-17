#ifndef GCODE_PARSER_H
#define GCODE_PARSER_H
 

typedef enum
{
   GCODE_F_G0,
   GCODE_F_G1,
   GCODE_F_G20,
   GCODE_F_G21,
   GCODE_F_G28,
   GCODE_F_M101,
   GCODE_F_CNT
}gcode_fn_e;



typedef enum
{
	GCODE_I_X = 0,
	GCODE_I_Y = 1,
	GCODE_I_Z = 2,
	GCODE_I_F,
	GCODE_I_G,
	GCODE_I_H,
	GCODE_I_CNT
}gcode_item_e;

typedef enum
{
	GCODE_V_NONE,
	GCODE_V_UINT,
	GCODE_V_INT,
	GCODE_V_FLOAT
}gcode_value_type_e;

typedef union
{
	uint32_t val_ui32;
	int32_t  val_i32;
	float    val_float;
}gcode_value_u;


typedef struct
{
	gcode_fn_e  fn;

	struct
	{
		gcode_value_u		value;
		gcode_value_type_e  value_type;
	}tokens[GCODE_I_CNT];

	uint32_t tokens_present_mask;

}gcode_command_t;



void 	 	 gcode_parser_init(void);
int32_t  	 gcode_parser_execute(gcode_command_t * cmd,char * cmd_line);
const char * gcode_parser_error(void);









#endif // GCODE_ENGINE_H
