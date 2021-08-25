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

#ifndef SYMMETRY_OPERATIONS_OPERATION_LABEL_H
#define SYMMETRY_OPERATIONS_OPERATION_LABEL_H

#include <stdexcept>
#include <string>

class OperationLabel {
public:
    enum class Element : unsigned char {
        ProperRotation,
        Inversion,
        ImproperRotation,
        Reflection,

        C = ProperRotation,
        I = Inversion,
        S = ImproperRotation,
        sigma = Reflection
    };

    enum class Plane : unsigned char {
        None,
        Horizontal,
        Vertical,
        Dihedral,

        h = Horizontal,
        v = Vertical,
        d = Dihedral
    };

    enum class Prime : unsigned char {
        None,
        Single,
        Double
    };

    static const unsigned int DEGREE_INF = 0;

private:
    Element element;
    unsigned int degree;
    int multiple;
    Plane plane;
    Prime prime;

public:
    /**
     * @brief Default constructor
     */
    OperationLabel();

    /**
     * @brief Construct a new Operation Label object
     *
     * @param element symmetry element
     * @param degree degree of symmetry element
     * @param multiple multiple of degree
     * @param plane plane in which the operation occurs
     * @param prime distinguish similar operations
     */
    OperationLabel(Element element, unsigned int degree, int multiple, Plane plane, Prime prime);

    /**
     * @brief Construct a new Operation Label object
     *
     * @param element symmetry element
     */
    OperationLabel(Element element);

    /**
     * @brief Construct a new Operation Label object
     *
     * @param element symmetry element
     * @param degree degree of symmetry element
     */
    OperationLabel(Element element, unsigned int degree);

    /**
     * @brief Construct a new Operation Label object
     *
     * @param element symmetry element
     * @param degree degree of symmetry element
     * @param multiple multiple of degree
     */
    OperationLabel(Element element, unsigned int degree, int multiple);

    /**
     * @brief Construct a new Operation Label object
     *
     * @param element symmetry element
     * @param degree degree of symmetry element
     * @param prime distinguish similar operations
     */
    OperationLabel(Element element, unsigned int degree, Prime prime);

    /**
     * @brief Construct a new Operation Label object
     *
     * @param element symmetry element
     * @param plane plane in which the operation occurs
     */
    OperationLabel(Element element, Plane plane);

    /**
     * @brief Construct a new Operation Label object
     *
     * @param element symmetry element
     * @param plane plane in which the operation occurs
     * @param prime distinguish similar operations
     */
    OperationLabel(Element element, Plane plane, Prime prime);

    /**
     * @brief Get the element of this symmetry operation
     *
     * @return const Element
     */
    const Element get_element() const;

    /**
     * @brief Get the plane of this symmetry operation
     *
     * @return const Plane
     */
    const Plane get_plane() const;

    /**
     * @brief Set the plane of this symmetry operation
     *
     * @param plane
     */
    void set_plane(Plane plane);

    /**
     * @brief Get the prime of this symmetry operation
     *
     * @return const Prime
     */
    const Prime get_prime() const;

    /**
     * @brief Set the prime of this symmetry operation
     *
     * @param prime
     */
    void set_prime(Prime prime);

    /**
     * @brief Get the name of this symmetry operation in plaintext
     *
     * @return const std::string
     */
    const std::string get_name() const;

    /**
     * @brief Get the name of this symmetry operation in HTML formatting
     *
     * @return const std::string
     */
    const std::string get_name_html() const;
};

#endif  // SYMMETRY_OPERATIONS_OPERATION_LABEL_H
