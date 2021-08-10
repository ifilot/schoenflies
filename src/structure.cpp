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

#include "structure.h"

/**
 * @brief Default constructor
 */
Structure::Structure() {}

/**
 * @brief Construct a new Structure object from a file
 *
 * @param path
 */
Structure::Structure(const std::string& path) {
    this->load_from_file(path);
}

/**
 * @brief Load structure from file
 *
 * @param path
 */
void Structure::load_from_file(const std::string& path) {
    boost::filesystem::path p(path);
    this->filename = p.filename().string();

    // determine file type
    if (this->filename.find(".xyz") != std::string::npos) {
        this->load_from_xyz(path);
    } else {
        throw std::runtime_error("File is not supported: " + path);
    }
}

/**
 * @brief Load structure from xyz file
 *
 * @param path
 */
void Structure::load_from_xyz(const std::string& path) {
    std::ifstream ifs(path);

    if (!ifs.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }

    std::string line;  // container for std::getline result
    std::vector<std::string> pieces;  // container for boost::split result

    // read number of atoms
    std::getline(ifs, line);
    boost::trim(line);
    this->num_atoms = boost::lexical_cast<unsigned int>(line);

    // read description
    std::getline(ifs, line);
    boost::trim(line);
    this->description = line;

    // read atoms
    for (unsigned int i = 0; i < this->num_atoms; ++i) {
        std::getline(ifs, line);
        boost::trim(line);
        boost::split(pieces, line, boost::is_any_of("\t "), boost::token_compress_on);

        if (pieces.size() == 4) {
            this->atomic_numbers.push_back(PeriodicTable::get_atomic_number(pieces[0]));
            this->coordinates.emplace_back(
                boost::lexical_cast<float>(pieces[1]),
                boost::lexical_cast<float>(pieces[2]),
                boost::lexical_cast<float>(pieces[3])
            );
        } else {
            throw std::runtime_error("File has invalid format: " + path);
        }
    }

    ifs.close();
}

/**
 * @brief Get the number of atoms in the structure
 *
 * @return const unsigned int
 */
const unsigned int Structure::get_num_atoms() const {
    return this->num_atoms;
}

/**
 * @brief Get the coordinates of the atoms in the structure
 *
 * @return const std::vector<glm::vec3>&
 */
const std::vector<glm::vec3>& Structure::get_coordinates() const {
    return this->coordinates;
}

/**
 * @brief Get the coordinates of an atom in the structure by index
 *
 * @param index index of atom
 * @return const glm::vec3
 */
const glm::vec3 Structure::get_coordinates(unsigned int index) const {
    if (index < this->get_num_atoms()) {
        return this->coordinates[index];
    } else {
        throw std::runtime_error("Requested invalid atom index.");
    }
}

/**
 * @brief Get the atomic numbers of the atoms in the structure
 *
 * @return const std::vector<unsigned int>&
 */
const std::vector<unsigned int>& Structure::get_atomic_numbers() const {
    return this->atomic_numbers;
}

/**
 * @brief Get the atomic number of an atom in the structure by index
 *
 * @param index index of atom
 * @return const unsigned int
 */
const unsigned int Structure::get_atomic_number(unsigned int index) const {
    if (index < this->get_num_atoms()) {
        return this->atomic_numbers[index];
    } else {
        throw std::runtime_error("Requested invalid atom index.");
    }
}

/**
 * @brief Get the description of the structure
 *
 * @return const std::string&
 */
const std::string& Structure::get_description() const {
    return this->description;
}

/**
 * @brief Get the filename of the structure
 *
 * @return const std::string&
 */
const std::string& Structure::get_filename() const {
    return this->filename;
}
