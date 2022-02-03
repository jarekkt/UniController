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
				{ 'B' , GCODE_I_B, GCODE_V_FLOAT} ,
				{ 'C' , GCODE_I_C, GCODE_V_FLOAT} ,
				{ 'D' , GCODE_I_D, GCODE_V_FLOAT} ,
				{ 'E' , GCODE_I_E, GCODE_V_FLOAT} ,
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
				{ 'B' , GCODE_I_B, GCODE_V_FLOAT} ,
				{ 'C' , GCODE_I_C, GCODE_V_FLOAT} ,
				{ 'D' , GCODE_I_D, GCODE_V_FLOAT} ,
				{ 'E' , GCODE_I_E, GCODE_V_FLOAT} ,
				{ 'F' , GCODE_I_F, GCODE_V_FLOAT} ,
				{ 'G' , GCODE_I_G, GCODE_V_FLOAT} ,
				{ 'H' , GCODE_I_H, GCODE_V_FLOAT} ,

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
				{ 'B' , GCODE_I_B, GCODE_V_FLOAT} ,
				{ 'C' , GCODE_I_C, GCODE_V_FLOAT} ,
				{ 'D' , GCODE_I_D, GCODE_V_FLOAT} ,
				{ 'E' , GCODE_I_E, GCODE_V_FLOAT} ,
				{ 'F' , GCODE_I_F, GCODE_V_FLOAT} ,
				{ 'G' , GCODE_I_G, GCODE_V_FLOAT} ,
				{ 'H' , GCODE_I_H, GCODE_V_FLOAT} ,
			}
		},
		{
			'G',92, 0,GCODE_F_G92,
			{
				{ 'X' , GCODE_I_X, GCODE_V_FLOAT} ,
				{ 'Y' , GCODE_I_Y, GCODE_V_FLOAT} ,
				{ 'Z' , GCODE_I_Z, GCODE_V_FLOAT} ,
				{ 'A' , GCODE_I_A, GCODE_V_FLOAT} ,
				{ 'B' , GCODE_I_B, GCODE_V_FLOAT} ,
				{ 'C' , GCODE_I_C, GCODE_V_FLOAT} ,
				{ 'D' , GCODE_I_D, GCODE_V_FLOAT} ,
				{ 'E' , GCODE_I_E, GCODE_V_FLOAT} ,
				{ 'F' , GCODE_I_F, GCODE_V_FLOAT} ,
				{ 'G' , GCODE_I_G, GCODE_V_FLOAT} ,
				{ 'H' , GCODE_I_H, GCODE_V_FLOAT} ,
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
			'M',201, 3,GCODE_F_M201_3,
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
					cmd->fn_letter = parse_table[ii].fn_letter;
					cmd->fn_number = parse_table[ii].fn_number;
					gctx.parse_fn_idx = ii;
					return 0;
				}
			}
		}
	}

	sprintf(gctx.error_message,"Unsupported function %s",chunk);

	return -1;
}

int32_t  gcode_parse_token(gcode_command_t * cmd,const char * chunk)
{
	int 		ii;
	long   		num;
	float       numf;
	char * 		endptr;
	const gcode_table_token_t * t = parse_table[gctx.parse_fn_idx].tokens;


	for(ii = 0;ii < GCODE_I_CNT;ii++)
	{
		if(t[ii].token_letter==0)
		{
			// No more tokens in the table
			break;
		}


		if(chunk[0] == t[ii].token_letter)
		{
			if( (cmd->tokens_present_mask  & (1 << t[ii].token_id) )!= 0)
			{
				sprintf(gctx.error_message,"Duplicate token %s",chunk);
				// duplicate
				return -1;
			}

			cmd->tokens_present_mask |= (1<<t[ii].token_id);
			cmd->tokens[t[ii].token_id].value_type = t[ii].token_val_type;
			cmd->tokens[t[ii].token_id].letter = t[ii].token_letter;

			switch(t[ii].token_val_type)
			{
				case GCODE_V_NONE:
				{
					cmd->tokens[t[ii].token_id].value.val_ui32 = 0;
				}break;

				case GCODE_V_UINT:
				{
					num  = strtol(&chunk[1], &endptr, 10);
					if ( (*endptr != '\0') || (num < 0) )
					{
					  sprintf(gctx.error_message,"Wrong uint value %s",chunk);
					  return -1;
					}
					cmd->tokens[t[ii].token_id].value.val_ui32 = (uint32_t)num;
				}break;

				case GCODE_V_INT:
				{
					num  = strtol(&chunk[1], &endptr, 10);
					if (*endptr != '\0')
					{
					  sprintf(gctx.error_message,"Wrong int value %s",chunk);
					  return -1;
					}
					cmd->tokens[ii].value.val_i32 = (int32_t)num;
				}break;

				case GCODE_V_FLOAT:
				{
					numf = strtof(&chunk[1], &endptr);
					if (*endptr != '\0')
					{
					  sprintf(gctx.error_message,"Wrong float value %s",chunk);
					  return -1;
					}
					cmd->tokens[t[ii].token_id].value.val_float = numf;
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

int32_t  gcode_parser_execute(gcode_command_t * cmd,char * cmd_line,uint32_t len)
{
	int32_t result = -1;
	int32_t idx;
    char *string,*found;


    printd(LVL_INFO,"GCODE parser - got this[%d]: \"%s\r\n",len,cmd_line);





    // Trim possible telnet commands
    // Look for G code commands ( which start - in our case - with 'G' or 'M' )
    while(
    	((*cmd_line) != 'G') &&
		((*cmd_line) != 'M') &&
		(len > 0)
	)
    {
    	cmd_line++;
    	len--;
    }


    // Cut off comments
    found = strchr(cmd_line,';');
    if(found)
    {
    	*found = '\x0';
    }


    // Replace unwanted characters
    gcode_parser_replacechar(cmd_line,'\r',' ');
    gcode_parser_replacechar(cmd_line,'\n',' ');

    printd(LVL_INFO,"GCODE parser - after pre-processing: \"%s\"\r\n",cmd_line);

    // Initial settings
    string = cmd_line;
    idx    = 0;
    memset(cmd,0,sizeof(*cmd));

    while(1)
    {
    	found = strsep(&string," ");
    	if(found != NULL)
    	{
    		if(*found =='\x00' )
    		{
    			// Multiple spaces
    			continue;
    		}

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
    			if(gcode_parse_token(cmd,found)!= 0)
    			{
    				// Bad , wrong token
    				result = -1;
    				break;
    			}
    		}
    		idx++;
    	}
		else
		{
			break;
		}

    }


    printd(
    	LVL_INFO,
		"GCODE parser - result: %d\r\n %c%d.%d with tokens 0x%04x\r\n",
    	result,
		cmd->fn_letter,
		cmd->fn_number,
		cmd->sub_fn,
		cmd->tokens_present_mask
    );

    for(idx = 0;idx < GCODE_I_CNT;idx++)
    {
    	if((cmd->tokens_present_mask & (1<<idx)) != 0)
    	{
    		switch(cmd->tokens[idx].value_type)
    		{
    			case GCODE_V_NONE: 	 printd(LVL_DEBUG,"  %c\r\n",   cmd->tokens[idx].letter);break;
				case GCODE_V_UINT:	 printd(LVL_DEBUG,"  %c=%d\r\n",cmd->tokens[idx].letter,cmd->tokens[idx].value.val_i32);break;
    			case GCODE_V_INT:	 printd(LVL_DEBUG,"  %c=%u\r\n",cmd->tokens[idx].letter,cmd->tokens[idx].value.val_ui32);break;
    			case GCODE_V_FLOAT:  printd(LVL_DEBUG,"  %c=%f\r\n",cmd->tokens[idx].letter,cmd->tokens[idx].value.val_float);break;
    			default:			 printd(LVL_DEBUG,"  %c=??\r\n",cmd->tokens[idx].letter);break;
    		}
    	}
    }

	return result;
}
