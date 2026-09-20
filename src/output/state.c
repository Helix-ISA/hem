#include "output/state.h"

#include "cli/cl_parser.h"
#include "types.h"
#include "vendor/cJSON.h"
#include <stdio.h>

b8 dump_state(hx_processor* processor, hx_cli *cli)
{
	cJSON *registers = NULL;
	cJSON *reg = NULL;
	
	cJSON *state = cJSON_CreateObject();
	if (state == NULL)
		goto end;
	
	registers = cJSON_CreateArray();
	if (registers == NULL)
		goto end;
	cJSON_AddItemToObject(state, "registers", registers);

	for (u8 i = 0; i < GPR_COUNT; i++) {
		reg = cJSON_CreateNumber(processor->gpr[i]);
		if (reg == NULL)
			goto end;
		cJSON_AddItemToArray(registers, reg);
	}

	char *output = cJSON_Print(state);


	fputs(output, cli->state_file);

	fclose(cli->state_file);

	cJSON_Delete(state);
	return success;
end:
	cJSON_Delete(state);
	return failure;

}
