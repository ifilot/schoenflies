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

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "../operations/improper_rotation.h"
#include "../operations/inversion.h"
#include "../operations/proper_rotation.h"
#include "../operations/reflection.h"

class PointGroup {
public:
    /**
     * @brief Classes of point groups; combine with an order in the four lower
     * bits (0-15) to form a point group.
     */
    enum Class: unsigned char {
        // classes with order
        C  = 0 << 4,   // cyclic groups
        Ch = 1 << 4,   // reflection groups
        Cv = 2 << 4,   // pyramidal groups
        S  = 3 << 4,   // improper rotation groups
        D  = 4 << 4,   // dihedral groups
        Dh = 5 << 4,   // prismatic groups
        Dd = 6 << 4,   // antiprismatic groups

        // classes without order
        T  = 7 << 4,   // chiral tetrahedral symmetry
        Td = 8 << 4,   // achiral tetrahedral symmetry
        Th = 9 << 4,   // pyritohedral symmetry
        O  = 10 << 4,  // chiral octahedral symmetry
        Oh = 11 << 4,  // achiral octahedral symmetry
        I  = 12 << 4,  // chiral icosahedral symmetry
        Ih = 13 << 4,  // achiral icosahedral symmetry

        // special cases; use these without an order!
        // note: these cases will not be returned with get_class()
        Cs = Ch | 1,   // Cs = C1h
        Ci = S | 2,    // Ci = S2
        Cinfv = Cv,    // linear; we use an order of 0 to indicate infinity
        Dinfh = Dh     // linear
    };

private:
    unsigned char identifier;

    unsigned int num_inversions;
    std::unordered_map<unsigned int, unsigned int> num_proper_rotations;
    std::unordered_map<unsigned int, unsigned int> num_improper_rotations;
    unsigned int num_reflections;

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
     * @param identifier point group identifier
     * @param num_inversions number of inversions in this point group
     * @param num_proper_rotations number of proper rotations per degree in this point group
     * @param num_improper_rotations number of improper rotations per degree in this point group
     * @param num_reflections number of reflections in this point group
     */
    PointGroup(
        unsigned char identifier,
        unsigned int num_inversions,
        std::unordered_map<unsigned int, unsigned int> num_proper_rotations,
        std::unordered_map<unsigned int, unsigned int> num_improper_rotations,
        unsigned int num_reflections
    );

    /**
     * @brief Compare this point group to a list of found symmetry operations
     *
     * @param inversions list of found inversions
     * @param proper_rotations list of found proper rotations
     * @param improper_rotations list of found improper rotations
     * @param reflections list of found reflections
     * @return const int -1 if not all symmetry operations in this point group
     * were found, or a positive number indicating the number of symmetry
     * operations found but not in the point group
     */
    const unsigned int compare_to_symmetry_operations(
        std::vector<Inversion>& inversions,
        std::vector<ProperRotation>& proper_rotations,
        std::vector<ImproperRotation>& improper_rotations,
        std::vector<Reflection>& reflections
    ) const;

    /**
     * @brief Get the identifier of the point group
     *
     * @return const unsigned char
     */
    const unsigned char get_identifier() const;

    /**
     * @brief Get the class of the point group
     *
     * Returns Ch for Cs, returns S for Ci.
     *
     * @return const Class
     */
    const Class get_class() const;

    /**
     * @brief Get the order of the point group
     *
     * Returns 0 when the order is not defined (T, Td, Th, O, Oh, I, Ih) or
     * infinity (C∞v, D∞h). Returns 1 for Cs, returns 2 for Ci.
     *
     * @return const unsigned int
     */
    const unsigned int get_order() const;

    /**
     * @brief Get the name of the point group in plaintext
     *
     * @return const std::string
     */
    const std::string get_name() const;

    /**
     * @brief Get the name of the point group in HTML formatting
     *
     * @return const std::string
     */
    const std::string get_name_html() const;
};

#endif  // SYMMETRY_POINT_GROUPS_POINT_GROUP_H
