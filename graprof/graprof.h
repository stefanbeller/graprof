
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013  Andreas Grapentin                                   *
 *                                                                            *
 *    This program is free software: you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation, either version 3 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 ******************************************************************************/


#pragma once

#include <config.h>

#include <argp.h>

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_adress = PACKAGE_BUGREPORT;

const char doc[] = PACKAGE_NAME " - a profiling and trace analysis tool";

const char args_doc[] = "<binary file> <trace file>";

static struct argp_option options[] =
{
    {"flat-profile", 'F', 0, 0, "generate flat profile from trace data", 0},
    {"call-graph", 'C', 0, 0, "generete call graph from trace data", 0},
    {"memory-profile", 'M', 0, 0, "generate flat memory profile from trace data", 0},
    {"no-gui", 'g', 0, 0, "do not open the trace explorer gui", 0},
    {"output-trace", 'O', "<file>", 0, "write tracefile to <file> instead of defaulting to graprof.out. This has higher priority than the GRAPROF_OUT variable", 0},
    {"output", 'o', "<file>", 0, "write profile results to <file> instead of stdout.", 0},
    {"analyze-only", 'a', "<file>", 0, "use existing trace file and analyze it", 0},
    {"verbose", 'v', 0, 0, "add descriptions to profiling output", 0},
    {0, 0, 0, 0, 0, 0}
};

#define GRAPROF_FLAT_PROFILE      0x01
#define GRAPROF_CALL_GRAPH        0x02
#define GRAPROF_MEMORY_PROFILE    0x04
#define GRAPROF_NO_GUI            0x08
#define GRAPROF_NO_ANALYSIS       0x10

struct arguments
{
  const char *binary_filename;
  const char *trace_filename;

  int tasks;
  const char *out_filename;
  int verbose;
  int profilee_index; // the index in argv when the profilee starts, 0 if no -- is there.
};

static int next_arg;

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = (struct arguments*)state->input;

  switch(key)
    {
    case 'F':
      arguments->tasks |= GRAPROF_FLAT_PROFILE;
      break;
    case 'C':
      arguments->tasks |= GRAPROF_CALL_GRAPH;
      break;
    case 'M':
      arguments->tasks |= GRAPROF_MEMORY_PROFILE;
      break;
    case 'g':
      arguments->tasks |= GRAPROF_NO_GUI;
      break;
    case 'o':
      arguments->out_filename = arg;
      break;
    case 'a':
      arguments->tasks |= GRAPROF_NO_ANALYSIS;
      /* intentional fall through */
    case 'O':
      arguments->trace_filename = arg;
      break;
    case 'v':
      arguments->verbose++;
      break;
    case ARGP_KEY_ARG:
      arguments->profilee_index = state->next - 1;
      arguments->binary_filename = arg;
      /* abort parsing completely when encountering the binary name,
       * as the following options are for the binary,
       * we don't understand them
       */
      state->next = state->argc;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 2)
        argp_usage(state);
    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};
