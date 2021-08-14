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
#include <math.h>
#include <memory>
#include <vector>
#include <Eigen/Dense>
#include "../structure.h"
#include "rotor_class.h"
#include "operations/inversion.h"
#include "operations/operation.h"
#include "operations/proper_rotation.h"

class Symmetry {
private:
    std::shared_ptr<Structure> structure;

    Eigen::Vector3d principal_moments;
    Eigen::Matrix3d principal_axes;

    RotorClass rotor_class;

    std::vector<Inversion> inversions;
    std::vector<ProperRotation> rotations;

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

    /**
     * @brief Find proper rotational axes in the structure.
     */
    void find_proper_rotational_axes();

    /**
     * @brief Find proper rotational axes along the principal axes of the
     * structure.
     */
    void find_proper_rotational_axes_along_principal_axes();

    /**
     * @brief Find proper rotational axes through the centre of mass and an
     * atom of the structure.
     */
    void find_proper_rotational_axes_through_atoms();

    /**
     * @brief Find proper rotational axes through the centre of mass and the
     * midpoints between pairs of atoms of the same element.
     */
    void find_proper_rotational_axes_between_atoms();

    /**
     * @brief Find proper rotational axes through polygonal faces for
     * structures classified as spherical top (cubic).
     */
    void find_proper_rotational_axes_polygonal_faces();

    /**
     * @brief Find proper rotational axes through polygonal faces for
     * structures with tetrahedral symmetry (T point group, 3 C2 rotations) and
     * octahedral symmetry (O point group, 9 C2 rotations).
     */
    void find_proper_rotational_axes_polygonal_faces_T_O();

    /**
     * @brief Find proper rotational axes through polygonal faces for
     * structures with icosahedral symmetry (I point group, 15 C2 rotations).
     *
     * @param C2s C2 rotations present in the structure
     */
    void find_proper_rotational_axes_polygonal_faces_I(std::vector<ProperRotation> C2s);

    /**
     * @brief Check whether an axis can be a symmetry axis based on the
     * inertial tensor.
     *
     * A symmetry operation must leave a molecule unchanged, which sets
     * restrictions on whether an axis or plane can be a symmetry axis or plane
     * based on the principal axes. Checking this early leads to increased
     * efficiency.
     *
     * @param axis the axis to check
     * @return true if axis can be a symmetry axis
     * @return false if axis cannot be a symmetry axis
     */
    bool axis_inertially_allowed(glm::vec3& axis);

    /**
     * @brief Check whether a symmetry operation exists in the structure.
     *
     * @param operation the symmetry operation to check
     * @return true if it exists
     * @return false if it doesn't exist
     */
    bool check_operation(Operation& operation);

    /**
     * @brief Add an operation to the list of operations, if it does not
     * already exist yet.
     *
     * @tparam T class derived from Operation
     * @param operation operation to add
     * @param operations vector of operations to add to
     */
    template <class T>
    void add_operation(T& operation, std::vector<T>& operations);

    /**
     * @brief Check whether a symmetry operation exists in the structure and
     * add it to the list of operations, if it does not already exist yet.
     *
     * @tparam T class derived from Operation
     * @param operation operation to check and add
     * @param operations vector of operations to add to
     */
    template <class T>
    void check_and_add_operation(T& operation, std::vector<T>& operations);
};

#endif  // SYMMETRY_SYMMETRY_H
