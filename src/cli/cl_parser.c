#include "cli/cl_parser.h"

#include "types.h"
#include <stdio.h>
#include <string.h>

b8 cl_parse_init(hx_cli *cli)
{
	cli->input_file = NULL;
	cli->dump_state = false;

	return success;
}

b8 cl_parse_free(hx_cli *cli)
{
	fclose(cli->input_file);

	return success;
}

b8 cl_parse_args(s32 count, char **args, hx_cli *cli)
{
	for (s32 i = 1; i < count; i++) {
		if (strcmp(args[i], "--dump-state") == 0) {
			cli->dump_state = true;
		} else if (strncmp(args[i], "-", 1) == 0) {
			fprintf(stderr, "unknown flag %s\n", args[i]);
			return failure;
		} else {
			cli->input_file = fopen(args[1], "rb");

			if (cli->input_file == NULL) {
				perror("fopen");
				return failure;
			}
		}
	}

	if (cli->input_file == NULL) {
		fprintf(stderr, "expected input file\n");
		return failure;
	}


	return success;
}
