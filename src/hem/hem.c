#include "hem/hem.h"

#include "processor/memory.h"
#include "processor/processor.h"
#include "types.h"
#include "output/state.h"

#include <flagparser/flagparser.h>
#include <stdio.h>

static const fp_flag flags[] = {
	{
		.sname = "h",
		.lname = "help",
		.flag_type = FLAG_ARG_NONE,
		.hidden = true
	},
	{
		.sname = "v",
		.lname = "verbose",
		.flag_type = FLAG_ARG_NONE,
		.description = "Enable verbose mode"
	},
	{
		.sname = "o",
		.lname = "output",
		.flag_type = FLAG_ARG_REQUIRED,
		.value_type = "FILE",
		.default_value = "state.json",
		.description = "Specifies output file",
	}
};

static inline void error(const char *message)
{
	fprintf(stderr, "%s\n", message);
}

int hem(int argc, char **argv)
{
	b8 status = 0;

	const fp_config config = {
		.program_name = argv[0],
		.version = "0.1.1",
		.flags = flags,
		.flag_count = sizeof(flags) / sizeof(flags[0])
	};

	fp_result result = {0};

	FILE *in = NULL;
	FILE *out = NULL;
	b8 processor_initialized = false;
	b8 memory_initialized = false;

	if (!fp_flag_parse(&config, argc, argv, &result)) {
		fp_print_error(&config, result.error);
		status = 1;
		goto cleanup;
	}

	const fp_parsed_flag *help = fp_get_flag(&result, "help");
	if (help) {
		fp_print_usage(&config);
		goto cleanup;
	}

	if (result.positions.count < 1) {
		error("expected input files");
		status = 1;
		goto cleanup;
	} else if (result.positions.count > 1) {
		error("only supports a single binary");
		status = 1;
		goto cleanup;
	}

	if (result.positions.count == 0) {
		error("expected input file");
		status = 1;
		goto cleanup;
	}

	in = fopen(result.positions.values[0], "rb");
	if (in == NULL) {
		perror(result.positions.values[0]);
		status = 1;
		goto cleanup;
	}

	hx_processor processor;
	if (!processor_create(&processor)) {
		error("failed to create processor");
		status = 1;
		goto cleanup;
	}
	processor_initialized = true;

	hx_memory memory;
	if (!memory_create(&memory)) {
		error("failed to create memory");
		status = 1;
		goto cleanup;
	}
	memory_initialized = true;

	if (!processor_run(&processor, &memory, in)) {
		error("Failure when running program");
		status = 1;
		goto cleanup;
	}

	const fp_parsed_flag *out_file_flag = fp_get_flag(&result, "output");
	out = fopen(out_file_flag->value, "wb");
	if (out == NULL) {
		perror(out_file_flag->value);
		status = 1;
		goto cleanup;
	}

	dump_state(&processor, out);

cleanup:

	if (memory_initialized)
		memory_destroy(&memory);

	if (processor_initialized)
		processor_destroy(&processor);

	if (in != NULL)
		fclose(in);

	if (out != NULL)
		fclose(out);

	fp_result_free(&result);

	return status;
}
