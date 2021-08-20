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

#include <limits>
#include <math.h>
#include <memory>
#include <vector>
#include <Eigen/Dense>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include "../structure.h"
#include "rotor_class.h"
#include "operations/improper_rotation.h"
#include "operations/inversion.h"
#include "operations/operation.h"
#include "operations/proper_rotation.h"
#include "operations/reflection.h"
#include "point_groups/point_group.h"
#include "point_groups/point_groups.h"

class Symmetry {
private:
    std::shared_ptr<Structure> structure;

    glm::vec3 principal_moments;
    glm::mat3x3 principal_axes;

    RotorClass rotor_class;

    std::vector<Inversion> inversions;
    std::vector<ProperRotation> proper_rotations;
    std::vector<ImproperRotation> improper_rotations;
    std::vector<Reflection> reflections;

    PointGroup point_group;

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
     * @return const glm::vec3&
     */
    const glm::vec3& get_principal_moments() const;

    /**
     * @brief Get the principal axes of the structure
     *
     * @return const glm::mat3x3&
     */
    const glm::mat3x3& get_principal_axes() const;

    /**
     * @brief Get the rotor class of the structure
     *
     * @return const RotorClass
     */
    const RotorClass get_rotor_class() const;

    /**
     * @brief Get the list of inversion operations present in the structure
     *
     * @return const std::vector<Inversion>&
     */
    const std::vector<Inversion>& get_inversions() const;

    /**
     * @brief Get the list of proper rotation operations present in the
     * structure
     *
     * @return const std::vector<ProperRotation>&
     */
    const std::vector<ProperRotation>& get_proper_rotations() const;

    /**
     * @brief Get the list of improper rotation operations present in the
     * structure
     *
     * @return const std::vector<ImproperRotation>&
     */
    const std::vector<ImproperRotation>& get_improper_rotations() const;

    /**
     * @brief Get the list of reflection operations present in the structure
     *
     * @return const std::vector<Reflection>&
     */
    const std::vector<Reflection>& get_reflections() const;

    /**
     * @brief Get the point group of the structure
     *
     * @return const PointGroup&
     */
    const PointGroup& get_point_group() const;

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
     * @brief Find improper rotational axes in the structure.
     */
    void find_improper_rotational_axes();

    /**
     * @brief Find reflection planes in the structure.
     */
    void find_reflection_planes();

    /**
     * @brief Find reflection planes which are normal to the principal axes of
     * the structure.
     */
    void find_reflection_planes_normal_to_principal_axes();

    /**
     * @brief Find reflection planes which are normal to proper rotational
     * axes.
     *
     * @param only_C2s whether to only search normal to C2 rotational axes (in
     * case of octahedral and icosahedral symmetry)
     */
    void find_reflection_planes_normal_to_proper_rotational_axes(bool only_C2s);

    /**
     * @brief Find reflection planes which pass through midpoints between pairs
     * of atoms of the same element.
     */
    void find_reflection_planes_in_midpoints();

    /**
     * @brief Find the point group with the highest match to the found symmetry
     * operations.
     */
    void find_point_group();

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
