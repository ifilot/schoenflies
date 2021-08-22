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

#ifndef SYMMETRY_OPERATIONS_OPERATION_H
#define SYMMETRY_OPERATIONS_OPERATION_H

#include <algorithm>
#include <cmath>
#include <math.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <glm/glm.hpp>
#include "../../structure.h"
#include "operation_label.h"

class Operation {
protected:
    OperationLabel label;

    float error = NAN;

public:
    /**
     * @brief Default constructor
     */
    Operation();

    /**
     * @brief Get the label of this symmetry operation
     *
     * @return OperationLabel&
     */
    OperationLabel& get_label();

    /**
     * @brief Set the label of this symmetry operation
     *
     * @param label
     */
    void set_label(OperationLabel label);

    /**
     * @brief Get the error of this symmetry operation
     *
     * @return const float
     */
    const float get_error() const;

    /**
     * @brief Perform the symmetry operation and set the error value
     *
     * @param structure structure on which to perform the operation
     */
    void do_operation(std::shared_ptr<Structure> structure);

    /**
     * @brief Perform the symmetry operation on a single atom
     *
     * @param coordinates coordinates of the atom
     * @return const glm::vec3 coordinates after performing the operation
     */
    virtual const glm::vec3 do_atom_operation(glm::vec3 coordinates) const = 0;

    /**
     * @brief Get the distance from the provided coordinates to the symmetry
     * element to which this operation belongs.
     *
     * @param coordinates coordinates from which to determine distance
     * @return const float distance
     */
    virtual const float get_distance_to_element(glm::vec3 coordinates) const = 0;
};

#endif  // SYMMETRY_OPERATIONS_OPERATION_H
