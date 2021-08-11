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

#include "geometry.h"

/**
 * @brief Create a unique pointer to a sphere model
 *
 * @param tesselation_level detail of the sphere
 * @return Model
 */
std::unique_ptr<Model> Geometry::sphere(unsigned int tesselation_level) {
    std::vector<glm::vec3> vertices = {
        glm::vec3( 1.0f,  0.0f,  0.0f),
        glm::vec3(-1.0f,  0.0f,  0.0f),
        glm::vec3( 0.0f,  1.0f,  0.0f),
        glm::vec3( 0.0f, -1.0f,  0.0f),
        glm::vec3( 0.0f,  0.0f,  1.0f),
        glm::vec3( 0.0f,  0.0f, -1.0f)
    };

    std::vector<unsigned int> indices = {
        0, 3, 5,
        3, 1, 5,
        3, 4, 1,
        0, 4, 3,
        2, 0, 5,
        2, 5, 1,
        4, 0, 2,
        4, 2, 1
    };

    std::vector<unsigned int> new_indices;

    for (unsigned int j = 0; j < tesselation_level; j++) {
        new_indices.resize(0);
        unsigned int size = indices.size();

        for (unsigned int i = 0; i < size; i += 3) {
            glm::vec3 centre1 = glm::normalize((vertices[indices[i]]     + vertices[indices[i + 1]]) / 2.0f);
            glm::vec3 centre2 = glm::normalize((vertices[indices[i]]     + vertices[indices[i + 2]]) / 2.0f);
            glm::vec3 centre3 = glm::normalize((vertices[indices[i + 1]] + vertices[indices[i + 2]]) / 2.0f);

            vertices.insert(vertices.end(), {centre1, centre2, centre3});

            unsigned int a = vertices.size() - 3;
            unsigned int b = vertices.size() - 2;
            unsigned int c = vertices.size() - 1;

            new_indices.insert(new_indices.end(), {
                indices[i], a, b,
                indices[i + 1], c, a,
                indices[i + 2], b, c,
                a, c, b
            });
        }

        indices = new_indices;
    }

    return std::make_unique<Model>(vertices, vertices, indices);
}

/**
 * @brief Create a unique pointer to a cylinder model
 *
 * @param stack_count number of stacks in axial direction
 * @param slice_count number of slices in radial direction
 * @return Model
 */
std::unique_ptr<Model> Geometry::cylinder(unsigned int stack_count, unsigned int slice_count) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    // construct vertices and normals
    for (unsigned int stack = 0; stack < stack_count; ++stack) {
        for (unsigned int slice = 0; slice < slice_count; ++slice) {
            float angle = (2.0f * (float) M_PI * slice) / slice_count;
            float x = std::sin(angle);
            float y = std::cos(angle);
            float z = stack / (stack_count - 1.0f);

            vertices.emplace_back(x, y, z);
            normals.push_back(glm::normalize(glm::vec3(x, y, 0)));
        }
    }

    // construct indices
    for (unsigned int stack = 0; stack < stack_count - 1; ++stack) {
        for (unsigned int slice = 0; slice < slice_count; ++slice) {
            // point 1
            indices.push_back(stack * slice_count + slice);

            // point 4
            indices.push_back((stack + 1) * slice_count + slice);

            // point 3
            if (slice + 1 == slice_count) {
                indices.push_back((stack + 1) * slice_count);
            } else {
                indices.push_back((stack + 1) * slice_count + slice + 1);
            }

            // point 1
            indices.push_back(stack * slice_count + slice);

            // point 3
            if (slice + 1 == slice_count) {
                indices.push_back((stack + 1) * slice_count);
            } else {
                indices.push_back((stack + 1) * slice_count + slice + 1);
            }

            // point 2
            if (slice + 1 == slice_count) {
                indices.push_back(stack * slice_count);
            } else {
                indices.push_back(stack * slice_count + slice + 1);
            }
        }
    }

    return std::make_unique<Model>(vertices, normals, indices);
}
