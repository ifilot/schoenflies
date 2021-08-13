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

#include <memory>
#include <boost/test/unit_test.hpp>
#include <Eigen/Dense>
#include "../src/structure.h"
#include "../src/symmetry/rotor_class.h"
#include "../src/symmetry/symmetry.h"
#include "utils.h"

BOOST_AUTO_TEST_SUITE(symmetry);

BOOST_AUTO_TEST_CASE(principal_axes) {
    std::string file = resolve_path("test/files/ammonia.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);

    Eigen::Vector3d principal_moments = symmetry.get_principal_moments();
    Eigen::Vector3d expected_principal_moments{.624037, 1.31218, 1.93622};
    BOOST_TEST(principal_moments.isApprox(expected_principal_moments, 1e-6));

    Eigen::Matrix3d principal_axes = symmetry.get_principal_axes();
    Eigen::Matrix3d expected_principal_axes;
    expected_principal_axes << -.5, -.677895, -.538941,
                               .866025, -.391383, -.311158,
                               0, .622316, -.782766;
    BOOST_TEST(principal_axes.isApprox(expected_principal_axes, 1e-6));
}

BOOST_AUTO_TEST_CASE(rotor_asymmetric_top) {
    std::string file = resolve_path("test/files/thionyl-chloride.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);

    BOOST_TEST(symmetry.get_rotor_class() == RotorClass::AsymmetricTop);
}

BOOST_AUTO_TEST_CASE(rotor_symmetric_top) {
    std::string file = resolve_path("test/files/benzene.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);

    BOOST_TEST(symmetry.get_rotor_class() == RotorClass::SymmetricTop);
}

BOOST_AUTO_TEST_CASE(rotor_linear) {
    std::string file = resolve_path("test/files/carbon-dioxide.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);

    BOOST_TEST(symmetry.get_rotor_class() == RotorClass::Linear);
}

BOOST_AUTO_TEST_CASE(rotor_spherical_top) {
    std::string file = resolve_path("test/files/adamantane.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);

    BOOST_TEST(symmetry.get_rotor_class() == RotorClass::SphericalTop);
}

BOOST_AUTO_TEST_SUITE_END();
