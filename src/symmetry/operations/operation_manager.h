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

#ifndef SYMMETRY_OPERATIONS_OPERATION_MANAGER_H
#define SYMMETRY_OPERATIONS_OPERATION_MANAGER_H

#include <memory>
#include <vector>
#include "../../structure.h"
#include "operation.h"
#include "operation_label.h"

class OperationManager {
private:
    unsigned int next_id = 0;

    std::shared_ptr<Structure> structure;
    std::vector<Operation> operations;

public:
    /**
     * @brief Default constructor
     */
    OperationManager();

    /**
     * @brief Construct a new Operation Manager object
     *
     * @param structure
     */
    OperationManager(std::shared_ptr<Structure> structure);

    /**
     * @brief Get the list of operations present in the structure
     *
     * @return std::vector<Operation>&
     */
    std::vector<Operation>& get_operations();

    /**
     * @brief Get the list of inversion operations present in the structure
     *
     * @return const std::vector<Operation>
     */
    const std::vector<Operation> get_inversions();

    /**
     * @brief Get the list of proper rotation operations present in the
     * structure
     *
     * @return const std::vector<Operation>
     */
    const std::vector<Operation> get_proper_rotations();

    /**
     * @brief Get the list of improper rotation operations present in the
     * structure
     *
     * @return const std::vector<Operation>
     */
    const std::vector<Operation> get_improper_rotations();

    /**
     * @brief Get the list of reflection operations present in the structure
     *
     * @return const std::vector<Operation>
     */
    const std::vector<Operation> get_reflections();

    /**
     * @brief Check whether a symmetry operation exists in the structure and
     * add it to the list of operations, if it does not already exist yet
     *
     * @param operation operation to check and add
     */
    void add_operation(Operation& operation);

private:
    /**
     * @brief Check whether a symmetry operation exists in the structure
     *
     * @param operation the symmetry operation to check
     * @return true if it exists
     * @return false if it doesn't exist
     */
    bool check_operation(Operation& operation);
};

#endif  // SYMMETRY_OPERATIONS_OPERATION_MANAGER_H
