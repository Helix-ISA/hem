#include "output/state.h"

#include "types.h"
#include "vendor/cJSON.h"
#include <stdio.h>

b8 dump_state(hx_processor* processor, FILE *out)
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
		reg = cJSON_CreateNumber((s64)processor->gpr[i]);
		if (reg == NULL)
			goto end;
		cJSON_AddItemToArray(registers, reg);
	}

	char *output = cJSON_Print(state);

	fputs(output, out);

	cJSON_Delete(state);
	return success;
end:
	cJSON_Delete(state);
	return failure;

}
