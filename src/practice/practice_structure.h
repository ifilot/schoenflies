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

#ifndef PRACTICE_PRACTICE_STRUCTURE_H
#define PRACTICE_PRACTICE_STRUCTURE_H

#include <algorithm>
#include <memory>
#include <random>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "../symmetry/symmetry.h"
#include "../basis_set.h"
#include "../structure.h"

using json = nlohmann::json;

class PracticeStructure {
private:
    std::shared_ptr<Symmetry> symmetry;

    bool basis_set_generated = false;
    std::shared_ptr<BasisSet> basis_set;

    std::mt19937 random_engine;

public:
    /**
     * @brief Construct a new PracticeStructure object
     *
     * @param symmetry
     */
    PracticeStructure(std::shared_ptr<Symmetry> symmetry);

    /**
     * @brief Get the symmetry object
     *
     * @return const std::shared_ptr<Symmetry>
     */
    const std::shared_ptr<Symmetry> get_symmetry() const;

    /**
     * @brief Get the basis set object (generate one if none exists yet)
     *
     * @return const std::shared_ptr<BasisSet>
     */
    const std::shared_ptr<BasisSet> get_basis_set();

private:
    /**
     * @brief Generate a random basis set object
     */
    void generate_basis_set();
};

#endif  // PRACTICE_PRACTICE_STRUCTURE_H
