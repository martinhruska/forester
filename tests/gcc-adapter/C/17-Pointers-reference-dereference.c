/* Test case: #17
 *
 * Copyright NOTE: This file is part of predator's test suite.
 *
 * predator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * predator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with predator. If not, see <http://www.gnu.org/licenses/>.
 */

int main(void)
{
  char c1 = 0, c2 = 1; 
  char *c_ptr;
  char **c_pptr;

  c_ptr = &c1;
  *c_ptr = c2;
  c_ptr = &c2;
  *c_ptr = 0;
  c_pptr = &c_ptr;
  *c_pptr = &c1;
  **c_pptr = 1;

  return 0;
}