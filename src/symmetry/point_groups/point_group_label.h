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

#ifndef SYMMETRY_POINT_GROUPS_POINT_GROUP_LABEL_H
#define SYMMETRY_POINT_GROUPS_POINT_GROUP_LABEL_H

#include <string>
#include <stdexcept>

class PointGroupLabel {
public:
    enum class Class : unsigned char {
        // classes with order
        C,      // cyclic groups
        Ch,     // reflection groups
        Cv,     // pyramidal groups
        S,      // improper rotation groups
        D,      // dihedral groups
        Dh,     // prismatic groups
        Dd,     // antiprismatic groups

        // classes without order
        T,      // chiral tetrahedral symmetry
        Td,     // achiral tetrahedral symmetry
        Th,     // pyritohedral symmetry
        O,      // chiral octahedral symmetry
        Oh,     // achiral octahedral symmetry
        I,      // chiral icosahedral symmetry
        Ih,     // achiral icosahedral symmetry

        // special cases
        Cs,     // Cs = C1h
        Ci,     // Ci = S2
        Cinfv,  // linear
        Dinfh   // linear
    };

private:
    Class point_group_class;
    unsigned int order;

public:
    /**
     * @brief Default constructor
     */
    PointGroupLabel();

    /**
     * @brief Construct a new Point Group Label object
     *
     * @param point_group_class class of the point group
     * @param order order of the point group
     */
    PointGroupLabel(Class point_group_class, unsigned int order);

    /**
     * @brief Construct a new Point Group Label object
     *
     * @param point_group_class class of the point group
     */
    PointGroupLabel(Class point_group_class);

    /**
     * @brief Get the class of the point group
     *
     * @return const Class
     */
    const Class get_class() const;

    /**
     * @brief Get the order of the point group
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

#endif  // SYMMETRY_POINT_GROUPS_POINT_GROUP_LABEL_H
