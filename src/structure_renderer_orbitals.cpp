/**
 * Schoenflies
 * Copyright (c) 2026 Ivo Filot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "structure_renderer.h"

#include <algorithm>
#include <unordered_set>

namespace {
float orbital_scale_for(const Element& element, OrbitalType type) {
    static const float shell_scale[] = {0.84f, 0.98f, 1.10f, 1.22f};
    const float atom_scale = std::clamp(1.45f * element.radius, 0.75f, 1.55f);
    return atom_scale * shell_scale[orbital_angular_momentum(type)];
}
}

std::vector<ModelInstance> StructureRenderer::get_orbital_model_instances() {
    std::vector<ModelInstance> model_instances;
    if (!this->structure_set) return model_instances;

    const glm::vec4 positive_colour(this->orbital_positive_colour, 0.90f);
    const glm::vec4 negative_colour(this->orbital_negative_colour, 0.90f);

    for (unsigned int i = 0; i < this->structure->get_num_atoms(); ++i) {
        const Element element = PeriodicTable::get_element(this->structure->get_atomic_number(i));
        const glm::vec3 coordinates = this->animation_matrix * this->structure->get_coordinates(i);
        const glm::mat3x3 orientation = this->animation_matrix * this->orbital_orientations[i];

        for (OrbitalType type : this->orbitals[i]) {
            glm::mat4x4 transform = glm::translate(this->base_matrix(), coordinates)
                * glm::mat4x4(orientation);
            transform = glm::scale(transform, glm::vec3(orbital_scale_for(element, type)));

            const std::string model_name = orbital_type_model_name(type);
            model_instances.push_back({model_name + "_positive", transform, positive_colour, -1});
            if (type != OrbitalType::S) {
                model_instances.push_back({model_name + "_negative", transform, negative_colour, -1});
            }
        }
    }

    return model_instances;
}

std::vector<ModelInstance> StructureRenderer::get_ghost_orbital_model_instances() {
    std::vector<ModelInstance> model_instances;
    if (!this->structure_set || !this->animating) return model_instances;

    const glm::vec4 positive_colour(this->orbital_positive_colour, 0.22f);
    const glm::vec4 negative_colour(this->orbital_negative_colour, 0.22f);

    for (unsigned int i = 0; i < this->structure->get_num_atoms(); ++i) {
        const Element element = PeriodicTable::get_element(this->structure->get_atomic_number(i));
        const glm::vec3 coordinates = this->structure->get_coordinates(i);

        for (OrbitalType type : this->orbitals[i]) {
            glm::mat4x4 transform = glm::translate(this->base_matrix(), coordinates)
                * glm::mat4x4(this->orbital_orientations[i]);
            transform = glm::scale(transform, glm::vec3(orbital_scale_for(element, type)));

            const std::string model_name = orbital_type_model_name(type);
            model_instances.push_back({model_name + "_positive", transform, positive_colour, -1});
            if (type != OrbitalType::S) {
                model_instances.push_back({model_name + "_negative", transform, negative_colour, -1});
            }
        }
    }

    return model_instances;
}

void StructureRenderer::set_orbitals(const OrbitalAssignments& orbitals) {
    if (!this->structure_set) throw std::runtime_error("No structure set.");
    if (orbitals.size() != this->structure->get_num_atoms()) {
        throw std::invalid_argument("Orbital assignment count does not match atom count.");
    }

    for (const std::vector<OrbitalType>& atom_orbitals : orbitals) {
        std::unordered_set<OrbitalType> unique;
        for (OrbitalType type : atom_orbitals) {
            if (type == OrbitalType::None || !unique.insert(type).second) {
                throw std::invalid_argument("Orbital assignments must be unique, visualisable orbitals.");
            }
        }
    }

    this->orbitals = orbitals;
    this->orbital_orientations.assign(orbitals.size(), this->orbital_base_orientation);
    emit this->update();
}

const OrbitalAssignments& StructureRenderer::get_orbitals() const {
    return this->orbitals;
}

void StructureRenderer::set_orbital_colours(
    const glm::vec3& positive,
    const glm::vec3& negative) {
    this->orbital_positive_colour = glm::clamp(positive, glm::vec3(0.0f), glm::vec3(1.0f));
    this->orbital_negative_colour = glm::clamp(negative, glm::vec3(0.0f), glm::vec3(1.0f));
    emit this->update();
}

const glm::vec3& StructureRenderer::get_orbital_positive_colour() const {
    return this->orbital_positive_colour;
}

const glm::vec3& StructureRenderer::get_orbital_negative_colour() const {
    return this->orbital_negative_colour;
}

void StructureRenderer::apply_operation_to_orbitals(Operation operation) {
    OrbitalAssignments transformed_orbitals(this->orbitals.size());
    std::vector<glm::mat3x3> transformed_orientations(
        this->orbital_orientations.size(), glm::mat3x3(1.0f));
    const glm::mat3x3 operation_matrix = operation.calculate_fractional_matrix(1.0);

    for (unsigned int i = 0; i < this->orbitals.size(); ++i) {
        const unsigned int result_index = operation.get_result_index(i);
        transformed_orbitals[result_index] = this->orbitals[i];
        transformed_orientations[result_index] = operation_matrix * this->orbital_orientations[i];
    }

    this->orbitals = std::move(transformed_orbitals);
    this->orbital_orientations = std::move(transformed_orientations);
}
