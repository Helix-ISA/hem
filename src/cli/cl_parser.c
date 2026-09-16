#include "cli/cl_parser.h"

#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

b8 cl_parse_init(hx_cli *cli)
{
	cli->input_file = NULL;

	return success;
}

b8 cl_parse_free(hx_cli *cli)
{
	fclose(cli->input_file);

	return success;
}

b8 cl_parse_args(s32 count, char **args, hx_cli *cli)
{
	(void)count;
	if (count < 2) {
		fprintf(stderr, "expected input file\n");
		exit(1);
	}

	cli->input_file = fopen(args[1], "rb");

	if (cli->input_file == NULL) {
		perror("fopen");
		return failure;
	}

	return success;
}
