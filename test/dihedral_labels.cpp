#include <boost/test/unit_test.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <limits>
#include "../src/library/library.h"
#include "../src/structure.h"
#include "../src/symmetry/symmetry.h"

BOOST_AUTO_TEST_SUITE(dihedral_labels)

BOOST_AUTO_TEST_CASE(all_library_axis_and_plane_families) {
    Library library;
    for (const auto& item : library.get_items()) {
        auto structure = std::make_shared<Structure>(item.get_path());
        Symmetry symmetry(structure);
        const auto group = symmetry.get_point_group().get_label();
        const bool dihedral = group.get_class() == PointGroupLabel::Class::D ||
                              group.get_class() == PointGroupLabel::Class::Dh;
        const bool cyclic = group.get_class() == PointGroupLabel::Class::Cv;
        if ((!dihedral && !cyclic) || group.get_order() % 2 != 0) continue;
        BOOST_TEST_CONTEXT("Library labels: " << item.get_name()) {
            unsigned int min_single = std::numeric_limits<unsigned int>::max(), max_double = 0;
            unsigned int min_vertical = std::numeric_limits<unsigned int>::max(), max_dihedral = 0;
            unsigned int doubles = 0, verticals = 0, dihedrals = 0;
            std::vector<glm::vec3> primed_axes;
            auto& operations = symmetry.get_operation_manager()->get_operations();
            for (auto& operation : operations) {
                const auto label = operation.get_label();
                if (label.get_element() != OperationLabel::Element::ProperRotation ||
                    operation.get_degree() != 2 || label.get_prime() == OperationLabel::Prime::None) continue;
                unsigned int fixed_atoms = 0;
                for (unsigned int i = 0; i < structure->get_num_atoms(); ++i) {
                    if (operation.get_result_index(i) == i) ++fixed_atoms;
                }
                if (label.get_prime() == OperationLabel::Prime::Single) {
                    min_single = std::min(min_single, fixed_atoms);
                    primed_axes.push_back(operation.get_axis());
                } else {
                    ++doubles;
                    max_double = std::max(max_double, fixed_atoms);
                }
            }
            if (dihedral) {
                BOOST_REQUIRE(!primed_axes.empty());
                BOOST_TEST(primed_axes.size() == group.get_order() / 2);
                BOOST_TEST(doubles == group.get_order() / 2);
                BOOST_TEST(min_single >= max_double);
                BOOST_TEST_MESSAGE(item.get_name() << ": C2' atoms >= " << min_single << ", C2'' atoms <= " << max_double);
            }
            for (auto& operation : operations) {
                const auto label = operation.get_label();
                if (label.get_element() != OperationLabel::Element::Reflection ||
                    label.get_plane() == OperationLabel::Plane::Horizontal) continue;
                unsigned int fixed_atoms = 0;
                for (unsigned int i = 0; i < structure->get_num_atoms(); ++i) {
                    if (operation.get_result_index(i) == i) ++fixed_atoms;
                }
                const bool vertical = label.get_plane() == OperationLabel::Plane::Vertical;
                if (vertical) {
                    ++verticals;
                    min_vertical = std::min(min_vertical, fixed_atoms);
                } else {
                    ++dihedrals;
                    max_dihedral = std::max(max_dihedral, fixed_atoms);
                }
                if (dihedral && group.get_order() > 2) {
                    bool contains_primed_axis = false;
                    for (const auto& axis : primed_axes) {
                        contains_primed_axis |= std::abs(glm::dot(axis, operation.get_axis())) < .02f;
                    }
                    BOOST_TEST_CONTEXT("Plane " << label.get_name() << "; dot(normal, y) = " <<
                                       glm::dot(operation.get_axis(), symmetry.get_y_axis())) {
                        BOOST_TEST(vertical == contains_primed_axis);
                    }
                }
            }
            if (cyclic && group.get_order() > 2) BOOST_TEST(min_vertical >= max_dihedral);
            if ((cyclic || group.get_class() == PointGroupLabel::Class::Dh) && group.get_order() > 2) {
                BOOST_TEST(verticals == group.get_order() / 2);
                BOOST_TEST(dihedrals == group.get_order() / 2);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(benzene_preserves_atom_and_bond_families) {
    auto structure = std::make_shared<Structure>(":/assets/structures/benzene.xyz");
    Symmetry symmetry(structure);
    for (auto& operation : symmetry.get_operation_manager()->get_operations()) {
        const auto label = operation.get_label();
        const auto axis = operation.get_axis();
        if (std::abs(glm::dot(axis, symmetry.get_z_axis())) > .98f) continue;
        const bool rotation = label.get_element() == OperationLabel::Element::ProperRotation;
        const bool reflection = label.get_element() == OperationLabel::Element::Reflection;
        if ((!rotation || operation.get_degree() != 2) && !reflection) continue;
        unsigned int carbons = 0;
        for (unsigned int i = 0; i < structure->get_num_atoms(); ++i) {
            if (structure->get_atomic_number(i) != 6) continue;
            const auto position = structure->get_coordinates(i);
            const float distance = rotation ? glm::length(glm::cross(axis, position)) :
                                              std::abs(glm::dot(axis, position));
            if (distance < .1f) ++carbons;
        }
        const bool through_atoms = rotation ? label.get_prime() == OperationLabel::Prime::Single :
                                             label.get_plane() == OperationLabel::Plane::Vertical;
        BOOST_TEST(carbons == (through_atoms ? 2u : 0u));
    }
}

BOOST_AUTO_TEST_CASE(crown_C2_and_mirror_families) {
    auto structure = std::make_shared<Structure>(":/assets/structures/18-crown-6.xyz");
    Symmetry symmetry(structure);
    PointGroupLabel expected(PointGroupLabel::Class::Dh, 6);
    BOOST_REQUIRE(symmetry.get_point_group().get_label().matches(expected));

    unsigned int singles = 0, doubles = 0, verticals = 0, dihedrals = 0;
    for (auto& operation : symmetry.get_operation_manager()->get_operations()) {
        const auto label = operation.get_label();
        const auto axis = operation.get_axis();
        unsigned int oxygens = 0;
        if (label.get_element() == OperationLabel::Element::ProperRotation && operation.get_degree() == 2) {
            if (std::abs(glm::dot(axis, symmetry.get_z_axis())) > .98f) continue;
            for (unsigned int i = 0; i < structure->get_num_atoms(); ++i) {
                if (structure->get_atomic_number(i) == 8 &&
                    glm::length(glm::cross(axis, structure->get_coordinates(i))) < .1f) ++oxygens;
            }
            if (label.get_prime() == OperationLabel::Prime::Single) {
                ++singles;
                BOOST_TEST(oxygens == 2u);
            } else {
                BOOST_CHECK(label.get_prime() == OperationLabel::Prime::Double);
                ++doubles;
                BOOST_TEST(oxygens == 0u);
            }
        } else if (label.get_element() == OperationLabel::Element::Reflection) {
            if (label.get_plane() == OperationLabel::Plane::Horizontal) continue;
            for (unsigned int i = 0; i < structure->get_num_atoms(); ++i) {
                if (structure->get_atomic_number(i) == 8 &&
                    std::abs(glm::dot(axis, structure->get_coordinates(i))) < .1f) ++oxygens;
            }
            if (label.get_plane() == OperationLabel::Plane::Vertical) {
                ++verticals;
                BOOST_TEST(oxygens == 2u);
            } else {
                BOOST_CHECK(label.get_plane() == OperationLabel::Plane::Dihedral);
                ++dihedrals;
                BOOST_TEST(oxygens == 0u);
            }
        }
    }
    BOOST_TEST(singles == 3u);
    BOOST_TEST(doubles == 3u);
    BOOST_TEST(verticals == 3u);
    BOOST_TEST(dihedrals == 3u);
}

BOOST_AUTO_TEST_SUITE_END()
