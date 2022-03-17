/**
 * Schoenflies
 * Copyright (c) 2022 Luuk Kempen
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

#include "practice_irreps.h"

/**
 * @brief Default constructor
 */
PracticeIrreps::PracticeIrreps() {}

/**
 * @brief Construct a new Practice Irreps object
 *
 * @param symmetry symmetry object to practice
 */
PracticeIrreps::PracticeIrreps(std::shared_ptr<Symmetry> symmetry) {
    this->symmetry = symmetry;

    std::random_device device;
    this->random_engine = std::mt19937(device());

    this->create_basis_set();
    this->calculate();
}

/**
 * @brief Get the element used as basis set
 *
 * @return const Element
 */
const Element PracticeIrreps::get_basis_set_element() const {
    return PeriodicTable::get_element(this->basis_set_atomic_number);
}

/**
 * @brief Get the number of atoms in the basis set
 *
 * @return const unsigned int
 */
const unsigned int PracticeIrreps::get_basis_set_count() const {
    return this->basis_set_atoms.size();
}

/**
 * @brief Get the orbital label of the basis set
 *
 * @return const OrbitalLabel&
 */
const OrbitalLabel& PracticeIrreps::get_basis_set_orbital_label() const {
    return this->basis_set_orbital_label;
}

/**
 * @brief Get the correct character values
 *
 * @return std::vector<std::pair<OperationLabelCount, double>>&
 */
std::vector<std::pair<OperationLabelCount, double>>& PracticeIrreps::get_correct_characters() {
    return this->correct_characters;
}

/**
 * @brief Get the correct irrep values
 *
 * @return std::vector<std::pair<IrrepLabel, double>>
 */
std::vector<std::pair<IrrepLabel, double>> PracticeIrreps::get_correct_irreps() {
    return this->correct_irreps;
}

/**
 * @brief Create the basis set
 */
void PracticeIrreps::create_basis_set() {
    std::shared_ptr<Structure> structure = this->symmetry->get_structure();

    json practice_config = structure->get_library_item()->get_item_practice_config()["irreps"];
    if (!practice_config.contains("basis_sets") || practice_config["basis_sets"].size() == 0) {
        throw std::runtime_error("No basis sets defined for the structure");
    }

    json& basis_sets = practice_config["basis_sets"];

    // select random basis set
    std::uniform_int_distribution<> dist(0, basis_sets.size() - 1);
    int index = dist(this->random_engine);

    std::string element = basis_sets[index]["element"];
    std::string orbital = basis_sets[index]["orbital"];

    // select atoms with element
    this->basis_set_atomic_number = PeriodicTable::get_atomic_number(element);
    for (unsigned int i = 0; i < structure->get_num_atoms(); ++i) {
        if (structure->get_atomic_number(i) == this->basis_set_atomic_number) this->basis_set_atoms.push_back(i);
    }

    // set orbital
    this->basis_set_orbital_label = OrbitalLabel::get_orbital_label(orbital);

    if (this->basis_set_orbital_label.get_azimuthal() == 1) {  // p
        switch (this->basis_set_orbital_label.get_magnetic()) {
            case -1:  // y
                this->basis_set_orbital_vector = glm::vec3(0, 1, 0);
                break;
            case 0:  // z
                this->basis_set_orbital_vector = glm::vec3(0, 0, 1);
                break;
            case 1:  // x
                this->basis_set_orbital_vector = glm::vec3(1, 0, 0);
                break;
        }
    }
}

/**
 * @brief Calculate the correct answers to the questions
 */
void PracticeIrreps::calculate() {
    this->calculate_characters();
    this->calculate_irreps();
}

/**
 * @brief Calculate the character of each operation (how many orbitals
 * remain in place)
 */
void PracticeIrreps::calculate_characters() {
    std::vector<OperationLabelCount> operation_labels_count = this->symmetry->get_point_group().get_unique_operations();
    std::vector<Operation> operations = this->symmetry->get_operation_manager()->get_operations();

    for (auto& operation_label_count : operation_labels_count) {
        // find an operation with this label
        unsigned int operation_index;
        bool found = false;

        for (unsigned int i = 0; i < operations.size(); ++i) {
            if (operations[i].get_label().matches(operation_label_count.get_label())) {
                operation_index = i;
                found = true;
                break;
            }
        }

        if (!found) {
            throw std::runtime_error("Could not find an operation, this should not happen");
        }

        Operation& operation = operations[operation_index];

        // determine the character of this operation
        double character = 0;
        for (unsigned int index : basis_set_atoms) {
            if (operation.get_result_index(index) == index) {
                if (this->basis_set_orbital_label.get_azimuthal() == 0) {  // s
                    character++;
                } else {  // p
                    character += glm::dot(operation.do_atom_operation(this->basis_set_orbital_vector),
                                          this->basis_set_orbital_vector);
                }
            }
        }
        character *= operation_label_count.get_count();  // multiply by number of operations

        this->correct_characters.push_back(std::make_pair(operation_label_count, character));
    }
}

/**
 * @brief Calculate the irreducible representations of the structure
 */
void PracticeIrreps::calculate_irreps() {
    std::vector<int> character_coefficients;
    character_coefficients.push_back(this->basis_set_atoms.size());  // manually add identity
    for (auto character : correct_characters) {
        character_coefficients.push_back(character.second);
    }

    std::vector<IrrepLabel> irreps = this->symmetry->get_point_group().get_irreps();
    std::vector<std::vector<double>> characters = this->symmetry->get_point_group().get_characters();
    unsigned int order = this->symmetry->get_point_group().get_order();

    for (unsigned int i = 0; i < irreps.size(); ++i) {
        if (character_coefficients.size() != characters[i].size()) {
            throw std::runtime_error("Vector sizes have to be equal to calculate irreps");
        }
        double dot = 0;
        for (unsigned int j = 0; j < character_coefficients.size(); ++j) {
            dot += character_coefficients[j] * characters[i][j];
        }

        this->correct_irreps.push_back(std::make_pair(irreps[i], dot / order));
    }
}
