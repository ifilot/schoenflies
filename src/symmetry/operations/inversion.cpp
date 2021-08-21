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

#include "inversion.h"

/**
 * @brief Construct a new Inversion object
 */
Inversion::Inversion() {
    this->set_label(OperationLabel(OperationLabel::Element::Inversion));
}

/**
 * @brief Perform the symmetry operation on a single atom
 *
 * @param coordinates coordinates of the atom
 * @return const glm::vec3 coordinates after performing the operation
 */
const glm::vec3 Inversion::do_atom_operation(glm::vec3 coordinates) const {
    return -coordinates;
}

/**
 * @brief Get the distance from the provided coordinates to the symmetry
 * element to which this operation belongs.
 *
 * @param coordinates coordinates from which to determine distance
 * @return const float distance
 */
const float Inversion::get_distance_to_element(glm::vec3 coordinates) const {
    return glm::length(coordinates);
}

/**
 * @brief Check whether this operation equals another operation
 *
 * @param other other operation
 * @return true if equal
 * @return false if inequal
 */
const bool Inversion::equals(Inversion& other) const {
    return true;  // inversions do not have any degrees of freedom
}
