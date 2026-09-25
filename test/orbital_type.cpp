/**
 * Schoenflies
 * Copyright (c) 2026 Ivo Filot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <boost/test/unit_test.hpp>
#include <glm/glm.hpp>
#include "../src/orbitals/orbital_type.h"

BOOST_AUTO_TEST_SUITE(orbital_type);

BOOST_AUTO_TEST_CASE(provides_complete_lowest_shell_set) {
    BOOST_TEST(orbital_types().size() == 16);
    BOOST_TEST(orbital_type_name(OrbitalType::S) == "1s");
    BOOST_TEST(orbital_type_name(OrbitalType::Px) == "2px");
    BOOST_TEST(orbital_type_name(OrbitalType::Dz2) == "3dz^2");
    BOOST_TEST(orbital_type_name(OrbitalType::Fz3) == "4fz^3");
}

BOOST_AUTO_TEST_CASE(reports_angular_momentum) {
    BOOST_TEST(orbital_angular_momentum(OrbitalType::S) == 0u);
    BOOST_TEST(orbital_angular_momentum(OrbitalType::Px) == 1u);
    BOOST_TEST(orbital_angular_momentum(OrbitalType::Dz2) == 2u);
    BOOST_TEST(orbital_angular_momentum(OrbitalType::Fxyz) == 3u);
}

BOOST_AUTO_TEST_CASE(p_and_d_nodal_planes) {
    const glm::vec3 x_axis(1.0f, 0.0f, 0.0f);
    const glm::vec3 y_axis(0.0f, 1.0f, 0.0f);
    const glm::vec3 z_axis(0.0f, 0.0f, 1.0f);

    BOOST_CHECK_SMALL(orbital_angular_value(OrbitalType::Px, y_axis), 1.0e-6f);
    BOOST_CHECK_SMALL(orbital_angular_value(OrbitalType::Py, z_axis), 1.0e-6f);
    BOOST_CHECK_SMALL(orbital_angular_value(OrbitalType::Pz, x_axis), 1.0e-6f);
    BOOST_CHECK_SMALL(orbital_angular_value(OrbitalType::Dxy, z_axis), 1.0e-6f);
    BOOST_CHECK_SMALL(orbital_angular_value(OrbitalType::Dx2Y2,
        glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f))), 1.0e-6f);
}

BOOST_AUTO_TEST_CASE(inversion_parity) {
    const glm::vec3 direction = glm::normalize(glm::vec3(0.31f, -0.47f, 0.83f));

    for (OrbitalType type : {OrbitalType::Px, OrbitalType::Dxy, OrbitalType::Fxyz}) {
        const float value = orbital_angular_value(type, direction);
        const float inverted = orbital_angular_value(type, -direction);
        if (type == OrbitalType::Dxy) {
            BOOST_CHECK_SMALL(inverted - value, 1.0e-6f);
        } else {
            BOOST_CHECK_SMALL(inverted + value, 1.0e-6f);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END();
