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

#include "operation_manager.h"

/**
 * @brief Default constructor
 */
OperationManager::OperationManager() {}

/**
 * @brief Construct a new Operation Manager object
 *
 * @param structure
 */
OperationManager::OperationManager(std::shared_ptr<Structure> structure) {
    this->structure = structure;
}

/**
 * @brief Get the list of operations present in the structure
 *
 * @return std::vector<Operation>&
 */
std::vector<Operation>& OperationManager::get_operations() {
    return this->operations;
}

/**
 * @brief Get the list of inversion operations present in the structure
 *
 * @return const std::vector<Operation>
 */
const std::vector<Operation> OperationManager::get_inversions() {
    std::vector<Operation> inversions;
    for (Operation& operation : operations) {
        if (operation.get_label().get_element() == OperationLabel::Element::Inversion)
            inversions.push_back(operation);
    }
    return inversions;
}

/**
 * @brief Get the list of proper rotation operations present in the
 * structure
 *
 * @return const std::vector<Operation>
 */
const std::vector<Operation> OperationManager::get_proper_rotations() {
    std::vector<Operation> proper_rotations;
    for (Operation& operation : operations) {
        if (operation.get_label().get_element() == OperationLabel::Element::ProperRotation)
            proper_rotations.push_back(operation);
    }
    return proper_rotations;
}

/**
 * @brief Get the list of improper rotation operations present in the
 * structure
 *
 * @return const std::vector<Operation>
 */
const std::vector<Operation> OperationManager::get_improper_rotations() {
    std::vector<Operation> improper_rotations;
    for (Operation& operation : operations) {
        if (operation.get_label().get_element() == OperationLabel::Element::ImproperRotation)
            improper_rotations.push_back(operation);
    }
    return improper_rotations;
}

/**
 * @brief Get the list of reflection operations present in the structure
 *
 * @return const std::vector<Operation>
 */
const std::vector<Operation> OperationManager::get_reflections() {
    std::vector<Operation> reflections;
    for (Operation& operation : operations) {
        if (operation.get_label().get_element() == OperationLabel::Element::Reflection)
            reflections.push_back(operation);
    }
    return reflections;
}

/**
 * @brief Check whether a symmetry operation exists in the structure and
 * add it to the list of operations, if it does not already exist yet
 *
 * @param operation operation to check and add
 */
void OperationManager::add_operation(Operation& operation) {
    if (this->check_operation(operation)) {
        bool found = false;

        operation.set_id(this->next_id++);

        for (unsigned int i = 0; i < this->operations.size(); ++i) {
            if (operation == this->operations[i]) {
                if (operation.get_error() < this->operations[i].get_error()) {
                    this->operations[i] = operation;
                }

                found = true;
                break;
            }
        }

        if (!found) this->operations.push_back(operation);
    }
}

/**
 * @brief Check whether a symmetry operation exists in the structure
 *
 * @param operation the symmetry operation to check
 * @return true if it exists
 * @return false if it doesn't exist
 */
bool OperationManager::check_operation(Operation& operation) {
    operation.do_operation(this->structure);

    // TODO move tolerance to a variable/constant
    return operation.get_error() < .1;
}
