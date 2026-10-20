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

#include <complex>
#include <set>
#include <string>
#include <vector>
#include <boost/test/unit_test.hpp>
#include "../src/symmetry/point_groups/point_groups.h"

namespace {
// total number of operations, including the identity
unsigned int count_operations(const std::vector<OperationLabelCount>& operations) {
    unsigned int count = 1;
    for (auto& operation : operations) count += operation.get_count();
    return count;
}

bool same_label_except_component(const IrrepLabel& a, const IrrepLabel& b) {
    return a.get_mulliken() == b.get_mulliken() && a.get_subscript() == b.get_subscript() &&
        a.get_parity() == b.get_parity() && a.get_prime() == b.get_prime();
}
}

BOOST_AUTO_TEST_SUITE(character_tables);

BOOST_AUTO_TEST_CASE(class_sizes_sum_to_order) {
    for (const PointGroup& point_group : PointGroups::point_groups) {
        if (point_group.get_label().is_linear()) continue;
        BOOST_TEST_CONTEXT("Point group: " << point_group.get_label().get_name()) {
            BOOST_TEST(count_operations(point_group.get_unique_operations()) == point_group.get_order());
            BOOST_TEST(count_operations(point_group.get_character_columns()) == point_group.get_order());
        }
    }
}

// little/great orthogonality theorem: sum_R chi_i(R) chi_j(R)* = h delta_ij
BOOST_AUTO_TEST_CASE(characters_are_orthonormal) {
    for (const PointGroup& point_group : PointGroups::point_groups) {
        if (point_group.get_label().is_linear()) continue;
        BOOST_TEST_CONTEXT("Point group: " << point_group.get_label().get_name()) {
            const auto& columns = point_group.get_character_columns();
            const auto& characters = point_group.get_characters();
            const auto& irreps = point_group.get_irreps();

            std::vector<double> weights = {1.0};  // identity
            for (auto& column : columns) weights.push_back(column.get_count());

            // a character table is square: as many irreps as classes
            BOOST_REQUIRE(irreps.size() == weights.size());
            BOOST_REQUIRE(characters.size() == irreps.size());

            for (unsigned int i = 0; i < irreps.size(); ++i) {
                BOOST_REQUIRE(characters[i].size() == weights.size());
                for (unsigned int j = 0; j < irreps.size(); ++j) {
                    std::complex<double> dot = 0;
                    for (unsigned int k = 0; k < weights.size(); ++k) {
                        dot += weights[k] * characters[i][k] * std::conj(characters[j][k]);
                    }
                    const double expected = (i == j) ? point_group.get_order() : 0.0;
                    BOOST_TEST_CONTEXT(irreps[i].get_name() << " . " << irreps[j].get_name()) {
                        BOOST_TEST(std::abs(dot - expected) < 1e-8);
                    }
                }
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(complex_irreps_come_in_conjugate_pairs) {
    for (const PointGroup& point_group : PointGroups::point_groups) {
        if (point_group.get_label().is_linear()) continue;
        BOOST_TEST_CONTEXT("Point group: " << point_group.get_label().get_name()) {
            const auto& characters = point_group.get_characters();
            const auto& irreps = point_group.get_irreps();

            for (unsigned int i = 0; i < irreps.size(); ++i) {
                bool complex = false;
                for (auto& character : characters[i]) complex |= std::abs(character.imag()) > 1e-8;
                BOOST_TEST(complex == (irreps[i].get_component() != IrrepLabel::Component::None));

                if (irreps[i].get_component() != IrrepLabel::Component::Plus) continue;

                // E+ is directly followed by its complex conjugate E-
                BOOST_REQUIRE(i + 1 < irreps.size());
                BOOST_TEST((irreps[i + 1].get_component() == IrrepLabel::Component::Minus));
                BOOST_TEST(same_label_except_component(irreps[i], irreps[i + 1]));
                for (unsigned int k = 0; k < characters[i].size(); ++k) {
                    BOOST_TEST(std::abs(characters[i + 1][k] - std::conj(characters[i][k])) < 1e-8);
                }
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(point_groups_with_complex_irreps) {
    std::set<std::string> expected = {
        "C3", "C4", "C5", "C6", "C7", "C8", "C9", "C10",
        "C3h", "C4h", "C5h", "C6h", "C7h", "C8h", "C9h", "C10h",
        "S4", "S6", "S8", "S10", "T", "Th"
    };

    std::set<std::string> found;
    for (const PointGroup& point_group : PointGroups::point_groups) {
        if (point_group.has_complex_characters()) found.insert(point_group.get_label().get_name());
    }

    BOOST_TEST(found == expected, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(S4_character_table) {
    const PointGroup& S4 = PointGroups::find(PointGroupLabel(PointGroupLabel::Class::S, 4));

    // E, S4, C2, S4^3: the class 2 S4 is split, as S4 and S4^3 have conjugate characters
    std::vector<std::string> columns;
    for (auto column : S4.get_character_columns()) columns.push_back(column.get_short_name());
    BOOST_TEST(columns == std::vector<std::string>({"S4", "C2", "S4^3"}), boost::test_tools::per_element());

    std::vector<std::string> irreps;
    for (auto& irrep : S4.get_irreps()) irreps.push_back(irrep.get_name());
    BOOST_TEST(irreps == std::vector<std::string>({"A", "B", "E+", "E−"}), boost::test_tools::per_element());

    const std::complex<double> i(0, 1);
    const std::vector<std::complex<double>> E_plus = {1.0, i, -1.0, -i};
    for (unsigned int k = 0; k < E_plus.size(); ++k) {
        BOOST_TEST(std::abs(S4.get_characters()[2][k] - E_plus[k]) < 1e-12);
    }
}

BOOST_AUTO_TEST_SUITE_END();
