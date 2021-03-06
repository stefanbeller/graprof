
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


#include "strtime.h"

#define MAX_TIMEVAL 100000

void 
strtime (unsigned long long *time, const char **prefix)
{
  *prefix = "n";
  if (*time >= MAX_TIMEVAL)
    {
      *prefix = "µ";
      *time /= 1000;
    }

  if (*time >= MAX_TIMEVAL)
    {
      *prefix = "m";
      *time /= 1000;
    }

  if (*time >= MAX_TIMEVAL)
    {
      *prefix = " ";
      *time /= 1000;
    }
}
