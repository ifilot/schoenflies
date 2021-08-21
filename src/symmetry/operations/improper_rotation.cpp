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

#include "improper_rotation.h"

/**
 * @brief Construct a new Improper Rotation object
 *
 * @param degree degree of the rotation
 * @param axis rotational axis of the operation
 */
ImproperRotation::ImproperRotation(unsigned int degree, glm::vec3 axis) {
    this->degree = degree;
    this->angle = 2 * M_PI / degree;
    this->axis = glm::normalize(axis);

    // precompute rotation matrix
    glm::mat3x3 rotation_matrix;
    if (degree != DEGREE_INF) {
        float sin = std::sin(this->angle);
        float cos = std::cos(this->angle);

        // note: GLM uses [col][row] notation
        rotation_matrix[0][0] = this->axis.x * this->axis.x * (1 - cos) + cos;
        rotation_matrix[0][1] = this->axis.y * this->axis.x * (1 - cos) + this->axis.z * sin;
        rotation_matrix[0][2] = this->axis.z * this->axis.x * (1 - cos) - this->axis.y * sin;

        rotation_matrix[1][0] = this->axis.x * this->axis.y * (1 - cos) - this->axis.z * sin;
        rotation_matrix[1][1] = this->axis.y * this->axis.y * (1 - cos) + cos;
        rotation_matrix[1][2] = this->axis.z * this->axis.y * (1 - cos) + this->axis.x * sin;

        rotation_matrix[2][0] = this->axis.x * this->axis.z * (1 - cos) + this->axis.y * sin;
        rotation_matrix[2][1] = this->axis.y * this->axis.z * (1 - cos) - this->axis.x * sin;
        rotation_matrix[2][2] = this->axis.z * this->axis.z * (1 - cos) + cos;
    }

    // precompute reflection matrix
    glm::mat3x3 reflection_matrix;

    // note: GLM uses [col][row] notation
    reflection_matrix[0][0] = 1 - 2 * this->axis.x * this->axis.x;
    reflection_matrix[0][1] = -2 * this->axis.x * this->axis.y;
    reflection_matrix[0][2] = -2 * this->axis.x * this->axis.z;

    reflection_matrix[1][0] = -2 * this->axis.x * this->axis.y;
    reflection_matrix[1][1] = 1 - 2 * this->axis.y * this->axis.y;
    reflection_matrix[1][2] = -2 * this->axis.y * this->axis.z;

    reflection_matrix[2][0] = -2 * this->axis.x * this->axis.z;
    reflection_matrix[2][1] = -2 * this->axis.y * this->axis.z;
    reflection_matrix[2][2] = 1 - 2 * this->axis.z * this->axis.z;

    // combine rotation and reflection
    if (degree != DEGREE_INF) {
        this->rotation_reflection_matrix = reflection_matrix * rotation_matrix;
    } else {
        this->rotation_reflection_matrix = reflection_matrix;
    }
}

/**
 * @brief Perform the symmetry operation on a single atom
 *
 * @param coordinates coordinates of the atom
 * @return const glm::vec3 coordinates after performing the operation
 */
const glm::vec3 ImproperRotation::do_atom_operation(glm::vec3 coordinates) const {
    return this->rotation_reflection_matrix * coordinates;
}

/**
 * @brief Get the distance from the provided coordinates to the symmetry
 * element to which this operation belongs.
 *
 * @param coordinates coordinates from which to determine distance
 * @return const float distance
 */
const float ImproperRotation::get_distance_to_element(glm::vec3 coordinates) const {
    // vector rejection
    return glm::length(coordinates - glm::dot(coordinates, this->axis) * this->axis);
}

/**
 * @brief Check whether this operation equals another operation
 *
 * @param other other operation
 * @return true if equal
 * @return false if inequal
 */
const bool ImproperRotation::equals(ImproperRotation& other) const {
    // TODO move tolerance to constant or variable
    return this->get_degree() == other.get_degree() &&
        1 - std::abs(glm::dot(this->get_axis(), other.get_axis())) < .01;
}

/**
 * @brief Get the name of a symmetry operation in plaintext
 *
 * @return const std::string
 */
const std::string ImproperRotation::get_name() const {
    return "S" + this->get_degree_str() + " improper rotation";
}

/**
 * @brief Get the name of a symmetry operation in HTML formatting
 *
 * @return const std::string
 */
const std::string ImproperRotation::get_name_html() const {
    return "<i>S</i><sub>" + this->get_degree_str() + "</sub> improper rotation";
}

/**
 * @brief Get the degree of the operation as a string
 *
 * @return const std::string
 */
const std::string ImproperRotation::get_degree_str() const {
    return (this->degree == DEGREE_INF) ? "∞" : std::to_string(this->degree);
}

/**
 * @brief Get the degree of the operation as an integer
 *
 * @return const unsigned int
 */
const unsigned int ImproperRotation::get_degree() const {
    return this->degree;
}

/**
 * @brief Get the axis of the operation
 *
 * @return const glm::vec3
 */
const glm::vec3 ImproperRotation::get_axis() const {
    return this->axis;
}
