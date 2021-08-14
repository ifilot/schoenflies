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

#include <iostream>
#include <memory>
#include <Eigen/Dense>
#include "../structure.h"
#include "rotor_class.h"
#include "operations/inversion.h"

class Symmetry {
private:
    std::shared_ptr<Structure> structure;

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
    Symmetry(std::shared_ptr<Structure> structure);

    /**
     * @brief Get the principal moments of the structure
     *
     * @return const Eigen::Vector3d&
     */
    const Eigen::Vector3d& get_principal_moments() const;

    /**
     * @brief Get the principal axes of the structure
     *
     * @return const Eigen::Matrix3d&
     */
    const Eigen::Matrix3d& get_principal_axes() const;

    /**
     * @brief Get the rotor class of the structure
     *
     * @return const RotorClass
     */
    const RotorClass get_rotor_class() const;

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

    /**
     * @brief Find all symmetry operations of the structure.
     */
    void find_symmetry_operations();

    /**
     * @brief Find an inversion centre in the structure.
     */
    void find_inversion_centre();
};

#endif  // SYMMETRY_SYMMETRY_H
