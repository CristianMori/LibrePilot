/**
 ******************************************************************************
 * @addtogroup OpenPilot Math Utilities
 * @{
 * @addtogroup Sine and cosine methods that use a cached lookup table
 * @{
 *
 * @file       sin_lookup.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2012.
 * @brief      Sine lookup table from flash with 1 degree resolution
 *
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "openpilot.h"
#include "math.h"
#include "stdbool.h"
#include "stdint.h"

//#define FLASH_TABLE
#ifdef FLASH_TABLE
 /** Version of the code which precomputes the lookup table in flash **/

 // This is a precomputed sin lookup table over 90 degrees that
 const float sin_table[] = {
 	0.000000f,0.017452f,0.034899f,0.052336f,0.069756f,0.087156f,0.104528f,0.121869f,0.139173f,0.156434f,
 	0.173648f,0.190809f,0.207912f,0.224951f,0.241922f,0.258819f,0.275637f,0.292372f,0.309017f,0.325568f,
 	0.342020f,0.358368f,0.374607f,0.390731f,0.406737f,0.422618f,0.438371f,0.453990f,0.469472f,0.484810f,
 	0.500000f,0.515038f,0.529919f,0.544639f,0.559193f,0.573576f,0.587785f,0.601815f,0.615661f,0.629320f,
 	0.642788f,0.656059f,0.669131f,0.681998f,0.694658f,0.707107f,0.719340f,0.731354f,0.743145f,0.754710f,
 	0.766044f,0.777146f,0.788011f,0.798636f,0.809017f,0.819152f,0.829038f,0.838671f,0.848048f,0.857167f,
 	0.866025f,0.874620f,0.882948f,0.891007f,0.898794f,0.906308f,0.913545f,0.920505f,0.927184f,0.933580f,
 	0.939693f,0.945519f,0.951057f,0.956305f,0.961262f,0.965926f,0.970296f,0.974370f,0.978148f,0.981627f,
 	0.984808f,0.987688f,0.990268f,0.992546f,0.994522f,0.996195f,0.997564f,0.998630f,0.999391f,0.999848f,
 	1.000000f,0.999848f,0.999391f,0.998630f,0.997564f,0.996195f,0.994522f,0.992546f,0.990268f,0.987688f,
 	0.984808f,0.981627f,0.978148f,0.974370f,0.970296f,0.965926f,0.961262f,0.956305f,0.951057f,0.945519f,
 	0.939693f,0.933580f,0.927184f,0.920505f,0.913545f,0.906308f,0.898794f,0.891007f,0.882948f,0.874620f,
 	0.866025f,0.857167f,0.848048f,0.838671f,0.829038f,0.819152f,0.809017f,0.798636f,0.788011f,0.777146f,
 	0.766044f,0.754710f,0.743145f,0.731354f,0.719340f,0.707107f,0.694658f,0.681998f,0.669131f,0.656059f,
 	0.642788f,0.629320f,0.615661f,0.601815f,0.587785f,0.573576f,0.559193f,0.544639f,0.529919f,0.515038f,
 	0.500000f,0.484810f,0.469472f,0.453990f,0.438371f,0.422618f,0.406737f,0.390731f,0.374607f,0.358368f,
 	0.342020f,0.325568f,0.309017f,0.292372f,0.275637f,0.258819f,0.241922f,0.224951f,0.207912f,0.190809f,
 	0.173648f,0.156434f,0.139173f,0.121869f,0.104528f,0.087156f,0.069756f,0.052336f,0.034899f,0.017452f
 };

int sin_lookup_initalize()
{
	return 0;
}

#else
/** Version of the code which allocates the lookup table in heap **/

const int SIN_RESOLUTION = 180;

static float *sin_table;
int sin_lookup_initalize()
{
	// Previously initialized
	if (sin_table)
		return 0;

	sin_table = (float *) pvPortMalloc(sizeof(float) * SIN_RESOLUTION);
	if (sin_table == NULL)
		return -1;

	for(uint32_t i = 0; i < 180; i++)
		sin_table[i] = sinf((float)i * 2 * M_PI / 360.0f);

	return 0;
}

#endif

/**
 * Use the lookup table to return sine(angle) where angle is in radians
 * to save flash this cheats and uses trig functions to only save
 * 90 values
 * @param[in] angle Angle in degrees
 * @returns sin(angle)
*/
float sin_lookup_deg(float angle)
{
	int i_ang = ((int32_t) angle) % 360;
	if (i_ang > 180)      // for 180 to 270 deg
		return -sin_table[i_ang - 180];
	else                       // for 0 to 90 deg
		return sin_table[i_ang];

	return 0;
}

/**
 * Get cos(angle) using the sine lookup table
 * @param[in] angle Angle in degrees
 * @returns cos(angle)
 */
float cos_lookup_deg(float angle)
{
	return sin_lookup_deg(angle + 90);
}

/**
 * Use the lookup table to return sine(angle) where angle is in radians
 * @param[in] angle Angle in radians
 * @returns sin(angle)
 */
float sin_lookup_rad(float angle)
{
	int degrees = angle * 180.0f / M_PI;
	return sin_lookup_deg(degrees);
}

/**
 * Use the lookup table to return sine(angle) where angle is in radians
 * @param[in] angle Angle in radians
 * @returns cos(angle)
 */
float cos_lookup_rad(float angle)
{
	int degrees = angle * 180.0f / M_PI;
	return cos_lookup_deg(degrees);
}