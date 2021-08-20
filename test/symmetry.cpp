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
#include <vector>
#include <boost/test/unit_test.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include "../src/structure.h"
#include "../src/symmetry/rotor_class.h"
#include "../src/symmetry/symmetry.h"
#include "utils.h"

BOOST_AUTO_TEST_SUITE(symmetry);

BOOST_AUTO_TEST_CASE(principal_axes) {
    std::string file = resolve_path("test/files/ammonia.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);

    glm::vec3 principal_moments = symmetry.get_principal_moments();
    glm::vec3 expected_principal_moments{1.65324, 1.65324, 2.62436};
    BOOST_TEST(glm::all(glm::epsilonEqual(principal_moments, expected_principal_moments, 1e-5f)));

    glm::mat3x3 principal_axes = symmetry.get_principal_axes();
    glm::mat3x3 expected_principal_axes{
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };
    for (unsigned int i = 0; i < 3; ++i) {
        BOOST_TEST(glm::all(glm::epsilonEqual(glm::column(principal_axes, i), glm::column(expected_principal_axes, i), 1e-6f)));
    }
}

BOOST_AUTO_TEST_CASE(rotor_asymmetric_top) {
    std::string file = resolve_path("test/files/thionyl-chloride.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);

    BOOST_TEST(symmetry.get_rotor_class() == RotorClass::AsymmetricTop);
}

BOOST_AUTO_TEST_CASE(rotor_oblate_symmetric_top) {
    std::string file = resolve_path("test/files/benzene.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);

    BOOST_TEST(symmetry.get_rotor_class() == RotorClass::OblateSymmetricTop);
}

BOOST_AUTO_TEST_CASE(rotor_prolate_symmetric_top) {
    std::string file = resolve_path("test/files/iron-pentacarbonyl.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);

    BOOST_TEST(symmetry.get_rotor_class() == RotorClass::ProlateSymmetricTop);
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

BOOST_AUTO_TEST_CASE(z_axis_benzene) {
    std::string file = resolve_path("test/files/benzene.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);

    glm::vec3 expected_z_axis{0, 0, 1};
    BOOST_TEST(glm::all(glm::epsilonEqual(symmetry.get_z_axis(), expected_z_axis, 1e-6f)));
}

BOOST_AUTO_TEST_CASE(z_axis_cyclooctatetraene) {
    std::string file = resolve_path("test/files/cyclooctatetraene.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);

    glm::vec3 expected_z_axis{0, 0, 1};
    BOOST_TEST(glm::all(glm::epsilonEqual(symmetry.get_z_axis(), expected_z_axis, 1e-6f)));
}

BOOST_AUTO_TEST_CASE(z_axis_sulfur_hexafluoride) {
    std::string file = resolve_path("test/files/sulfur-hexafluoride.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);

    // sulfur hexafluoride is a cubic structure
    BOOST_TEST(glm::all(glm::isnan(symmetry.get_z_axis())));
}

BOOST_AUTO_TEST_SUITE_END();
