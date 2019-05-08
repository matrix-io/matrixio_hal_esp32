/*
 * Copyright 2016 <Admobilize>
 * MATRIX Labs  [http://creator.matrix.one]
 * This file is part of MATRIX Creator HAL
 *
 * MATRIX Creator HAL is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CPP_DRIVER_MICARRAY_LOCATION_H_
#define CPP_DRIVER_MICARRAY_LOCATION_H_

#include <string>
#include "matrix_driver.h"

namespace matrix_hal {

/*
  x,y  position in milimeters
 */

static float micarray_location[8][2] = {
    { 		0	, 		0 },  /* M1 */
    { -38.133	, +03.576 }, /* M2 */
    { -20.980	, +32.043 }, /* M3 */
    { +11.971	, +36.381 },  /* M4 */
    { +35.908	, +13.323 },  /* M5 */
    { +32.805	, -19.767 },   /* M6 */
    { +04.999	, -37.972 },   /* M7 */
    { -26.571	, -27.584 }   /* M8 */
};

};      // namespace matrix_hal
#endif  // CPP_DRIVER_MICARRAY_LOCATION_H_
