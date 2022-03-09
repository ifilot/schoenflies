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

#include "library.h"

/**
 * @brief Construct a new Library object
 */
Library::Library() {
    this->add_items_from_package(":/assets/structures/library.json");
}

/**
 * @brief Get the items in the library
 *
 * @return std::vector<LibraryItem>&
 */
std::vector<LibraryItem>& Library::get_items() {
    if (!this->items_sorted) {
        std::sort(this->items.begin(), this->items.end());
        this->items_sorted = true;
    }

    return this->items;
}

/**
 * @brief Add items from a package
 *
 * @param path path to the package metadata file
 */
void Library::add_items_from_package(const std::string path) {
    LibraryPackage package(path);

    for (unsigned int i = 0; i < package.get_size(); ++i) {
        this->items.push_back(package.get_item(i));
    }

    this->items_sorted = false;
}
