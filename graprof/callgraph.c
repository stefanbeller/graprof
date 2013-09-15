
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


#include "callgraph.h"

#include "trace.h"
#include "strtime.h"
#include "function.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

extern FILE *graprof_out;

void
callgraph_print (void)
{
  fprintf(graprof_out, "Call graph:\n");
  fprintf(graprof_out, "\n");

  const char *prefix;
  unsigned long long time = trace_get_total_runtime();

  strtime(&time, &prefix);

  fprintf(graprof_out, " total runtime:                  %llu %sseconds\n", time, prefix);
  fprintf(graprof_out, " total number of function calls: %llu\n", function_get_total_calls());
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, "          %%       self     children\n");
  fprintf(graprof_out, "  index  time      time      time       called       name\n");

  unsigned int nfunctions = 0;
  function *functions = function_get_all(&nfunctions);

  unsigned int i;
  for (i = 0; i < nfunctions; ++i)
    {
      function *f = functions + i;

      // print parents
      unsigned int j;
      for (j = 0; j < f->ncallers; ++j)
        {
          if (f->callers[j].function_id == (unsigned int)-1)
            {
              fprintf(graprof_out, "                                   %8lu/%-8lu      <spontaneous>\n", f->callers[j].calls, f->calls);
            }
          else
            {
              time = f->callers[j].self_time;
              strtime(&time, &prefix);
              fprintf(graprof_out, "               %6llu %ss ", time, prefix);

              time = f->callers[j].children_time;
              strtime(&time, &prefix);
              fprintf(graprof_out, "%6llu %ss ", time, prefix);

              fprintf(graprof_out, "%8lu/%-8lu  ", f->callers[j].calls, f->calls);

              if (!strcmp(functions[f->callers[j].function_id].name, "??"))
                fprintf(graprof_out, "    0x%" PRIxPTR , functions[f->callers[j].function_id].address);
              else
                fprintf(graprof_out, "    %s", functions[f->callers[j].function_id].name);
              fprintf(graprof_out, " [%u]\n", f->callers[j].function_id);
            }
        }

      // print self
      time = trace_get_total_runtime();
      fprintf(graprof_out, " %6u %6.2f ", i, (100.0 * (f->self_time + f->children_time)) / time);

      time = f->self_time;
      strtime(&time, &prefix);
      fprintf(graprof_out, "%6llu %ss ", time, prefix);

      time = f->children_time;
      strtime(&time, &prefix);
      fprintf(graprof_out, "%6llu %ss ", time, prefix);

      fprintf(graprof_out, "%8lu           ", f->calls);

      if (!strcmp(f->name, "??"))
        fprintf(graprof_out, "0x%" PRIxPTR , f->address);
      else
        fprintf(graprof_out, "%s", f->name);
      fprintf(graprof_out, " [%u]\n", i);

      // print children
      for (j = 0; j < f->ncallees; ++j)
        {
          time = f->callees[j].self_time;
          strtime(&time, &prefix);
          fprintf(graprof_out, "               %6llu %ss ", time, prefix);

          time = f->callees[j].children_time;
          strtime(&time, &prefix);
          fprintf(graprof_out, "%6llu %ss ", time, prefix);

          fprintf(graprof_out, "%8lu/%-8lu  ", f->callees[j].calls, functions[f->callees[j].function_id].calls);

          if (!strcmp(functions[f->callees[j].function_id].name, "??"))
            fprintf(graprof_out, "    0x%" PRIxPTR , functions[f->callees[j].function_id].address);
          else
            fprintf(graprof_out, "    %s", functions[f->callees[j].function_id].name);
          fprintf(graprof_out, " [%u]\n", f->callees[j].function_id);
        }

      fprintf(graprof_out, " -------------------------------------------------------\n");
    }

  fprintf(graprof_out, "\n");

  fprintf(graprof_out, " This table describes the call tree of the program, and was\n");
  fprintf(graprof_out, " sorted by the total amount of time spent in and below each\n");
  fprintf(graprof_out, " function in the call graph.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, " Each entry in this table consists of several lines. The line\n");
  fprintf(graprof_out, " with the index number at the left hand margin lists the current\n");
  fprintf(graprof_out, " function, and the lines above it lists the functions callers, and\n");
  fprintf(graprof_out, " the functions below it list the functions callees.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, " The current function line lists:\n");
  fprintf(graprof_out, "   index      A unique function identifier that is also referenced\n");
  fprintf(graprof_out, "              in the name field of this listing.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, "   %%          This is the percantage of the total time spent in and\n");
  fprintf(graprof_out, "   time       below this function in the call tree.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, "   self       This is the total amount of time spent in this function.\n");
  fprintf(graprof_out, "   time       \n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, "   children   This is the total amount of time spent in the descendants\n");
  fprintf(graprof_out, "   time       of this function.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, "   calls      This is the number of times this function was invoked.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, "   name       The name of the function.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, " For the functions callers, the fields have the following meaning:\n");
  fprintf(graprof_out, "   self       This is the amount of time that was propagated directly\n");
  fprintf(graprof_out, "   time       from the function into this caller.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, "   children   This is the amount of time that was propagated from the\n");
  fprintf(graprof_out, "   time       functions children into this caller.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, "   calls      This is the number of times this caller called the function,\n");
  fprintf(graprof_out, "              out of the total number this function was called.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, "   name       The name of the caller.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, " If the caller of a function can not be determined, it will be registered\n");
  fprintf(graprof_out, " as <spontaneous> in the name field.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, " For the functions callees, the fields have the following meaning:\n");
  fprintf(graprof_out, "   self       This is the amount of time that was propagated directly from\n");
  fprintf(graprof_out, "   time       this callee into the function.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, "   children   This is the amount of time that was propagated from the\n");
  fprintf(graprof_out, "   time       callees children into the function.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, "   calls      This is the total number of times the callee function was\n");
  fprintf(graprof_out, "              called from the function, of the total times the callee was\n");
  fprintf(graprof_out, "              called.\n");
  fprintf(graprof_out, " \n");
  fprintf(graprof_out, "   name       The name of the callee.\n");
  fprintf(graprof_out, " \n");

}
