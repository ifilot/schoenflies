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

#ifndef LIBRARY_LIBRARY_H
#define LIBRARY_LIBRARY_H

#include <iostream>

#include <algorithm>
#include <string>
#include <vector>
#include "library_item.h"
#include "library_package.h"

class Library {
private:
    std::vector<LibraryItem> items;
    bool items_sorted = false;

public:
    /**
     * @brief Construct a new Library object
     */
    Library();

    /**
     * @brief Get the items in the library
     *
     * @return std::vector<LibraryItem>&
     */
    std::vector<LibraryItem>& get_items();

private:
    /**
     * @brief Add items from a package
     *
     * @param path path to the package metadata file
     */
    void add_items_from_package(const std::string path);
};

#endif  // LIBRARY_LIBRARY_H
