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

#include "point_group.h"

/**
 * @brief Default constructor
 */
PointGroup::PointGroup() {}

/**
 * @brief Construct a new Point Group object
 *
 * Note that, for the number of rotations, degenerate rotations (rotations
 * with the same degree around the same axis, e.g. C_3 and C_3^2) are only
 * counted once.
 *
 * @param name name of the point group
 * @param num_inversions number of inversions in this point group
 * @param num_proper_rotations number of proper rotations per degree in this point group
 * @param num_improper_rotations number of improper rotations per degree in this point group
 * @param num_reflections number of reflections in this point group
 */
PointGroup::PointGroup(
    unsigned char identifier,
    unsigned int num_inversions,
    std::unordered_map<unsigned int, unsigned int> num_proper_rotations,
    std::unordered_map<unsigned int, unsigned int> num_improper_rotations,
    unsigned int num_reflections
) {
    this->identifier = identifier;
    this->num_inversions = num_inversions;
    this->num_proper_rotations = num_proper_rotations;
    this->num_improper_rotations = num_improper_rotations;
    this->num_reflections = num_reflections;
}

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
const unsigned int PointGroup::compare_to_symmetry_operations(
    std::vector<Inversion>& inversions,
    std::vector<ProperRotation>& proper_rotations,
    std::vector<ImproperRotation>& improper_rotations,
    std::vector<Reflection>& reflections
) const {
    // initialise num_remaining as total of all operations
    // we then subtract all operations that were found in the point group
    unsigned int num_remaining = inversions.size() + proper_rotations.size() +
                                 improper_rotations.size() + reflections.size();

    // first, check inversions and reflections, as these do not have degrees
    if (inversions.size() < this->num_inversions) return -1;
    num_remaining -= this->num_inversions;

    if (reflections.size() < this->num_reflections) return -1;
    num_remaining -= this->num_reflections;

    // then, check rotations
    for (auto& num_proper_rotation : this->num_proper_rotations) {
        unsigned int degree = num_proper_rotation.first;
        unsigned int num = num_proper_rotation.second;

        unsigned int num_found = 0;
        for (unsigned int i = 0; i < proper_rotations.size(); ++i) {
            if (proper_rotations[i].get_degree() == degree) num_found++;
        }

        if (num_found < num) return -1;
        num_remaining -= num;
    }

    for (auto& num_improper_rotation : this->num_improper_rotations) {
        unsigned int degree = num_improper_rotation.first;
        unsigned int num = num_improper_rotation.second;

        unsigned int num_found = 0;
        for (unsigned int i = 0; i < improper_rotations.size(); ++i) {
            if (improper_rotations[i].get_degree() == degree) num_found++;
        }

        if (num_found < num) return -1;
        num_remaining -= num;
    }

    return num_remaining;
}

/**
 * @brief Get the identifier of the point group
 *
 * @return const unsigned char
 */
const unsigned char PointGroup::get_identifier() const {
    return this->identifier;
}

/**
 * @brief Get the class of the point group
 *
 * Returns Ch for Cs, returns S for Ci.
 *
 * @return const Class
 */
const PointGroup::Class PointGroup::get_class() const {
    // get upper 4 bits of identifier
    return static_cast<PointGroup::Class>(this->identifier & 0xF0);
}

/**
 * @brief Get the order of the point group
 *
 * Returns 0 when the order is not defined (T, Td, Th, O, Oh, I, Ih) or
 * infinity (C∞v, D∞h). Returns 1 for Cs, returns 2 for Ci.
 *
 * @return const unsigned int
 */
const unsigned int PointGroup::get_order() const {
    // get lower 4 bits of identifier
    return this->identifier & 0x0F;
}

/**
 * @brief Get the name of the point group in plaintext
 *
 * @return const std::string
 */
const std::string PointGroup::get_name() const {
    std::string order = std::to_string(this->get_order());

    switch (this->get_class()) {
        case PointGroup::Class::C:
            return "C" + order;
        case PointGroup::Class::Ch:
            return (this->get_order() != 1) ? "C" + order + "h" : "Cs";
        case PointGroup::Class::Cv:
            return (this->get_order() != 0) ? "C" + order + "v" : "C∞v";
        case PointGroup::Class::S:
            return (this->get_order() != 2) ? "S" + order : "Ci";
        case PointGroup::Class::D:
            return "D" + order;
        case PointGroup::Class::Dh:
            return (this->get_order() != 0) ? "D" + order + "h" : "D∞h";
        case PointGroup::Class::Dd:
            return "D" + order + "d";
        case PointGroup::Class::T:
            return "T";
        case PointGroup::Class::Td:
            return "Td";
        case PointGroup::Class::Th:
            return "Th";
        case PointGroup::Class::O:
            return "O";
        case PointGroup::Class::Oh:
            return "Oh";
        case PointGroup::Class::I:
            return "I";
        case PointGroup::Class::Ih:
            return "Ih";
        default:
            throw std::runtime_error("Unexpected point group class encountered.");
    }
}

/**
 * @brief Get the name of the point group in HTML formatting
 *
 * @return const std::string
 */
const std::string PointGroup::get_name_html() const {
    std::string order = std::to_string(this->get_order());

    switch (this->get_class()) {
        case PointGroup::Class::C:
            return "<i>C</i><sub>" + order + "</sub>";
        case PointGroup::Class::Ch:
            return (this->get_order() != 1) ? "<i>C</i><sub>" + order + "h</sub>" : "<i>C</i><sub>s</sub>";
        case PointGroup::Class::Cv:
            return (this->get_order() != 0) ? "<i>C</i><sub>" + order + "v</sub>" : "<i>C</i><sub>∞v</sub>";
        case PointGroup::Class::S:
            return (this->get_order() != 2) ? "<i>S</i><sub>" + order + "</sub>" : "<i>C</i><sub>i</sub>";
        case PointGroup::Class::D:
            return "<i>D</i><sub>" + order + "</sub>";
        case PointGroup::Class::Dh:
            return (this->get_order() != 0) ? "<i>D</i><sub>" + order + "h</sub>" : "<i>D</i><sub>∞h</sub>";
        case PointGroup::Class::Dd:
            return "<i>D</i><sub>" + order + "d</sub>";
        case PointGroup::Class::T:
            return "<i>T</i>";
        case PointGroup::Class::Td:
            return "<i>T</i><sub>d</sub>";
        case PointGroup::Class::Th:
            return "<i>T</i><sub>h</sub>";
        case PointGroup::Class::O:
            return "<i>O</i>";
        case PointGroup::Class::Oh:
            return "<i>O</i><sub>h</sub>";
        case PointGroup::Class::I:
            return "<i>I</i>";
        case PointGroup::Class::Ih:
            return "<i>I</i><sub>h</sub>";
        default:
            throw std::runtime_error("Unexpected point group class encountered.");
    }
}
