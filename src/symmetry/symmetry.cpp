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

#include "symmetry.h"

/**
 * @brief Default constructor
 */
Symmetry::Symmetry() {}

/**
 * @brief Construct a new Symmetry object
 *
 * @param structure
 */
Symmetry::Symmetry(std::shared_ptr<Structure> structure) {
    this->structure = structure;

    this->determine_principal_axes();
    this->determine_rotor_class();
    this->find_symmetry_operations();
}

/**
 * @brief Calculate the inertial tensor and use this to determine principal
 * axes and their principal moments.
 */
void Symmetry::determine_principal_axes() {
    // calculate inertial tensor
    double Ixx = 0, Iyy = 0, Izz = 0, Ixy = 0, Ixz = 0, Iyz = 0;

    for (unsigned int i = 0; i < this->structure->get_num_atoms(); ++i) {
        float mass = PeriodicTable::get_element(this->structure->get_atomic_number(i)).mass;
        glm::vec3 r = this->structure->get_coordinates(i);

        Ixx += mass * (r.y * r.y + r.z * r.z);
        Iyy += mass * (r.x * r.x + r.z * r.z);
        Izz += mass * (r.x * r.x + r.y * r.y);

        Ixy -= mass * r.x * r.y;
        Ixz -= mass * r.x * r.z;
        Iyz -= mass * r.y * r.z;
    }

    // symmetrix matrix with only real entries <=> self-adjoint matrix
    Eigen::Matrix3d I;
    I << Ixx, Ixy, Ixz,
         Ixy, Iyy, Iyz,
         Ixz, Iyz, Izz;

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(I);

    if (solver.info() != Eigen::ComputationInfo::Success) {
        throw std::runtime_error("Diagonalisation of inertial tensor did not converge.");
    }

    this->principal_moments = solver.eigenvalues();
    this->principal_axes = solver.eigenvectors();
}

/**
 * @brief Determine the rotor class of the structure (given by the
 * degeneracy of the inertial moments).
 */
void Symmetry::determine_rotor_class() {
    // TODO move tolerance to a variable/constant

    // we use the fact that the eigenvalues are sorted in increasing order
    // (from Eigen docs)
    if ((this->principal_moments[2] - this->principal_moments[0]) / this->principal_moments[2] < .02) {
        // all principal moments are (approximately) degenerate
        this->rotor_class = RotorClass::SphericalTop;
    } else if ((this->principal_moments[1] - this->principal_moments[0]) / this->principal_moments[1] < .02 ||
               (this->principal_moments[2] - this->principal_moments[1]) / this->principal_moments[2] < .02) {
        // two principal moments are (approximately) degenerate
        if (this->principal_moments[0] < .02) {
            // the lowest principal moment is (approximately) zero
            this->rotor_class = RotorClass::Linear;
        } else {
            this->rotor_class = RotorClass::SymmetricTop;
        }
    } else {
        // all principal moments are unique
        this->rotor_class = RotorClass::AsymmetricTop;
    }
}

/**
 * @brief Find all symmetry operations of the structure.
 */
void Symmetry::find_symmetry_operations() {
    this->find_inversion_centre();
}

/**
 * @brief Find an inversion centre in the structure.
 */
void Symmetry::find_inversion_centre() {
    Inversion inversion;
    inversion.do_operation(this->structure);

    // TODO move tolerance to a variable/constant
    if (inversion.get_error() < .1) {
        // TODO add the operation to a list of operations
        std::cout << "Inversion centre found in the structure." << std::endl;
    }
}

/**
 * @brief Get the principal moments of the structure
 *
 * @return const Eigen::Vector3d&
 */
const Eigen::Vector3d& Symmetry::get_principal_moments() const {
    return this->principal_moments;
}

/**
 * @brief Get the principal axes of the structure
 *
 * @return const Eigen::Matrix3d&
 */
const Eigen::Matrix3d& Symmetry::get_principal_axes() const {
    return this->principal_axes;
}

/**
 * @brief Get the rotor class of the structure
 *
 * @return const RotorClass
 */
const RotorClass Symmetry::get_rotor_class() const {
    return this->rotor_class;
}
