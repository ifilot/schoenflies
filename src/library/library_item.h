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

#ifndef LIBRARY_LIBRARY_ITEM_H
#define LIBRARY_LIBRARY_ITEM_H

#include <string>
#include "../symmetry/point_groups/point_group_label.h"

class LibraryItem {
private:
    std::string path;
    std::string name;
    std::string chemical_formula;
    PointGroupLabel point_group_label;

public:
    /**
     * @brief Construct a new LibraryItem object
     *
     * @param path path of the XYZ file
     * @param name name of the structure
     * @param chemical_formula chemical formula of the structure
     * @param point_group_label point group label of the structure
     */
    LibraryItem(const std::string path,
                const std::string name,
                const std::string chemical_formula,
                const PointGroupLabel point_group_label);

    /**
     * @brief Get the path of the item
     *
     * @return const std::string&
     */
    const std::string& get_path() const;

    /**
     * @brief Get the name of the item
     *
     * @return const std::string&
     */
    const std::string& get_name() const;

    /**
     * @brief Get the chemical formula of the item
     *
     * @return const std::string&
     */
    const std::string& get_chemical_formula() const;

    /**
     * @brief Get the point group label of the item
     *
     * @return const PointGroupLabel&
     */
    const PointGroupLabel& get_point_group_label() const;

    /**
     * @brief Less than operator
     *
     * @param other
     * @return true
     * @return false
     */
    bool operator<(const LibraryItem& other) const;
};


#endif  // LIBRARY_LIBRARY_ITEM_H
