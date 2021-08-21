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

#include "reflection.h"

/**
 * @brief Construct a new Reflection object
 *
 * @param normal vector perpendicular to reflection plane
 */
Reflection::Reflection(glm::vec3 normal) {
    this->set_label(OperationLabel(OperationLabel::Element::Reflection));

    this->normal = glm::normalize(normal);

    // precompute reflection matrix
    // note: GLM uses [col][row] notation
    this->reflection_matrix[0][0] = 1 - 2 * this->normal.x * this->normal.x;
    this->reflection_matrix[0][1] = -2 * this->normal.x * this->normal.y;
    this->reflection_matrix[0][2] = -2 * this->normal.x * this->normal.z;

    this->reflection_matrix[1][0] = -2 * this->normal.x * this->normal.y;
    this->reflection_matrix[1][1] = 1 - 2 * this->normal.y * this->normal.y;
    this->reflection_matrix[1][2] = -2 * this->normal.y * this->normal.z;

    this->reflection_matrix[2][0] = -2 * this->normal.x * this->normal.z;
    this->reflection_matrix[2][1] = -2 * this->normal.y * this->normal.z;
    this->reflection_matrix[2][2] = 1 - 2 * this->normal.z * this->normal.z;
}

/**
 * @brief Perform the symmetry operation on a single atom
 *
 * @param coordinates coordinates of the atom
 * @return const glm::vec3 coordinates after performing the operation
 */
const glm::vec3 Reflection::do_atom_operation(glm::vec3 coordinates) const {
    return this->reflection_matrix * coordinates;
}

/**
 * @brief Get the distance from the provided coordinates to the symmetry
 * element to which this operation belongs.
 *
 * @param coordinates coordinates from which to determine distance
 * @return const float distance
 */
const float Reflection::get_distance_to_element(glm::vec3 coordinates) const {
    // because the plane goes through the origin and the normal is a unit vector,
    // the distance is equal to the (absolute) dot product
    return std::abs(glm::dot(this->normal, coordinates));
}

/**
 * @brief Check whether this operation equals another operation
 *
 * @param other other operation
 * @return true if equal
 * @return false if inequal
 */
const bool Reflection::equals(Reflection& other) const {
    // TODO move tolerance to constant or variable
    return 1 - std::abs(glm::dot(this->get_normal(), other.get_normal())) < .01;
}

/**
 * @brief Get the normal of the operation
 *
 * @return const glm::vec3
 */
const glm::vec3 Reflection::get_normal() const {
    return this->normal;
}
