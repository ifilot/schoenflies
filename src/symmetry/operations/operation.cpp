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

#include "operation.h"

/**
 * @brief Default constructor
 */
Operation::Operation() {}

/**
 * @brief Construct a new Operation object (inversion)
 *
 * @param element symmetry element to which this operation belongs
 */
Operation::Operation(OperationLabel::Element element) {
    if (element != OperationLabel::Element::Inversion) {
        throw std::logic_error("Incorrect Operation constructor used for this symmetry element.");
    }

    this->set_label(OperationLabel(element));
    this->matrix = this->calculate_matrix();
}

/**
 * @brief Construct a new Operation object (reflection)
 *
 * @param element symmetry element to which this operation belongs
 * @param normal plane normal of this operation
 */
Operation::Operation(OperationLabel::Element element, glm::vec3 normal) {
    if (element != OperationLabel::Element::Reflection) {
        throw std::logic_error("Incorrect Operation constructor used for this symmetry element.");
    }

    this->set_label(OperationLabel(element));
    this->axis = glm::normalize(normal);
    this->matrix = this->calculate_matrix();
}

/**
 * @brief Construct a new Operation object (proper/improper rotation)
 *
 * @param element symmetry element to which this operation belongs
 * @param degree degree of the rotation
 * @param axis rotation axis of this operation
 */
Operation::Operation(OperationLabel::Element element, unsigned int degree, glm::vec3 axis) {
    if (element != OperationLabel::Element::ProperRotation &&
        element != OperationLabel::Element::ImproperRotation) {
        throw std::logic_error("Incorrect Operation constructor used for this symmetry element.");
    }

    this->set_label(OperationLabel(element, degree));
    this->degree = degree;
    this->axis = glm::normalize(axis);
    this->matrix = this->calculate_matrix();
}

/**
 * @brief Get the id of this symmetry operation
 *
 * @return unsigned int
 */
const unsigned int Operation::get_id() const {
    return this->id;
}

/**
 * @brief Set the id of this symmetry operation
 *
 * @param id
 */
void Operation::set_id(unsigned int id) {
    this->id = id;
}

/**
 * @brief Get the label of this symmetry operation
 *
 * @return OperationLabel&
 */
OperationLabel& Operation::get_label() {
    return this->label;
}

/**
 * @brief Set the label of this symmetry operation
 *
 * @param label
 */
void Operation::set_label(OperationLabel label) {
    this->label = label;
}

/**
 * @brief Get the error of this symmetry operation
 *
 * @return const float
 */
const float Operation::get_error() const {
    if (isnan(this->error)) {
        throw std::runtime_error("Tried to get the error of a symmetry operation before it was computed.");
    }

    return this->error;
}

/**
 * @brief Get the degree of the operation as an integer
 *
 * @return const unsigned int
 */
const unsigned int Operation::get_degree() const {
    return this->degree;
}

/**
 * @brief Get the axis of the operation
 *
 * @return const glm::vec3
 */
const glm::vec3 Operation::get_axis() const {
    return this->axis;
}

/**
 * @brief Check whether this operation equals another operation
 *
 * @param other other operation
 * @return true if equal
 * @return false if inequal
 */
const bool Operation::operator==(Operation& other) const {
    if (this->label.get_element() != other.get_label().get_element()) return false;

    switch (this->label.get_element()) {
        case OperationLabel::Element::Inversion:
            return true;  // inversions do not have any degrees of freedom
        case OperationLabel::Element::ProperRotation:
        case OperationLabel::Element::ImproperRotation:
            // TODO move tolerance to constant or variable
            return this->get_degree() == other.get_degree() &&
                1 - std::abs(glm::dot(this->get_axis(), other.get_axis())) < .01;
        case OperationLabel::Element::Reflection:
            // TODO move tolerance to constant or variable
            return 1 - std::abs(glm::dot(this->get_axis(), other.get_axis())) < .01;
        default:
            throw std::runtime_error("Unexpected symmetry element encountered.");
    }
}

/**
 * @brief Perform the symmetry operation and set the error value
 *
 * @param structure structure on which to perform the operation
 */
void Operation::do_operation(std::shared_ptr<Structure> structure) {
    float max_error = 0;

    for (unsigned int i = 0; i < structure->get_num_atoms(); ++i) {
        glm::vec3 after = this->do_atom_operation(structure->get_coordinates(i));
        glm::vec3 closest_original_atom = structure->find_closest_coordinates(after, structure->get_atomic_number(i));
        float distance = glm::distance(after, closest_original_atom);

        float dist_to_element = this->get_distance_to_element(after);

        // prevent high errors with atoms close to the symmetry element
        float error = (dist_to_element > 1) ? distance / dist_to_element : distance;

        if (error > max_error) max_error = error;
    }

    this->error = max_error;
}

/**
 * @brief Perform the symmetry operation on a single atom
 *
 * @param coordinates coordinates of the atom
 * @return const glm::vec3 coordinates after performing the operation
 */
const glm::vec3 Operation::do_atom_operation(glm::vec3 coordinates) const {
    return this->matrix * coordinates;
}

/**
 * @brief Get the distance from the provided coordinates to the symmetry
 * element to which this operation belongs
 *
 * @param coordinates coordinates from which to determine distance
 * @return const float distance
 */
const float Operation::get_distance_to_element(glm::vec3 coordinates) const {
    switch (this->label.get_element()) {
        case OperationLabel::Element::Inversion:
            return this->get_distance_to_inversion(coordinates);
        case OperationLabel::Element::ProperRotation:
        case OperationLabel::Element::ImproperRotation:
            return this->get_distance_to_rotation(coordinates);
        case OperationLabel::Element::Reflection:
            return this->get_distance_to_reflection(coordinates);
        default:
            throw std::runtime_error("Unexpected symmetry element encountered.");
    }
}

/**
 * @brief Get the distance from the provided coordinates to an inversion
 * centre
 *
 * @param coordinates coordinates from which to determine distance
 * @return const float distance
 */
const float Operation::get_distance_to_inversion(glm::vec3 coordinates) const {
    // inversion centre is at origin
    return glm::length(coordinates);
}

/**
 * @brief Get the distance from the provided coordinates to a rotation axis
 *
 * @param coordinates coordinates from which to determine distance
 * @return const float distance
 */
const float Operation::get_distance_to_rotation(glm::vec3 coordinates) const {
    // vector rejection
    return glm::length(coordinates - glm::dot(coordinates, this->axis) * this->axis);
}

/**
 * @brief Get the distance from the provided coordinates to a reflection
 * plane
 *
 * @param coordinates coordinates from which to determine distance
 * @return const float distance
 */
const float Operation::get_distance_to_reflection(glm::vec3 coordinates) const {
    // because the plane goes through the origin and the normal is a unit vector,
    // the distance is equal to the (absolute) dot product
    return std::abs(glm::dot(this->axis, coordinates));
}

/**
 * @brief Calculate the operation matrix for this symmetry operation
 *
 * @return glm::mat3x3
 */
const glm::mat3x3 Operation::calculate_matrix() const {
    return this->calculate_fractional_matrix(1);
}

/**
 * @brief Calculate a fractional operation matrix for this symmetry
 * operation
 *
 * If the fraction is not 1, the operation is applied partially, used in
 * animating the symmetry operation.
 *
 * @param f progress fraction of operation
 * @return const glm::mat3x3
 */
const glm::mat3x3 Operation::calculate_fractional_matrix(double f) const {
    switch (this->label.get_element()) {
        case OperationLabel::Element::Inversion:
            return this->calculate_matrix_inversion(f);
        case OperationLabel::Element::ProperRotation:
            return this->calculate_matrix_proper_rotation(f);
        case OperationLabel::Element::ImproperRotation:
            return this->calculate_matrix_improper_rotation(f);
        case OperationLabel::Element::Reflection:
            return this->calculate_matrix_reflection(f);
        default:
            throw std::runtime_error("Unexpected symmetry element encountered.");
    }
}

/**
 * @brief Calculate the operation matrix for an inversion
 *
 * @return glm::mat3x3
 */
const glm::mat3x3 Operation::calculate_matrix_inversion(double f) const {
    return glm::mat3x3(1 - 2 * f);
}

/**
 * @brief Calculate the operation matrix for a proper rotation
 *
 * @return glm::mat3x3
 */
const glm::mat3x3 Operation::calculate_matrix_proper_rotation(double f) const {
    if (this->degree == DEGREE_INF) {
        return glm::mat3x3(1.0f);
    }

    float angle = 2 * M_PI / this->degree * f;

    float sin = std::sin(angle);
    float cos = std::cos(angle);

    glm::mat3x3 matrix;

    // note: GLM uses [col][row] notation
    matrix[0][0] = this->axis.x * this->axis.x * (1 - cos) + cos;
    matrix[0][1] = this->axis.y * this->axis.x * (1 - cos) + this->axis.z * sin;
    matrix[0][2] = this->axis.z * this->axis.x * (1 - cos) - this->axis.y * sin;

    matrix[1][0] = this->axis.x * this->axis.y * (1 - cos) - this->axis.z * sin;
    matrix[1][1] = this->axis.y * this->axis.y * (1 - cos) + cos;
    matrix[1][2] = this->axis.z * this->axis.y * (1 - cos) + this->axis.x * sin;

    matrix[2][0] = this->axis.x * this->axis.z * (1 - cos) + this->axis.y * sin;
    matrix[2][1] = this->axis.y * this->axis.z * (1 - cos) - this->axis.x * sin;
    matrix[2][2] = this->axis.z * this->axis.z * (1 - cos) + cos;

    return matrix;
}

/**
 * @brief Calculate the operation matrix for a reflection
 *
 * @return glm::mat3x3
 */
const glm::mat3x3 Operation::calculate_matrix_reflection(double f) const {
    glm::mat3x3 matrix;

    matrix[0][0] = 1 - 2 * f * this->axis.x * this->axis.x;
    matrix[0][1] = -2 * f * this->axis.x * this->axis.y;
    matrix[0][2] = -2 * f * this->axis.x * this->axis.z;

    matrix[1][0] = -2 * f * this->axis.x * this->axis.y;
    matrix[1][1] = 1 - 2 * f * this->axis.y * this->axis.y;
    matrix[1][2] = -2 * f * this->axis.y * this->axis.z;

    matrix[2][0] = -2 * f * this->axis.x * this->axis.z;
    matrix[2][1] = -2 * f * this->axis.y * this->axis.z;
    matrix[2][2] = 1 - 2 * f * this->axis.z * this->axis.z;

    return matrix;
}

/**
 * @brief Calculate the operation matrix for an improper rotation
 *
 * @return glm::mat3x3
 */
const glm::mat3x3 Operation::calculate_matrix_improper_rotation(double f) const {
    double f_rot = (f < .5) ? 2 * f : 1;
    double f_ref = (f > .5) ? 2 * f - 1 : 0;

    glm::mat3x3 rotation_matrix = this->calculate_matrix_proper_rotation(f_rot);
    glm::mat3x3 reflection_matrix = this->calculate_matrix_reflection(f_ref);

    // combine rotation and reflection
    return reflection_matrix * rotation_matrix;
}
