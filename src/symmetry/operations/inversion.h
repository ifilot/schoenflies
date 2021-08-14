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

#ifndef SYMMETRY_ELEMENTS_INVERSION_H
#define SYMMETRY_ELEMENTS_INVERSION_H

#include <string>
#include <glm/glm.hpp>
#include "operation.h"

class Inversion: public Operation {
public:
    /**
     * @brief Construct a new Inversion object
     */
    Inversion();

    /**
     * @brief Perform the symmetry operation on a single atom
     *
     * @param coordinates coordinates of the atom
     * @return const glm::vec3 coordinates after performing the operation
     */
    const glm::vec3 do_atom_operation(glm::vec3 coordinates) const override;

    /**
     * @brief Get the distance from the provided coordinates to the symmetry
     * element to which this operation belongs.
     *
     * @param coordinates coordinates from which to determine distance
     * @return const float distance
     */
    const float get_distance_to_element(glm::vec3 coordinates) const override;

    /**
     * @brief Get the name of a symmetry operation in plaintext
     *
     * @return const std::string
     */
    const std::string get_name() const override;

    /**
     * @brief Get the name of a symmetry operation in HTML formatting
     *
     * @return const std::string
     */
    const std::string get_name_html() const override;
};

#endif  // SYMMETRY_ELEMENTS_INVERSION_H
