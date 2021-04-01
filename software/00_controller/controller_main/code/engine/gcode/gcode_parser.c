/*!
    \file gcode_parser.c

    \brief
*/

#include "system.h"
#include "services.h"
#include "middleware.h"
#include "engine.h"
#include "gcode_parser.h"



typedef struct
{
	char 				token_letter;
	gcode_item_e		token_id;
	gcode_value_type_e	token_val_type;
}gcode_table_token_t;

typedef struct
{
	char 		fn_letter;
	uint32_t 	fn_number;
	uint32_t 	fn_sub_number;
	gcode_fn_e  fn_id;

	gcode_table_token_t tokens[GCODE_I_CNT];

}gcode_parse_table_t;

typedef struct
{
	char error_message[64];
	int	 parse_fn_idx;
}gcode_pasres_ctx_t;

gcode_parse_table_t  parse_table[] =
{
		{
			'G',0,0, GCODE_F_G0,
			{
				{ 'X' , GCODE_I_X, GCODE_V_FLOAT} ,
				{ 'Y' , GCODE_I_Y, GCODE_V_FLOAT} ,
				{ 'Z' , GCODE_I_Z, GCODE_V_FLOAT} ,
				{ 'A' , GCODE_I_A, GCODE_V_FLOAT} ,
				{ 'U' , GCODE_I_U, GCODE_V_FLOAT} ,
				{ 'V' , GCODE_I_V, GCODE_V_FLOAT} ,
				{ 'W' , GCODE_I_W, GCODE_V_FLOAT} ,
				{ 'B' , GCODE_I_B, GCODE_V_FLOAT} ,
				{ 'F' , GCODE_I_F, GCODE_V_FLOAT} ,
				{ 'G' , GCODE_I_F, GCODE_V_FLOAT} ,
				{ 'H' , GCODE_I_F, GCODE_V_FLOAT} ,
			}
		},
		{
			'G',1,0, GCODE_F_G1,
			{
				{ 'X' , GCODE_I_X, GCODE_V_FLOAT} ,
				{ 'Y' , GCODE_I_Y, GCODE_V_FLOAT} ,
				{ 'Z' , GCODE_I_Z, GCODE_V_FLOAT} ,
				{ 'A' , GCODE_I_A, GCODE_V_FLOAT} ,
				{ 'U' , GCODE_I_U, GCODE_V_FLOAT} ,
				{ 'V' , GCODE_I_V, GCODE_V_FLOAT} ,
				{ 'W' , GCODE_I_W, GCODE_V_FLOAT} ,
				{ 'B' , GCODE_I_B, GCODE_V_FLOAT} ,
				{ 'F' , GCODE_I_F, GCODE_V_FLOAT} ,
				{ 'G' , GCODE_I_F, GCODE_V_FLOAT} ,
				{ 'H' , GCODE_I_F, GCODE_V_FLOAT} ,

			}
		},
		{
			'G',4, 0,GCODE_F_G4,
		 	 {
				{ 'S' , GCODE_I_S, GCODE_V_FLOAT} ,
		 	 }
		},
		{
			'G',20, 0,GCODE_F_G20,
		},
		{
			'G',21, 0,GCODE_F_G21,
		},
		{
			'G',28, 0,GCODE_F_G28,
			{
				{ 'X' , GCODE_I_X, GCODE_V_FLOAT} ,
				{ 'Y' , GCODE_I_Y, GCODE_V_FLOAT} ,
				{ 'Z' , GCODE_I_Z, GCODE_V_FLOAT} ,
				{ 'A' , GCODE_I_A, GCODE_V_FLOAT} ,
				{ 'U' , GCODE_I_U, GCODE_V_FLOAT} ,
				{ 'V' , GCODE_I_V, GCODE_V_FLOAT} ,
				{ 'W' , GCODE_I_W, GCODE_V_FLOAT} ,
				{ 'B' , GCODE_I_B, GCODE_V_FLOAT} ,
				{ 'F' , GCODE_I_F, GCODE_V_FLOAT} ,
				{ 'G' , GCODE_I_F, GCODE_V_FLOAT} ,
				{ 'H' , GCODE_I_F, GCODE_V_FLOAT} ,
			}
		},
		{
			'G',92, 0,GCODE_F_G92,
			{
				{ 'X' , GCODE_I_X, GCODE_V_FLOAT} ,
				{ 'Y' , GCODE_I_Y, GCODE_V_FLOAT} ,
				{ 'Z' , GCODE_I_Z, GCODE_V_FLOAT} ,
				{ 'A' , GCODE_I_A, GCODE_V_FLOAT} ,
				{ 'U' , GCODE_I_U, GCODE_V_FLOAT} ,
				{ 'V' , GCODE_I_V, GCODE_V_FLOAT} ,
				{ 'W' , GCODE_I_W, GCODE_V_FLOAT} ,
				{ 'B' , GCODE_I_B, GCODE_V_FLOAT} ,
				{ 'F' , GCODE_I_F, GCODE_V_FLOAT} ,
				{ 'G' , GCODE_I_F, GCODE_V_FLOAT} ,
				{ 'H' , GCODE_I_F, GCODE_V_FLOAT} ,
			}
		},
		{
			'M',42, 0,GCODE_F_M42,
			 {
				{ 'S' , GCODE_I_S, GCODE_V_UINT} ,
				{ 'P' , GCODE_I_P, GCODE_V_UINT} ,
			 }
		},
		{
			'M',105, 0,GCODE_F_M105,
			 {
				{ 'T' , GCODE_I_T, GCODE_V_UINT} ,
			 }
		},
		{
			'M',114, 0,GCODE_F_M114,
		},
		{
			'M',115, 0,GCODE_F_M115,
		},
		{
			'M',204, 0,GCODE_F_M204,
		 	 {
				{ 'S' , GCODE_I_S, GCODE_V_FLOAT} ,
		 	 }
		}
		,
		{
			'M',204, 3,GCODE_F_M204_3,
		 	 {
				{ 'S' , GCODE_I_S, GCODE_V_FLOAT} ,
		 	 }
		},
		{
			'M',400, 0,GCODE_F_M400,
		},
};

gcode_pasres_ctx_t gctx;

void gcode_parser_init()
{
	memset(&gctx,0,sizeof(gctx));
}

const char * gcode_parser_error(void)
{
	return gctx.error_message;
}

int32_t  gcode_parse_fn(gcode_command_t * cmd,const char * chunk)
{
	int    ii;
	long   num;
	long   subnum = 0;

	char * endptr = NULL;

	num  = strtol(&chunk[1], &endptr, 10);

	if(*endptr == '.')
	{
		subnum  = strtol(endptr+1, &endptr, 10);
	}

	if (*endptr != '\0')
	{
	  sprintf(gctx.error_message,"Wrong/missing number/sub number in %s",chunk);
	  return -1;
	}

	for(ii =0; ii < DIM(parse_table);ii++)
	{
		if(chunk[0] == parse_table[ii].fn_letter)
		{
			if(chunk[1]!= '\x0')
			{
				if( (num == parse_table[ii].fn_number) && (subnum == parse_table[ii].fn_sub_number) )
				{
					cmd->fn = parse_table[ii].fn_id;
					gctx.parse_fn_idx = ii;
					return 0;
				}
			}
		}
	}

	sprintf(gctx.error_message,"Unsupported function %s",chunk);

	return -1;
}

int32_t  gcode_parse_token(gcode_command_t * cmd,const char * chunk,int token_idx)
{
	int 		ii;
	uint32_t	mask = 0;
	long   		num;
	float       numf;
	char * 		endptr;
	const gcode_table_token_t * t = parse_table[gctx.parse_fn_idx].tokens;


	for(ii = 0;ii < GCODE_I_CNT;ii++)
	{
		if(chunk[0] == t->token_letter)
		{
			if( (mask & (1<<ii) )!= 0)
			{
				sprintf(gctx.error_message,"Duplicate token %s",chunk);
				// duplicate
				return -1;
			}
			mask |=  (1<<ii);

			cmd->tokens_present_mask |= (1<<ii);
			cmd->tokens[token_idx].value_type = t->token_val_type;

			switch(t->token_val_type)
			{
				case GCODE_V_NONE:
				{
					cmd->tokens[token_idx].value.val_ui32 = 0;
				}break;

				case GCODE_V_UINT:
				{
					num  = strtol(&chunk[1], &endptr, 10);
					if ( (*endptr != '\0') || (num < 0) )
					{
					  sprintf(gctx.error_message,"Wrong uint value %s",chunk);
					  return -1;
					}
					cmd->tokens[token_idx].value.val_ui32 = (uint32_t)num;
				}break;

				case GCODE_V_INT:
				{
					num  = strtol(&chunk[1], &endptr, 10);
					if (*endptr != '\0')
					{
					  sprintf(gctx.error_message,"Wrong int value %s",chunk);
					  return -1;
					}
					cmd->tokens[token_idx].value.val_i32 = (int32_t)num;
				}break;

				case GCODE_V_FLOAT:
				{
					numf = strtof(&chunk[1], &endptr);
					if (*endptr != '\0')
					{
					  sprintf(gctx.error_message,"Wrong float value %s",chunk);
					  return -1;
					}
					cmd->tokens[token_idx].value.val_float = numf;
				}break;

				default:
				{
					sprintf(gctx.error_message,"Internal error %s",chunk) ;
					return -1;
				}break;
			}

			break;
		}
	}

	return 0;
}

int32_t gcode_parser_replacechar(char *str, char orig, char rep)
{
    char *ix = str;
    int32_t n = 0;
    while((ix = strchr(ix, orig)) != NULL)
    {
        *ix++ = rep;
        n++;
    }
    return n;
}

int32_t  gcode_parser_execute(gcode_command_t * cmd,char * cmd_line)
{
	int32_t result = -1;
	int32_t idx;
    char *string,*found;


    // Initial settings
    string = cmd_line;
    idx    = 0;
    memset(cmd,0,sizeof(*cmd));

    // Cut off comments
    found = strchr(cmd_line,';');
    if(found)
    {
    	*found = '\x0';
    }

    // Replace unwanted characters
    gcode_parser_replacechar(cmd_line,'\r',' ');
    gcode_parser_replacechar(cmd_line,'\n',' ');

    memset(cmd,0,sizeof(*cmd));

    while(1)
    {
    	found = strsep(&string," ");
    	if(found != NULL)
    	{
    		if(idx == 0)
    		{
				if(gcode_parse_fn(cmd,found)!= 0)
				{
					break;
				}
				// Good ,we have header parsed
				result = 0;
    		}
    		else
    		{
    			if(gcode_parse_token(cmd,found,idx-1)!= 0)
    			{
    				// Bad , wrong token
    				result = -1;
    				break;
    			}
    		}
    	}
		else
		{
			break;
		}

    	idx++;
    }


	return result;
}
