/**
 * Schoenflies
 * Copyright (c) 2021 Luuk Kempen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SYMMETRY_SYMMETRY_H
#define SYMMETRY_SYMMETRY_H

#include <Eigen/Dense>
#include "../structure.h"
#include "rotor_class.h"

class Structure;  // forward declaration

class Symmetry {
private:
    Structure* structure;

    Eigen::Vector3d principal_moments;
    Eigen::Matrix3d principal_axes;

    RotorClass rotor_class;

public:
    /**
     * @brief Default constructor
     */
    Symmetry();

    /**
     * @brief Construct a new Symmetry object
     *
     * @param structure
     */
    Symmetry(Structure* structure);

private:
    /**
     * @brief Calculate the inertial tensor and use this to determine principal
     * axes and their principal moments.
     */
    void determine_principal_axes();

    /**
     * @brief Determine the rotor class of the structure (given by the
     * degeneracy of the inertial moments).
     */
    void determine_rotor_class();
};

#endif  // SYMMETRY_SYMMETRY_H
