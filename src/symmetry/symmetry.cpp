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
    this->find_point_group();
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

    auto& principal_moments = solver.eigenvalues();
    auto& principal_axes = solver.eigenvectors();

    // convert Eigen objects to GLM objects
    for (unsigned int i = 0; i < 3; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            this->principal_axes[i][j] = principal_axes(j, i);
        }

        this->principal_moments[i] = principal_moments(i);
    }
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
    } else if ((this->principal_moments[1] - this->principal_moments[0]) / this->principal_moments[1] < .02) {
        // the two lower principal moments are (approximately) degenerate
        this->rotor_class = RotorClass::OblateSymmetricTop;
    } else if ((this->principal_moments[2] - this->principal_moments[1]) / this->principal_moments[2] < .02) {
        // the two higher principal moments are (approximately) degenerate
        if (this->principal_moments[0] < .02) {
            // the lowest principal moment is (approximately) zero
            this->rotor_class = RotorClass::Linear;
        } else {
            this->rotor_class = RotorClass::ProlateSymmetricTop;
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
    this->find_proper_rotational_axes();
    this->find_improper_rotational_axes();
    this->find_reflection_planes();
}

/**
 * @brief Find an inversion centre in the structure.
 */
void Symmetry::find_inversion_centre() {
    Inversion inversion;
    this->check_and_add_operation(inversion, this->inversions);
}

/**
 * @brief Find proper rotational axes in the structure.
 */
void Symmetry::find_proper_rotational_axes() {
    if (this->get_rotor_class() == RotorClass::Linear) {
        // only C∞ for linear structures
        glm::vec3 axis = glm::column(this->get_principal_axes(), 0);

        ProperRotation rotation(ProperRotation::DEGREE_INF, axis);
        this->check_and_add_operation(rotation, this->proper_rotations);
    } else {
        this->find_proper_rotational_axes_along_principal_axes();
        this->find_proper_rotational_axes_through_atoms();
        this->find_proper_rotational_axes_between_atoms();

        if (this->get_rotor_class() == RotorClass::SphericalTop) {
            this->find_proper_rotational_axes_polygonal_faces();
        }
    }
}

/**
 * @brief Find proper rotational axes along the principal axes of the
 * structure.
 */
void Symmetry::find_proper_rotational_axes_along_principal_axes() {
    for (unsigned int i = 0; i < 3; ++i) {
        glm::vec3 axis = glm::column(this->get_principal_axes(), i);

        // TODO move maximum degree to a constant
        for (unsigned int degree = 2; degree <= 8; ++degree) {
            ProperRotation rotation(degree, axis);
            this->check_and_add_operation(rotation, this->proper_rotations);
        }
    }
}

/**
 * @brief Find proper rotational axes through the centre of mass and an
 * atom of the structure.
 */
void Symmetry::find_proper_rotational_axes_through_atoms() {
    for (unsigned int i = 0; i < this->structure->get_num_atoms(); ++i) {
        glm::vec3 axis = this->structure->get_coordinates(i);

        if (glm::length2(axis) == 0) continue;
        if (!this->axis_inertially_allowed(axis)) continue;

        // TODO move maximum degree to a constant
        for (unsigned int degree = 2; degree <= 8; ++degree) {
            ProperRotation rotation(degree, axis);
            this->check_and_add_operation(rotation, this->proper_rotations);
        }
    }
}

/**
 * @brief Find proper rotational axes through the centre of mass and the
 * midpoints between pairs of atoms of the same element.
 */
void Symmetry::find_proper_rotational_axes_between_atoms() {
    for (unsigned int i = 0; i < this->structure->get_num_atoms() - 1; ++i) {
        for (unsigned int j = i + 1; j < this->structure->get_num_atoms(); ++j) {
            if (this->structure->get_atomic_number(i) != this->structure->get_atomic_number(j)) continue;

            // calculate midpoint between atoms i and j
            glm::vec3 axis = .5f * (this->structure->get_coordinates(i) + this->structure->get_coordinates(j));

            if (glm::length2(axis) == 0) continue;
            if (!this->axis_inertially_allowed(axis)) continue;

            // we know that this axis lies along a midpoint of atoms,
            // therefore it must have an even degree
            // TODO move maximum degree to a constant
            for (unsigned int degree = 2; degree <= 8; degree += 2) {
                ProperRotation rotation(degree, axis);
                this->check_and_add_operation(rotation, this->proper_rotations);
            }
        }
    }
}

/**
 * @brief Find proper rotational axes through polygonal faces for
 * structures classified as spherical top (cubic).
 */
void Symmetry::find_proper_rotational_axes_polygonal_faces() {
    // the number of C2 rotations determines how we find the remaining axes
    std::vector<ProperRotation> C2s;

    for (unsigned int i = 0; i < this->proper_rotations.size(); ++i) {
        if (this->proper_rotations[i].get_degree() == 2) C2s.push_back(this->proper_rotations[i]);
    }

    switch (C2s.size()) {
        case 3:
        case 9:
            this->find_proper_rotational_axes_polygonal_faces_T_O();
            break;
        case 15:
            this->find_proper_rotational_axes_polygonal_faces_I(C2s);
            break;
    }
}

/**
 * @brief Find proper rotational axes through polygonal faces for
 * structures with tetrahedral symmetry (T point group, 3 C2 rotations) and
 * octahedral symmetry (O point group, 9 C2 rotations).
 */
void Symmetry::find_proper_rotational_axes_polygonal_faces_T_O() {
    // the remaining C3 rotations are formed by combining the three principal axes
    // we only need to enumerate half of the combinations; the other half are
    // the same axes but in opposite directions
    for (int i = -1; i < 2; i += 2) {  // {-1, 1}
        for (int j = -1; j < 2; j += 2) {  // {-1, 1}
            glm::vec3 axis = glm::column(this->get_principal_axes(), 0) * (float) i +
                             glm::column(this->get_principal_axes(), 1) * (float) j +
                             glm::column(this->get_principal_axes(), 2);

            ProperRotation rotation(3, axis);
            this->check_and_add_operation(rotation, this->proper_rotations);
        }
    }
}

/**
 * @brief Find proper rotational axes through polygonal faces for
 * structures with icosahedral symmetry (I point group, 15 C2 rotations).
 *
 * @param C2s C2 rotations present in the structure
 */
void Symmetry::find_proper_rotational_axes_polygonal_faces_I(std::vector<ProperRotation> C2s) {
    // the remaining C3 and C5 rotations are orthogonal to pairs of C2 rotations
    // so we take cross products of pairs of C2 rotations and test for C3 and C5 symmetry
    for (unsigned int i = 0; i < C2s.size() - 1; ++i) {
        for (unsigned int j = i + 1; j < C2s.size(); ++j) {
            glm::vec3 axis = glm::cross(C2s[i].get_axis(), C2s[j].get_axis());

            if (glm::length2(axis) == 0) continue;

            for (unsigned int degree = 3; degree <= 5; degree += 2) {  // {3, 5}
                ProperRotation rotation(degree, axis);
                this->check_and_add_operation(rotation, this->proper_rotations);
            }
        }
    }
}

/**
 * @brief Find improper rotational axes in the structure.
 */
void Symmetry::find_improper_rotational_axes() {
    if (this->get_rotor_class() == RotorClass::Linear) {
        // only S∞ for linear structures
        glm::vec3 axis = glm::column(this->get_principal_axes(), 0);

        ImproperRotation imp_rotation(ImproperRotation::DEGREE_INF, axis);
        this->check_and_add_operation(imp_rotation, this->improper_rotations);
    } else {
        // improper rotational axes are coincident with proper rotational axes, and
        // have degree equal to either n or 2n
        for (unsigned int i = 0; i < this->proper_rotations.size(); ++i) {
            for (unsigned int degree_factor = 1; degree_factor <= 2; ++degree_factor) {  // {1, 2}
                unsigned int degree = this->proper_rotations[i].get_degree() * degree_factor;
                if (degree <= 2) continue;  // S1 = σ, S2 = i

                ImproperRotation imp_rotation(degree, this->proper_rotations[i].get_axis());
                this->check_and_add_operation(imp_rotation, this->improper_rotations);
            }
        }
    }
}

/**
 * @brief Find reflection planes in the structure.
 */
void Symmetry::find_reflection_planes() {
    // infinite reflection planes (∞ σv) in C∞v and D∞d are not tracked here
    if (this->get_rotor_class() == RotorClass::Linear) return;

    // if octahedral or icosahedral symmetry, we can limit the reflection plane
    // search to only include planes with normals coinciding with C2 rotational
    // axes
    bool octahedral_or_icosahedral = false;

    if (this->get_rotor_class() == RotorClass::SphericalTop) {
        unsigned int num_C2s = 0;
        for (unsigned int i = 0; i < this->proper_rotations.size(); ++i) {
            if (this->proper_rotations[i].get_degree() == 2) num_C2s++;
        }

        if (num_C2s == 9 || num_C2s == 15) octahedral_or_icosahedral = true;
    }

    if (!octahedral_or_icosahedral) this->find_reflection_planes_normal_to_principal_axes();
    this->find_reflection_planes_normal_to_proper_rotational_axes(octahedral_or_icosahedral);
    if (!octahedral_or_icosahedral) this->find_reflection_planes_in_midpoints();
}

/**
 * @brief Find reflection planes which are normal to the principal axes of
 * the structure.
 */
void Symmetry::find_reflection_planes_normal_to_principal_axes() {
    for (unsigned int i = 0; i < 3; ++i) {
        glm::vec3 normal = glm::column(this->get_principal_axes(), i);
        Reflection reflection(normal);
        this->check_and_add_operation(reflection, this->reflections);
    }
}

/**
 * @brief Find reflection planes which are normal to proper rotational
 * axes.
 *
 * @param only_C2s whether to only search normal to C2 rotational axes (in
 * case of octahedral and icosahedral symmetry)
 */
void Symmetry::find_reflection_planes_normal_to_proper_rotational_axes(bool only_C2s) {
    for (unsigned int i = 0; i < this->proper_rotations.size(); ++i) {
        ProperRotation& rotation = this->proper_rotations[i];
        if (only_C2s && rotation.get_degree() != 2) continue;

        Reflection reflection(rotation.get_axis());
        this->check_and_add_operation(reflection, this->reflections);
    }
}

/**
 * @brief Find reflection planes which pass through midpoints between pairs
 * of atoms of the same element.
 */
void Symmetry::find_reflection_planes_in_midpoints() {
    for (unsigned int i = 0; i < this->structure->get_num_atoms() - 1; ++i) {
        for (unsigned int j = i + 1; j < this->structure->get_num_atoms(); ++j) {
            if (this->structure->get_atomic_number(i) != this->structure->get_atomic_number(j)) continue;

            // calculate midpoint between atoms i and j
            glm::vec3 midpoint = .5f * (this->structure->get_coordinates(i) + this->structure->get_coordinates(j));

            // calculate normal for reflection plane, which is in the plane
            // formed by i and j and perpendicular to the midpoint vector
            glm::vec3 axis = glm::cross(this->structure->get_coordinates(i), this->structure->get_coordinates(j));
            glm::vec3 normal = glm::cross(midpoint, axis);

            if (glm::length2(normal) == 0) continue;
            if (!this->axis_inertially_allowed(normal)) continue;

            Reflection reflection(normal);
            this->check_and_add_operation(reflection, this->reflections);
        }
    }
}

/**
 * @brief Find the point group with the highest match to the found symmetry
 * operations.
 */
void Symmetry::find_point_group() {
    std::vector<PointGroup> point_groups = PointGroups::point_groups;

    int min_diff = INT_MAX;
    unsigned int best_group_index;

    for (unsigned int i = 0; i < point_groups.size(); ++i) {
        PointGroup& point_group = point_groups[i];
        int diff = point_group.compare_to_symmetry_operations(this->inversions, this->proper_rotations, this->improper_rotations, this->reflections);

        if (diff >= 0 && diff < min_diff) {
            min_diff = diff;
            best_group_index = i;
        }
    }

    this->point_group = point_groups[best_group_index];
}

/**
 * @brief Check whether an axis can be a symmetry axis based on the
 * inertial tensor.
 *
 * A symmetry operation must leave a molecule unchanged, which sets
 * restrictions on whether an axis or plane can be a symmetry axis or plane
 * based on the principal axes. Checking this early leads to increased
 * efficiency.
 *
 * @param axis the axis to check
 * @return true if axis can be a symmetry axis
 * @return false if axis cannot be a symmetry axis
 */
bool Symmetry::axis_inertially_allowed(glm::vec3& axis) {
    RotorClass rotor_class = this->get_rotor_class();

    // if the structure is a spherical top, every axis is inertially allowed
    if (rotor_class == RotorClass::SphericalTop) return true;

    // if the structure is a symmetric top, the axis must either contain the
    // nondegenerate principal axis or be orthogonal to it
    if (rotor_class == RotorClass::OblateSymmetricTop ||
        rotor_class == RotorClass::ProlateSymmetricTop ||
        rotor_class == RotorClass::Linear) {
        glm::vec3 nondegenerate_axis;
        if (rotor_class == RotorClass::OblateSymmetricTop) {
            nondegenerate_axis = glm::column(this->get_principal_axes(), 2);
        } else if (rotor_class == RotorClass::ProlateSymmetricTop || rotor_class == RotorClass::Linear) {
            nondegenerate_axis = glm::column(this->get_principal_axes(), 0);
        }

        float dot = glm::dot(nondegenerate_axis, axis);

        // TODO move tolerance to a variable/constant
        return dot < .02 || dot > 1 - .02;
    }

    // if the structure is an asymmetric top, the axis must contain at least one
    // of the principal axes
    if (rotor_class == RotorClass::AsymmetricTop) {
        float min_dot = INFINITY;

        for (unsigned int i = 0; i < 3; ++i) {
            glm::vec3 principal_axis = glm::column(this->get_principal_axes(), i);
            float dot = glm::dot(principal_axis, axis);

            if (dot < min_dot) min_dot = dot;
        }

        // TODO move tolerance to a variable/constant
        return min_dot < .02;
    }

    return false;
}

/**
 * @brief Check whether a symmetry operation exists in the structure.
 *
 * @param operation the symmetry operation to check
 * @return true if it exists
 * @return false if it doesn't exist
 */
bool Symmetry::check_operation(Operation& operation) {
    operation.do_operation(this->structure);

    // TODO move tolerance to a variable/constant
    return operation.get_error() < .1;
}

/**
 * @brief Add an operation to the list of operations, if it does not
 * already exist yet.
 *
 * @tparam T class derived from Operation
 * @param operation operation to add
 * @param operations vector of operations to add to
 */
template <class T>
void Symmetry::add_operation(T& operation, std::vector<T>& operations) {
    bool found = false;

    for (unsigned int i = 0; i < operations.size(); ++i) {
        if (operation.equals(operations[i])) {
            if (operation.get_error() < operations[i].get_error()) {
                operations[i] = operation;
            }

            found = true;
            break;
        }
    }

    if (!found) operations.push_back(operation);
}

/**
 * @brief Check whether a symmetry operation exists in the structure and
 * add it to the list of operations, if it does not already exist yet.
 *
 * @tparam T class derived from Operation
 * @param operation operation to check and add
 * @param operations vector of operations to add to
 */
template <class T>
void Symmetry::check_and_add_operation(T& operation, std::vector<T>& operations) {
    if (this->check_operation(operation)) this->add_operation(operation, operations);
}

/**
 * @brief Get the principal moments of the structure
 *
 * @return const glm::vec3&
 */
const glm::vec3& Symmetry::get_principal_moments() const {
    return this->principal_moments;
}

/**
 * @brief Get the principal axes of the structure
 *
 * @return const glm::mat3x3&
 */
const glm::mat3x3& Symmetry::get_principal_axes() const {
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

/**
 * @brief Get the list of inversion operations present in the structure
 *
 * @return const std::vector<Inversion>&
 */
const std::vector<Inversion>& Symmetry::get_inversions() const {
    return this->inversions;
}

/**
 * @brief Get the list of proper rotation operations present in the
 * structure
 *
 * @return const std::vector<ProperRotation>&
 */
const std::vector<ProperRotation>& Symmetry::get_proper_rotations() const {
    return this->proper_rotations;
}

/**
 * @brief Get the list of improper rotation operations present in the
 * structure
 *
 * @return const std::vector<ImproperRotation>&
 */
const std::vector<ImproperRotation>& Symmetry::get_improper_rotations() const {
    return this->improper_rotations;
}

/**
 * @brief Get the list of reflection operations present in the structure
 *
 * @return const std::vector<Reflection>&
 */
const std::vector<Reflection>& Symmetry::get_reflections() const {
    return this->reflections;
}

/**
 * @brief Get the point group of the structure
 *
 * @return const PointGroup&
 */
const PointGroup& Symmetry::get_point_group() const {
    return this->point_group;
}
