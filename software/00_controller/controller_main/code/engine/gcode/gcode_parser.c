/*!
    \file gcode_parser.c

    \brief
*/

#include "system.h"
#include "services.h"
#include "middleware.h"
#include "engine.h"
#include "gcode_parser.h"




#define   OBL_OPTIONAL  0
#define   OBL_ONEMORE   1
#define   OBL_ALWAYS    2
#define   OBL_ONLYONE   3

typedef struct
{
	char 				token_letter;
	gcode_item_e		token_id;
	gcode_value_type_e	token_val_type;
	int					obligatory;
}gcode_table_token_t;

typedef struct
{
	char 		fn_letter;
	uint32_t 	fn_number;
	uint32_t 	fn_sub_number;
	gcode_fn_e  fn_id;

	gcode_table_token_t tokens[GCODE_I_CNT+1];

}gcode_parse_table_t;

typedef struct
{
	char error_message[64];
	int	 parse_fn_idx;
}gcode_pasres_ctx_t;

const gcode_parse_table_t  parse_table[] =
{
		{
			'G',0,0, GCODE_F_G0,
			{
				{ 'X' , GCODE_I_X, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'Y' , GCODE_I_Y, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'Z' , GCODE_I_Z, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'A' , GCODE_I_A, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'B' , GCODE_I_B, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'C' , GCODE_I_C, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'D' , GCODE_I_D, GCODE_V_FLOAT, OBL_ONEMORE}
			}
		},
		{
			'G',1,0, GCODE_F_G1,
			{
				{ 'X' , GCODE_I_X, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'Y' , GCODE_I_Y, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'Z' , GCODE_I_Z, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'A' , GCODE_I_A, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'B' , GCODE_I_B, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'C' , GCODE_I_C, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'D' , GCODE_I_D, GCODE_V_FLOAT, OBL_ONEMORE}

			}
		},
		{
			'G',4, 0,GCODE_F_G4,
		 	 {
				{ 'S' , GCODE_I_S, GCODE_V_FLOAT,OBL_ALWAYS} ,
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
				{ 'X' , GCODE_I_X, GCODE_V_FLOAT, OBL_ONLYONE} ,
				{ 'Y' , GCODE_I_Y, GCODE_V_FLOAT, OBL_ONLYONE} ,
				{ 'Z' , GCODE_I_Z, GCODE_V_FLOAT, OBL_ONLYONE} ,
				{ 'A' , GCODE_I_A, GCODE_V_FLOAT, OBL_ONLYONE} ,
				{ 'B' , GCODE_I_B, GCODE_V_FLOAT, OBL_ONLYONE} ,
				{ 'C' , GCODE_I_C, GCODE_V_FLOAT, OBL_ONLYONE} ,
				{ 'D' , GCODE_I_D, GCODE_V_FLOAT, OBL_ONLYONE} ,
			}
		},
		{
			'G',90, 0,GCODE_F_G90,
			{
				{ 'X' , GCODE_I_X, GCODE_V_NONE, OBL_ONEMORE} ,
				{ 'Y' , GCODE_I_Y, GCODE_V_NONE, OBL_ONEMORE} ,
				{ 'Z' , GCODE_I_Z, GCODE_V_NONE, OBL_ONEMORE} ,
				{ 'A' , GCODE_I_A, GCODE_V_NONE, OBL_ONEMORE} ,
				{ 'B' , GCODE_I_B, GCODE_V_NONE, OBL_ONEMORE} ,
				{ 'C' , GCODE_I_C, GCODE_V_NONE, OBL_ONEMORE} ,
				{ 'D' , GCODE_I_D, GCODE_V_NONE, OBL_ONEMORE} ,
			}
		},
		{
			'G',91, 0,GCODE_F_G91,
			{
				{ 'X' , GCODE_I_X, GCODE_V_NONE, OBL_ONEMORE} ,
				{ 'Y' , GCODE_I_Y, GCODE_V_NONE, OBL_ONEMORE} ,
				{ 'Z' , GCODE_I_Z, GCODE_V_NONE, OBL_ONEMORE} ,
				{ 'A' , GCODE_I_A, GCODE_V_NONE, OBL_ONEMORE} ,
				{ 'B' , GCODE_I_B, GCODE_V_NONE, OBL_ONEMORE} ,
				{ 'C' , GCODE_I_C, GCODE_V_NONE, OBL_ONEMORE} ,
				{ 'D' , GCODE_I_D, GCODE_V_NONE, OBL_ONEMORE} ,
			}
		},
		{
			'G',92, 0,GCODE_F_G92,
			{
				{ 'X' , GCODE_I_X, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'Y' , GCODE_I_Y, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'Z' , GCODE_I_Z, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'A' , GCODE_I_A, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'B' , GCODE_I_B, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'C' , GCODE_I_C, GCODE_V_FLOAT, OBL_ONEMORE} ,
				{ 'D' , GCODE_I_D, GCODE_V_FLOAT, OBL_ONEMORE} ,
			}
		},
		{
			'M',42, 0,GCODE_F_M42,
			 {
				{ 'S' , GCODE_I_S, GCODE_V_UINT, OBL_ALWAYS} ,
				{ 'P' , GCODE_I_P, GCODE_V_UINT, OBL_ALWAYS} ,
			 }
		},
		{
			'M',105, 0,GCODE_F_M105,
			 {
				{ 'T' , GCODE_I_T, GCODE_V_UINT, OBL_OPTIONAL} ,
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
				{ 'S' , GCODE_I_S, GCODE_V_FLOAT, OBL_ALWAYS} ,
		 	 }
		}
		,
		{
			'M',201, 3,GCODE_F_M201_3,
		 	 {
				{ 'S' , GCODE_I_S, GCODE_V_FLOAT, OBL_ALWAYS} ,
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
	int32_t		result = -1;
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
				break;
			}

			cmd->tokens_present_mask |= (1<<t[ii].token_id);
			cmd->tokens[t[ii].token_id].value_type = t[ii].token_val_type;
			cmd->tokens[t[ii].token_id].letter = t[ii].token_letter;
			cmd->tokens[t[ii].token_id].obligatory  = t[ii].obligatory;

			switch(t[ii].token_val_type)
			{
				case GCODE_V_NONE:
				{
					if( chunk[1] != '\0')
					{
						sprintf(gctx.error_message,"Unexpected value %s",chunk);
					}
					else
					{
						cmd->tokens[t[ii].token_id].value.val_ui32 = 0;
						result = 0;
					}
				}break;

				case GCODE_V_UINT:
				{
					num  = strtol(&chunk[1], &endptr, 10);
					if ( (*endptr != '\0') || (num < 0) )
					{
					    sprintf(gctx.error_message,"Wrong uint value %s",chunk);
					}
					else
					{
						cmd->tokens[t[ii].token_id].value.val_ui32 = (uint32_t)num;
						result = 0;
					}
				}break;

				case GCODE_V_INT:
				{
					num  = strtol(&chunk[1], &endptr, 10);
					if (*endptr != '\0')
					{
					  sprintf(gctx.error_message,"Wrong int value %s",chunk);
					}
					else
					{
						cmd->tokens[ii].value.val_i32 = (int32_t)num;
					}
				}break;

				case GCODE_V_FLOAT:
				{
					numf = strtof(&chunk[1], &endptr);
					if (*endptr != '\0')
					{
					  sprintf(gctx.error_message,"Wrong float value %s",chunk);

					}
					else
					{
						cmd->tokens[t[ii].token_id].value.val_float = numf;
						result = 0;
					}
				}break;

				default:
				{
					sprintf(gctx.error_message,"Internal error %s",chunk) ;
				}break;
			}

			break;
		}
	}

	if(ii == GCODE_I_CNT)
	{
		sprintf(gctx.error_message,"Unrecognized token %s",chunk) ;
	}

	return result;
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

    int32_t  t_one_more  = 0, c_one_more = 0;
    int32_t  t_only_one  = 0, c_only_one = 0;
    int32_t  t_always    = 0, c_always   = 0;



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
					// Error, wrong header
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


    if(result == 0)
    {
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

			switch(cmd->tokens[idx].obligatory)
			{
				case OBL_ONEMORE:
				{
					t_one_more++;
				}break;

				case OBL_ALWAYS:
				{
					t_always++;
				}break;

				case OBL_ONLYONE:
				{
					t_only_one++;
				}break;

				case OBL_OPTIONAL:
				default:
				{
					// ignore these
				}break;
			}

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

				switch(cmd->tokens[idx].obligatory)
				{
					case OBL_ONEMORE:
					{
						c_one_more++;
					}break;

					case OBL_ALWAYS:
					{
						c_always++;
					}break;

					case OBL_ONLYONE:
					{
						c_only_one++;
					}break;

					case OBL_OPTIONAL:
					default:
					{
						// ignore these
					}break;
				}
			}
		}

		// Check syntax

		if(t_one_more>0)
		{
			if(c_one_more <1)
			{
				printds(LVL_INFO,"Syntax error - Token(OneOrMore)\r\n");
				result = -1;
			}
		}

		if(t_always!= c_always)
		{
			printds(LVL_INFO,"Syntax error - Token(Always)\r\n");
		}

		if(c_only_one>0)
		{
			if(c_only_one !=1)
			{
				printds(LVL_INFO,"Syntax error - Token(OnlyOne)\r\n");
				result = -1;
			}
		}
    }
    else
    {
		printd(
			LVL_INFO,
			"GCODE parser failure: %s\r\n",
			gctx.error_message
		);

    }







	return result;
}
