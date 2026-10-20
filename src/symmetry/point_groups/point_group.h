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

#ifndef SYMMETRY_POINT_GROUPS_POINT_GROUP_H
#define SYMMETRY_POINT_GROUPS_POINT_GROUP_H

#include <complex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "../irreps/irrep_label.h"
#include "../operations/operation.h"
#include "../operations/operation_label.h"
#include "../operations/operation_label_count.h"
#include "point_group_label.h"

class PointGroup {
private:
    PointGroupLabel label;

    unsigned int order;
    unsigned int num_inversions;
    std::unordered_map<unsigned int, unsigned int> num_proper_rotations;
    std::unordered_map<unsigned int, unsigned int> num_improper_rotations;
    unsigned int num_reflections;
    std::vector<OperationLabelCount> unique_operations;
    std::vector<OperationLabelCount> character_columns;
    std::vector<IrrepLabel> irreps;
    std::vector<std::vector<std::complex<double>>> characters;

public:
    /**
     * @brief Default constructor
     */
    PointGroup();

    /**
     * @brief Construct a new Point Group object
     *
     * Note that, for the number of rotations, degenerate rotations (rotations
     * with the same degree around the same axis, e.g. C_3 and C_3^2) are only
     * counted once.
     *
     * @param label point group label
     * @param order order of this point group (total number of unique symmetry operations)
     * @param num_inversions number of inversions in this point group
     * @param num_proper_rotations number of proper rotations per degree in this point group
     * @param num_improper_rotations number of improper rotations per degree in this point group
     * @param num_reflections number of reflections in this point group
     * @param unique_operations vector of labels of unique operations in this point group
     * @param irreps vector of irreducible representations in this point group
     * @param characters matrix of characters (indexed as [irrep][operation])
     */
    PointGroup(
        PointGroupLabel label,
        unsigned int order,
        unsigned int num_inversions,
        std::unordered_map<unsigned int, unsigned int> num_proper_rotations,
        std::unordered_map<unsigned int, unsigned int> num_improper_rotations,
        unsigned int num_reflections,
        std::vector<OperationLabelCount> unique_operations,
        std::vector<IrrepLabel> irreps,
        std::vector<std::vector<std::complex<double>>> characters
    );

    /**
     * @brief Construct a new Point Group object whose character table has
     * different columns than its list of unique operations
     *
     * This is needed for point groups with complex-conjugate pairs of
     * irreducible representations (E+ and E-). An operation and its inverse
     * (e.g. C4 and C4^3) have conjugate characters in these irreps, so they
     * have to be listed as separate columns, whereas the list of unique
     * operations groups them together (e.g. 2 C4).
     *
     * @param label point group label
     * @param order order of this point group (total number of unique symmetry operations)
     * @param num_inversions number of inversions in this point group
     * @param num_proper_rotations number of proper rotations per degree in this point group
     * @param num_improper_rotations number of improper rotations per degree in this point group
     * @param num_reflections number of reflections in this point group
     * @param unique_operations vector of labels of unique operations in this point group
     * @param character_columns vector of labels of the columns of the character table
     * @param irreps vector of irreducible representations in this point group
     * @param characters matrix of characters (indexed as [irrep][column])
     */
    PointGroup(
        PointGroupLabel label,
        unsigned int order,
        unsigned int num_inversions,
        std::unordered_map<unsigned int, unsigned int> num_proper_rotations,
        std::unordered_map<unsigned int, unsigned int> num_improper_rotations,
        unsigned int num_reflections,
        std::vector<OperationLabelCount> unique_operations,
        std::vector<OperationLabelCount> character_columns,
        std::vector<IrrepLabel> irreps,
        std::vector<std::vector<std::complex<double>>> characters
    );

    /**
     * @brief Compare this point group to a list of found symmetry operations
     *
     * @param operations list of found operations
     * @return const int -1 if not all symmetry operations in this point group
     * were found, or a positive number indicating the number of symmetry
     * operations found but not in the point group
     */
    const unsigned int compare_to_symmetry_operations(std::vector<Operation>& operations) const;

    /**
     * @brief Get the label of the point group
     *
     * @return const PointGroupLabel&
     */
    const PointGroupLabel& get_label() const;

    /**
     * @brief Get the order of the point group
     *
     * @return const unsigned int
     */
    const unsigned int get_order() const;

    /**
     * @brief Get the unique operations of the point group
     *
     * @return const std::vector<OperationLabelCount>&
     */
    const std::vector<OperationLabelCount>& get_unique_operations() const;

    /**
     * @brief Get the columns of the character table of the point group
     * (excluding the identity)
     *
     * For most point groups these are the unique operations, but for point
     * groups with complex-conjugate irreps an operation and its inverse are
     * listed separately.
     *
     * @return const std::vector<OperationLabelCount>&
     */
    const std::vector<OperationLabelCount>& get_character_columns() const;

    /**
     * @brief Get the irreducible representations of the point group
     *
     * @return const std::vector<IrrepLabel>&
     */
    const std::vector<IrrepLabel>& get_irreps() const;

    /**
     * @brief Get the characters matrix of the point group
     *
     * @return const std::vector<std::vector<std::complex<double>>>&
     */
    const std::vector<std::vector<std::complex<double>>>& get_characters() const;

    /**
     * @brief Check whether the character table contains complex characters,
     * i.e. whether the point group has complex-conjugate pairs of irreps
     *
     * @return true if any character has a non-zero imaginary part
     * @return false otherwise
     */
    const bool has_complex_characters() const;
};

#endif  // SYMMETRY_POINT_GROUPS_POINT_GROUP_H
