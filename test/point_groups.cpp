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
#include "../src/structure.h"
#include "../src/symmetry/symmetry.h"
#include "../src/symmetry/point_groups/point_group.h"
#include "utils.h"

BOOST_AUTO_TEST_SUITE(point_groups);

BOOST_AUTO_TEST_CASE(adamantane) {
    std::string file = resolve_path("test/files/adamantane.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Td);
}

BOOST_AUTO_TEST_CASE(ammonia) {
    std::string file = resolve_path("test/files/ammonia.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Cv | 3);
}

BOOST_AUTO_TEST_CASE(benzene) {
    std::string file = resolve_path("test/files/benzene.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dh | 6);
}

BOOST_AUTO_TEST_CASE(bicyclooctane) {
    std::string file = resolve_path("test/files/bicyclooctane.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::D | 3);
}

BOOST_AUTO_TEST_CASE(biphenyl) {
    std::string file = resolve_path("test/files/biphenyl.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::D | 2);
}

BOOST_AUTO_TEST_CASE(boric_acid) {
    std::string file = resolve_path("test/files/boric-acid.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Ch | 3);
}

BOOST_AUTO_TEST_CASE(buckminsterfullerene) {
    std::string file = resolve_path("test/files/buckminsterfullerene.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Ih);
}

BOOST_AUTO_TEST_CASE(carbon_dioxide) {
    std::string file = resolve_path("test/files/carbon-dioxide.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dinfh);
}

BOOST_AUTO_TEST_CASE(coronene) {
    std::string file = resolve_path("test/files/coronene.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dh | 6);
}

BOOST_AUTO_TEST_CASE(cubane) {
    std::string file = resolve_path("test/files/cubane.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Oh);
}

BOOST_AUTO_TEST_CASE(cyclobutadiene) {
    std::string file = resolve_path("test/files/cyclobutadiene.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dh | 4);
}

BOOST_AUTO_TEST_CASE(cyclooctatetraene) {
    std::string file = resolve_path("test/files/cyclooctatetraene.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dd | 2);
}

BOOST_AUTO_TEST_CASE(diborane) {
    std::string file = resolve_path("test/files/diborane.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dh | 2);
}

BOOST_AUTO_TEST_CASE(dodecahydrododecaborate) {
    std::string file = resolve_path("test/files/dodecahydrododecaborate.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Ih);
}

BOOST_AUTO_TEST_CASE(e_hex_3_ene) {
    std::string file = resolve_path("test/files/E-hex-3-ene.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Ci);
}

BOOST_AUTO_TEST_CASE(ferrocene_eclipsed) {
    std::string file = resolve_path("test/files/ferrocene-eclipsed.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dh | 5);
}

BOOST_AUTO_TEST_CASE(ferrocene_staggered) {
    std::string file = resolve_path("test/files/ferrocene-staggered.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dd | 5);
}

BOOST_AUTO_TEST_CASE(fluorochlorobromomethane) {
    std::string file = resolve_path("test/files/fluorochlorobromomethane.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::C | 1);
}

BOOST_AUTO_TEST_CASE(hydrazine) {
    std::string file = resolve_path("test/files/hydrazine.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::C | 2);
}

BOOST_AUTO_TEST_CASE(hydrogen_chloride) {
    std::string file = resolve_path("test/files/hydrogen-chloride.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Cinfv);
}

BOOST_AUTO_TEST_CASE(inositol) {
    std::string file = resolve_path("test/files/inositol.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dd | 3);
}

BOOST_AUTO_TEST_CASE(iron_pentacarbonyl) {
    std::string file = resolve_path("test/files/iron-pentacarbonyl.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dh | 3);
}

BOOST_AUTO_TEST_CASE(methane) {
    std::string file = resolve_path("test/files/methane.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Td);
}

BOOST_AUTO_TEST_CASE(octasulfur) {
    std::string file = resolve_path("test/files/octasulfur.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dd | 4);
}

BOOST_AUTO_TEST_CASE(pentaborane_9) {
    std::string file = resolve_path("test/files/pentaborane-9.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Cv | 4);
}

BOOST_AUTO_TEST_CASE(prismane) {
    std::string file = resolve_path("test/files/prismane.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dh | 3);
}

BOOST_AUTO_TEST_CASE(sulfur_hexafluoride) {
    std::string file = resolve_path("test/files/sulfur-hexafluoride.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Oh);
}

BOOST_AUTO_TEST_CASE(thionyl_chloride) {
    std::string file = resolve_path("test/files/thionyl-chloride.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Cs);
}

BOOST_AUTO_TEST_CASE(trans_azobenzene) {
    std::string file = resolve_path("test/files/trans-azobenzene.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Ch | 2);
}

BOOST_AUTO_TEST_CASE(triethylamine) {
    std::string file = resolve_path("test/files/triethylamine.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::C | 3);
}

BOOST_AUTO_TEST_CASE(tropylium) {
    std::string file = resolve_path("test/files/tropylium.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Dh | 7);
}

BOOST_AUTO_TEST_CASE(water) {
    std::string file = resolve_path("test/files/water.xyz");
    auto struc = std::make_shared<Structure>(file);
    Symmetry symmetry(struc);
    unsigned char identifier = symmetry.get_point_group().get_identifier();

    BOOST_TEST(identifier == PointGroup::Class::Cv | 2);
}

BOOST_AUTO_TEST_SUITE_END();
