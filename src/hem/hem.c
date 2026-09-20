#include "hem/hem.h"

#include "cli/cl_parser.h"
#include "output/state.h"
#include "processor/memory.h"
#include "processor/processor.h"

int hem(int argc, char **argv)
{
	hx_cli cli;
	u8 status = 0;

	if (!cl_parse_init(&cli))
		return 1;

	if (!cl_parse_args(argc, argv, &cli)) {
		status = 1;
		goto cleanup_cli;
	}

	hx_processor processor;
	if (!processor_create(&processor)) {
		status = 1;
		goto cleanup_cli;
	}

	hx_memory memory;
	if (!memory_create(&memory)) {
		status = 1;
		goto cleanup_cli;
	}

	if (!processor_run(&processor, &memory, &cli)) {
		status = 1;
		goto cleanup_cli;
	}

	if (cli.dump_state) {
		if (!dump_state(&processor)) {
			status = 1;
			goto cleanup_cli;
		}
	}

	if (!memory_destroy(&memory)) {
		status = 1;
		goto cleanup_cli;
	}

	if (!processor_destroy(&processor)) {
		status = 1;
		goto cleanup_cli;
	}

cleanup_cli:
	cl_parse_free(&cli);

	return status;
}
