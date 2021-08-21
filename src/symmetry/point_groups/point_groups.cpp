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

#include "point_groups.h"

/**
 * @brief List of supported point groups
 */
const std::vector<PointGroup> PointGroups::point_groups = {
    // {label, num_inversions, num_proper_rotations, num_improper_rotations, num_reflections}
    // nonaxial symmetries
    {Label(Class::C, 1)  , 0, {}, {}, 0},
    {Label(Class::Ci)    , 1, {}, {}, 0},
    {Label(Class::Cs)    , 0, {}, {}, 1},
    // cyclic groups
    {Label(Class::C, 2)  , 0, {{2, 1}}                 , {}, 0},
    {Label(Class::C, 3)  , 0, {{3, 1}}                 , {}, 0},
    {Label(Class::C, 4)  , 0, {{4, 1}, {2, 1}}         , {}, 0},
    {Label(Class::C, 5)  , 0, {{5, 1}}                 , {}, 0},
    {Label(Class::C, 6)  , 0, {{6, 1}, {3, 1}, {2, 1}} , {}, 0},
    {Label(Class::C, 7)  , 0, {{7, 1}}                 , {}, 0},
    {Label(Class::C, 8)  , 0, {{8, 1}, {4, 1}, {2, 1}} , {}, 0},
    {Label(Class::C, 9)  , 0, {{9, 1}, {3, 1}}         , {}, 0},
    {Label(Class::C, 10) , 0, {{10, 1}, {5, 1}, {2, 1}}, {}, 0},
    // reflection groups
    {Label(Class::Ch, 2) , 1, {{2, 1}}                 , {}               , 1},
    {Label(Class::Ch, 3) , 0, {{3, 1}}                 , {{3, 1}}         , 1},
    {Label(Class::Ch, 4) , 1, {{4, 1}, {2, 1}}         , {{4, 1}}         , 1},
    {Label(Class::Ch, 5) , 0, {{5, 1}}                 , {{5, 1}}         , 1},
    {Label(Class::Ch, 6) , 1, {{6, 1}, {3, 1}, {2, 1}} , {{6, 1}, {3, 1}} , 1},
    {Label(Class::Ch, 7) , 0, {{7, 1}}                 , {{7, 1}}         , 1},
    {Label(Class::Ch, 8) , 1, {{8, 1}, {4, 1}, {2, 1}} , {{8, 1}, {4, 1}} , 1},
    {Label(Class::Ch, 9) , 0, {{9, 1}, {3, 1}}         , {{9, 1}, {3, 1}} , 1},
    {Label(Class::Ch, 10), 1, {{10, 1}, {5, 1}, {2, 1}}, {{10, 1}, {5, 1}}, 1},
    // pyramidal groups
    {Label(Class::Cv, 2) , 0, {{2, 1}}                 , {}, 2},
    {Label(Class::Cv, 3) , 0, {{3, 1}}                 , {}, 3},
    {Label(Class::Cv, 4) , 0, {{4, 1}, {2, 1}}         , {}, 4},
    {Label(Class::Cv, 5) , 0, {{5, 1}}                 , {}, 5},
    {Label(Class::Cv, 6) , 0, {{6, 1}, {3, 1}, {2, 1}} , {}, 6},
    {Label(Class::Cv, 7) , 0, {{7, 1}}                 , {}, 7},
    {Label(Class::Cv, 8) , 0, {{8, 1}, {4, 1}, {2, 1}} , {}, 8},
    {Label(Class::Cv, 9) , 0, {{9, 1}, {3, 1}}         , {}, 9},
    {Label(Class::Cv, 10), 0, {{10, 1}, {5, 1}, {2, 1}}, {}, 10},
    // improper rotation groups
    {Label(Class::S, 4)  , 0, {{2, 1}}        , {{4, 1}} , 0},
    {Label(Class::S, 6)  , 1, {{3, 1}}        , {{6, 1}} , 0},
    {Label(Class::S, 8)  , 0, {{4, 1}, {2, 1}}, {{8, 1}} , 0},
    {Label(Class::S, 10) , 1, {{5, 1}}        , {{10, 1}}, 0},
    // dihedral groups
    {Label(Class::D, 2) , 0, {{2, 3}}                  , {}, 0},
    {Label(Class::D, 3) , 0, {{3, 1}, {2, 3}}          , {}, 0},
    {Label(Class::D, 4) , 0, {{4, 1}, {2, 5}}          , {}, 0},
    {Label(Class::D, 5) , 0, {{5, 1}, {2, 5}}          , {}, 0},
    {Label(Class::D, 6) , 0, {{6, 1}, {3, 1}, {2, 7}}  , {}, 0},
    {Label(Class::D, 7) , 0, {{7, 1}, {2, 7}}          , {}, 0},
    {Label(Class::D, 8) , 0, {{8, 1}, {4, 1}, {2, 9}}  , {}, 0},
    {Label(Class::D, 9) , 0, {{9, 1}, {3, 1}, {2, 9}}  , {}, 0},
    {Label(Class::D, 10), 0, {{10, 1}, {5, 1}, {2, 11}}, {}, 0},
    // prismatic groups
    {Label(Class::Dh, 2) , 1, {{2, 3}}                  , {}               , 3},
    {Label(Class::Dh, 3) , 0, {{3, 1}, {2, 3}}          , {{3, 1}}         , 4},
    {Label(Class::Dh, 4) , 1, {{4, 1}, {2, 5}}          , {{4, 1}}         , 5},
    {Label(Class::Dh, 5) , 0, {{5, 1}, {2, 5}}          , {{5, 1}}         , 6},
    {Label(Class::Dh, 6) , 1, {{6, 1}, {3, 1}, {2, 7}}  , {{6, 1}, {3, 1}} , 7},
    {Label(Class::Dh, 7) , 0, {{7, 1}, {2, 7}}          , {{7, 1}}         , 8},
    {Label(Class::Dh, 8) , 1, {{8, 1}, {4, 1}, {2, 9}}  , {{8, 1}, {4, 1}} , 9},
    {Label(Class::Dh, 9) , 0, {{9, 1}, {3, 1}, {2, 9}}  , {{9, 1}, {3, 1}} , 10},
    {Label(Class::Dh, 10), 1, {{10, 1}, {5, 1}, {2, 11}}, {{10, 1}, {5, 1}}, 11},
    // antiprismatic groups
    {Label(Class::Dd, 2) , 0, {{2, 3}}                  , {{4, 1}}         , 2},
    {Label(Class::Dd, 3) , 1, {{3, 1}, {2, 3}}          , {{6, 1}}         , 3},
    {Label(Class::Dd, 4) , 0, {{4, 1}, {2, 5}}          , {{8, 1}}         , 4},
    {Label(Class::Dd, 5) , 1, {{5, 1}, {2, 5}}          , {{10, 1}}        , 5},
    {Label(Class::Dd, 6) , 0, {{6, 1}, {3, 1}, {2, 7}}  , {{12, 1}, {4, 1}}, 6},
    {Label(Class::Dd, 7) , 1, {{7, 1}, {2, 7}}          , {{14, 1}}        , 7},
    {Label(Class::Dd, 8) , 0, {{8, 1}, {4, 1}, {2, 9}}  , {{16, 2}}        , 8},
    {Label(Class::Dd, 9) , 1, {{9, 1}, {2, 9}}          , {{18, 1}, {6, 1}}, 9},
    {Label(Class::Dd, 10), 0, {{10, 1}, {5, 1}, {2, 11}}, {{20, 1}}        , 10},
    // chiral tetrahedral symmetry
    {Label(Class::T)     , 0, {{3, 4}, {2, 3}}, {}, 0},
    // achiral tetrahedral symmetry
    {Label(Class::Td)    , 0, {{3, 4}, {2, 3}}, {{4, 3}}, 6},
    // pyritohedral symmetry
    {Label(Class::Th)    , 1, {{3, 4}, {2, 3}}, {{6, 4}}, 3},
    // chiral octahedral symmetry
    {Label(Class::O)     , 0, {{4, 3}, {3, 4}, {2, 9}}, {}, 0},
    // achiral octahedral symmetry
    {Label(Class::Oh)    , 1, {{4, 3}, {3, 4}, {2, 9}}, {{6, 4}, {4, 3}}, 9},
    // chiral icosahedral symmetry
    {Label(Class::I)     , 0, {{5, 6}, {3, 10}, {2, 15}}, {}, 0},
    // achiral icosahedral symmetry
    {Label(Class::Ih)    , 1, {{5, 6}, {3, 10}, {2, 15}}, {{10, 6}, {6, 10}}, 15},
    // linear
    {Label(Class::Cinfv) , 0, {{ProperRotation::DEGREE_INF, 1}}, {}, 0},
    {Label(Class::Dinfh) , 1, {{ProperRotation::DEGREE_INF, 1}}, {{ImproperRotation::DEGREE_INF, 1}}, 0}
};
