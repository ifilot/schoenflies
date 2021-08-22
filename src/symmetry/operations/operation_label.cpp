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

#include "operation_label.h"

/**
 * @brief Default constructor
 */
OperationLabel::OperationLabel() {}

/**
 * @brief Construct a new Operation Label object
 *
 * @param element symmetry element
 * @param degree degree of symmetry element
 * @param multiple multiple of degree
 * @param plane plane in which the operation occurs
 * @param prime distinguish similar operations
 */
OperationLabel::OperationLabel(Element element, unsigned int degree, int multiple, Plane plane, Prime prime) {
    this->element = element;
    this->degree = degree;
    this->multiple = multiple;
    this->plane = plane;
    this->prime = prime;
}

/**
 * @brief Construct a new Operation Label object
 *
 * @param element symmetry element
 */
OperationLabel::OperationLabel(Element element) {
    this->element = element;
    this->degree = 0;
    this->multiple = 1;
    this->plane = Plane::None;
    this->prime = Prime::None;
}

/**
 * @brief Construct a new Operation Label object
 *
 * @param element symmetry element
 * @param degree degree of symmetry element
 */
OperationLabel::OperationLabel(Element element, unsigned int degree) {
    this->element = element;
    this->degree = degree;
    this->multiple = 1;
    this->plane = Plane::None;
    this->prime = Prime::None;
}

/**
 * @brief Construct a new Operation Label object
 *
 * @param element symmetry element
 * @param degree degree of symmetry element
 * @param multiple multiple of degree
 */
OperationLabel::OperationLabel(Element element, unsigned int degree, int multiple) {
    this->element = element;
    this->degree = degree;
    this->multiple = multiple;
    this->plane = Plane::None;
    this->prime = Prime::None;
}

/**
 * @brief Construct a new Operation Label object
 *
 * @param element symmetry element
 * @param degree degree of symmetry element
 * @param prime distinguish similar operations
 */
OperationLabel::OperationLabel(Element element, unsigned int degree, Prime prime) {
    this->element = element;
    this->degree = degree;
    this->multiple = 1;
    this->plane = Plane::None;
    this->prime = prime;
}

/**
 * @brief Construct a new Operation Label object
 *
 * @param element symmetry element
 * @param plane plane in which the operation occurs
 */
OperationLabel::OperationLabel(Element element, Plane plane) {
    this->element = element;
    this->degree = 0;
    this->multiple = 1;
    this->plane = plane;
    this->prime = Prime::None;
}

/**
 * @brief Construct a new Operation Label object
 *
 * @param element symmetry element
 * @param plane plane in which the operation occurs
 * @param prime distinguish similar operations
 */
OperationLabel::OperationLabel(Element element, Plane plane, Prime prime) {
    this->element = element;
    this->degree = 0;
    this->multiple = 1;
    this->plane = plane;
    this->prime = prime;
}

/**
 * @brief Set the plane of this symmetry operation
 *
 * @param plane
 */
void OperationLabel::set_plane(Plane plane) {
    this->plane = plane;
}

/**
 * @brief Set the prime of this symmetry operation
 *
 * @param prime
 */
void OperationLabel::set_prime(Prime prime) {
    this->prime = prime;
}

/**
 * @brief Get the name of this symmetry operation in plaintext
 *
 * @return const std::string
 */
const std::string OperationLabel::get_name() const {
    std::string degree = std::to_string(this->degree);
    std::string multiple = std::to_string(this->multiple);

    std::string symbol, suffix;

    switch (this->element) {
        case Element::ProperRotation:
            symbol = "C";
            suffix = " rotation";
            break;
        case Element::Inversion:
            symbol = "i";
            suffix = " inversion";
            break;
        case Element::ImproperRotation:
            symbol = "S";
            suffix = " improper rotation";
            break;
        case Element::Reflection:
            symbol = "σ";
            suffix = " reflection";
            break;
        default:
            throw std::runtime_error("Unexpected symmetry element encountered.");
    }

    std::string name = symbol;

    // degree and multiple
    if (this->element == Element::ProperRotation || this->element == Element::ImproperRotation) {
        name += (this->degree != DEGREE_INF) ? degree : "∞";
        name += (this->multiple != 1) ? "^" + multiple : "";
    }

    // plane
    if (this->element == Element::Reflection && this->plane != Plane::None) {
        switch (this->plane) {
            case Plane::Horizontal:
                name += "h";
                break;
            case Plane::Vertical:
                name += "v";
                break;
            case Plane::Dihedral:
                name += "d";
                break;
            default:
                throw std::runtime_error("Unexpected symmetry plane encountered.");
        }
    }

    // prime
    if (this->prime != Prime::None) {
        switch (this->prime) {
            case Prime::Single:
                name += "\u2032";
                break;
            case Prime::Double:
                name += "\u2033";
                break;
            default:
                throw std::runtime_error("Unexpected symmetry prime encountered.");
        }
    }

    name += suffix;

    return name;
}

/**
 * @brief Get the name of this symmetry operation in HTML formatting
 *
 * @return const std::string
 */
const std::string OperationLabel::get_name_html() const {
    std::string degree = std::to_string(this->degree);
    std::string multiple = std::to_string(this->multiple);

    std::string symbol, suffix;

    switch (this->element) {
        case Element::ProperRotation:
            symbol = "<i>C</i>";
            suffix = " rotation";
            break;
        case Element::Inversion:
            symbol = "<i>i</i>";
            suffix = " inversion";
            break;
        case Element::ImproperRotation:
            symbol = "<i>S</i>";
            suffix = " improper rotation";
            break;
        case Element::Reflection:
            symbol = "<i>σ</i>";
            suffix = " reflection";
            break;
        default:
            throw std::runtime_error("Unexpected symmetry element encountered.");
    }

    std::string name = symbol;

    // degree and multiple
    if (this->element == Element::ProperRotation || this->element == Element::ImproperRotation) {
        name += (this->degree != 0) ? "<sub>" + degree + "</sub>" : "<sub>∞</sub>";
        name += (this->multiple != 1) ? "<sup>" + multiple + "</sup>" : "";
    }

    // plane
    if (this->element == Element::Reflection && this->plane != Plane::None) {
        switch (this->plane) {
            case Plane::Horizontal:
                name += "<sub>h</sub>";
                break;
            case Plane::Vertical:
                name += "<sub>v</sub>";
                break;
            case Plane::Dihedral:
                name += "<sub>d</sub>";
                break;
            default:
                throw std::runtime_error("Unexpected symmetry plane encountered.");
        }
    }

    // prime
    if (this->prime != Prime::None) {
        switch (this->prime) {
            case Prime::Single:
                name += "\u2032";
                break;
            case Prime::Double:
                name += "\u2033";
                break;
            default:
                throw std::runtime_error("Unexpected symmetry prime encountered.");
        }
    }

    name += suffix;

    return name;
}
