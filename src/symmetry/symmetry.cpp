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
Symmetry::Symmetry(Structure* structure) {
    this->structure = structure;

    this->determine_principal_axes();
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
