/**
 * Schoenflies
 * Copyright (c) 2022 Luuk Kempen
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
#include <string>
#include <vector>
#include <boost/test/unit_test.hpp>
#include "../src/library/library.h"
#include "../src/library/library_item.h"
#include "../src/practice/practice_irreps.h"
#include "../src/practice/practice_structure.h"
#include "../src/symmetry/symmetry.h"
#include "../src/basis_set.h"
#include "../src/structure.h"
#include "utils.h"

namespace {
// practice irreps for a library structure, using only the basis set with the given index
PracticeIrreps irreps_for_basis_set(const LibraryItem& item, std::shared_ptr<Structure> structure,
                                    std::shared_ptr<Symmetry> symmetry, unsigned int basis_set_index) {
    json config = item.get_item_practice_config();
    config["basis_sets"] = json::array({config["basis_sets"][basis_set_index]});
    structure->set_library_item(std::make_shared<LibraryItem>(
        item.get_path(), item.get_name(), item.get_chemical_formula(), item.get_point_group_label(), config));
    return PracticeIrreps(std::make_shared<PracticeStructure>(symmetry, false));
}

LibraryItem find_library_item(const std::string& name) {
    Library library;
    for (auto item : library.get_items()) {
        if (item.get_name() == name) return item;
    }
    throw std::runtime_error("Library item not found: " + name);
}
}

BOOST_AUTO_TEST_SUITE(practice_irreps);

BOOST_AUTO_TEST_CASE(benzene) {
    std::string file = resolve_path("test/files/test_library_practice.json");
    Library library(false);
    library.add_items_from_package(file);
    LibraryItem item = library.get_items()[0];

    auto struc = std::make_shared<Structure>(item.get_path());
    struc->set_library_item(std::make_shared<LibraryItem>(item));
    auto symmetry = std::make_shared<Symmetry>(struc);
    auto practice_structure = std::make_shared<PracticeStructure>(symmetry, false);
    PracticeIrreps irreps(practice_structure);

    BOOST_TEST(practice_structure->get_basis_set()->get_element().symbol == "C");
    BOOST_TEST(practice_structure->get_basis_set()->get_orbital_label().get_principal() == 2);
    BOOST_TEST(practice_structure->get_basis_set()->get_orbital_label().get_azimuthal() == 1);
    BOOST_TEST(practice_structure->get_basis_set()->get_orbital_label().get_magnetic() == 0);

    auto calculated_characters = irreps.get_correct_characters();
    std::vector<double> correct_characters = {0, 0, 0, -6, 0, 0, 0, 0, -6, 6, 0};
    for (unsigned int i = 0; i < correct_characters.size(); ++i) {
        BOOST_TEST(calculated_characters[i].second == correct_characters[i], boost::test_tools::tolerance(1e-8));
    }

    auto calculated_irreps = irreps.get_correct_irreps();
    std::vector<double> correct_irreps = {0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1};
    for (unsigned int i = 0; i < correct_irreps.size(); ++i) {
        BOOST_TEST(calculated_irreps[i].second == correct_irreps[i], boost::test_tools::tolerance(1e-8));
    }
}

// S4 has the complex-conjugate pair E+/E-: the little orthogonality theorem
// only gives the correct multiplicities with the complex conjugate characters
BOOST_AUTO_TEST_CASE(tetrathiazyl_tetrafluoride) {
    LibraryItem item = find_library_item("Tetrathiazyl tetrafluoride");
    auto structure = std::make_shared<Structure>(item.get_path());
    auto symmetry = std::make_shared<Symmetry>(structure);
    PracticeIrreps irreps = irreps_for_basis_set(item, structure, symmetry, 0);  // F 2s

    // S4, C2, S4^3: none of the four F atoms stays in place
    auto calculated_characters = irreps.get_correct_characters();
    BOOST_REQUIRE(calculated_characters.size() == 3);
    for (auto& character : calculated_characters) {
        BOOST_TEST(character.second == 0.0, boost::test_tools::tolerance(1e-8));
    }

    // A + B + E+ + E-
    auto calculated_irreps = irreps.get_correct_irreps();
    std::vector<double> correct_irreps = {1, 1, 1, 1};
    BOOST_REQUIRE(calculated_irreps.size() == correct_irreps.size());
    for (unsigned int i = 0; i < correct_irreps.size(); ++i) {
        BOOST_TEST(calculated_irreps[i].second == correct_irreps[i], boost::test_tools::tolerance(1e-8));
    }
}

BOOST_AUTO_TEST_CASE(tetraphenylmethane) {
    LibraryItem item = find_library_item("Tetraphenylmethane");
    auto structure = std::make_shared<Structure>(item.get_path());
    auto symmetry = std::make_shared<Symmetry>(structure);
    PracticeIrreps irreps = irreps_for_basis_set(item, structure, symmetry, 0);  // C 2s

    // only the central carbon atom stays in place
    auto calculated_characters = irreps.get_correct_characters();
    std::vector<double> correct_characters = {1, 1, 1};
    BOOST_REQUIRE(calculated_characters.size() == correct_characters.size());
    for (unsigned int i = 0; i < correct_characters.size(); ++i) {
        BOOST_TEST(calculated_characters[i].second == correct_characters[i], boost::test_tools::tolerance(1e-8));
    }

    // 7 A + 6 B + 6 E+ + 6 E-
    auto calculated_irreps = irreps.get_correct_irreps();
    std::vector<double> correct_irreps = {7, 6, 6, 6};
    BOOST_REQUIRE(calculated_irreps.size() == correct_irreps.size());
    for (unsigned int i = 0; i < correct_irreps.size(); ++i) {
        BOOST_TEST(calculated_irreps[i].second == correct_irreps[i], boost::test_tools::tolerance(1e-8));
    }
}

// for every basis set of every irreps exercise, the multiplicities must be
// non-negative integers whose dimensions add up to the number of orbitals
BOOST_AUTO_TEST_CASE(all_library_reductions_are_consistent) {
    Library library;
    for (const auto& item : library.get_items()) {
        const json& config = item.get_item_practice_config();
        if (!config.value("irreps", json::object()).value("enabled", false)) continue;

        auto structure = std::make_shared<Structure>(item.get_path());
        auto symmetry = std::make_shared<Symmetry>(structure);
        const auto& characters = symmetry->get_point_group().get_characters();

        for (unsigned int b = 0; b < config["basis_sets"].size(); ++b) {
            BOOST_TEST_CONTEXT("Library irreps: " << item.get_name() << ", basis set " << b) {
                PracticeIrreps irreps = irreps_for_basis_set(item, structure, symmetry, b);
                auto calculated_irreps = irreps.get_correct_irreps();

                double dimension = 0;
                for (unsigned int i = 0; i < calculated_irreps.size(); ++i) {
                    BOOST_TEST(calculated_irreps[i].second >= 0);
                    dimension += calculated_irreps[i].second * characters[i][0].real();

                    // both components of a complex-conjugate pair occur equally often
                    if (calculated_irreps[i].first.get_component() == IrrepLabel::Component::Plus) {
                        BOOST_TEST(calculated_irreps[i].second == calculated_irreps[i + 1].second);
                    }
                }

                json basis_set = config["basis_sets"][b];
                double num_orbitals = BasisSet(basis_set, symmetry).get_atoms().size();
                BOOST_TEST(dimension == num_orbitals, boost::test_tools::tolerance(1e-8));
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END();
