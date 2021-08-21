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
 * @brief Get the label of this symmetry operation
 *
 * @return const OperationLabel&
 */
const OperationLabel& Operation::get_label() const {
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
