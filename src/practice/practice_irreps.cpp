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
 * @param practice_structure practice structure object
 */
PracticeIrreps::PracticeIrreps(std::shared_ptr<PracticeStructure> practice_structure) {
    this->practice_structure = practice_structure;
    this->calculate();
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
 * @brief Calculate the correct answers to the questions
 */
void PracticeIrreps::calculate() {
    this->calculate_characters();
    this->calculate_irreps();
}

/**
 * @brief Calculate the character of each column of the character table (how
 * many orbitals remain in place)
 */
void PracticeIrreps::calculate_characters() {
    std::shared_ptr<Symmetry> symmetry = this->practice_structure->get_symmetry();
    std::shared_ptr<BasisSet> basis_set = this->practice_structure->get_basis_set();

    std::vector<OperationLabelCount> character_columns = symmetry->get_point_group().get_character_columns();
    std::unordered_map<unsigned int, Operation> operations = symmetry->get_operation_manager()->get_point_group_operations();

    for (auto& character_column : character_columns) {
        // find an operation with this label and power; the basis set spans a
        // real representation, so an operation and its inverse (and thus any
        // operation with this label) have the same character
        Operation* operation = nullptr;
        for (auto& point_group_operation : operations) {
            OperationLabel& label = point_group_operation.second.get_label();
            if (!label.matches_power(character_column.get_label())) continue;

            operation = &point_group_operation.second;
            // prefer the operation as it was found, for reproducible rounding
            if (label.get_multiple() == character_column.get_label().get_multiple()) break;
        }

        if (operation == nullptr) {
            throw std::runtime_error("Could not find an operation, this should not happen");
        }

        // matrix including the power of the operation (e.g. C5^2 instead of C5)
        glm::mat3x3 matrix = operation->calculate_fractional_matrix(1);

        // determine the character of this operation
        double character = 0;
        for (unsigned int index : basis_set->get_atoms()) {
            if (operation->get_result_index(index) == index) {
                if (basis_set->get_orbital_label().get_azimuthal() == 0) {  // s
                    character++;
                } else {  // p
                    character += glm::dot(matrix * basis_set->get_orbital_vector(),
                                          basis_set->get_orbital_vector());
                }
            }
        }
        character *= character_column.get_count();  // multiply by number of operations

        this->correct_characters.push_back(std::make_pair(character_column, character));
    }
}

/**
 * @brief Calculate the irreducible representations of the structure
 *
 * This applies the little orthogonality theorem, using the complex conjugate
 * of the characters of the irreducible representations. This matters for
 * complex-conjugate pairs of irreps (E+ and E-), e.g. in C_n, C_nh and S_n.
 */
void PracticeIrreps::calculate_irreps() {
    std::shared_ptr<Symmetry> symmetry = this->practice_structure->get_symmetry();
    std::shared_ptr<BasisSet> basis_set = this->practice_structure->get_basis_set();

    std::vector<double> character_coefficients;
    character_coefficients.push_back(basis_set->get_atoms().size());  // manually add identity
    for (auto character : correct_characters) {
        character_coefficients.push_back(character.second);
    }

    std::vector<IrrepLabel> irreps = symmetry->get_point_group().get_irreps();
    std::vector<std::vector<std::complex<double>>> characters = symmetry->get_point_group().get_characters();
    unsigned int order = symmetry->get_point_group().get_order();

    for (unsigned int i = 0; i < irreps.size(); ++i) {
        if (character_coefficients.size() != characters[i].size()) {
            throw std::runtime_error("Vector sizes have to be equal to calculate irreps");
        }
        std::complex<double> dot = 0;
        for (unsigned int j = 0; j < character_coefficients.size(); ++j) {
            dot += character_coefficients[j] * std::conj(characters[i][j]);
        }

        // multiplicities are integers, so remove numerical noise (and negative zero)
        double multiplicity = std::round(dot.real() / order) + 0.0;
        this->correct_irreps.push_back(std::make_pair(irreps[i], multiplicity));
    }
}
