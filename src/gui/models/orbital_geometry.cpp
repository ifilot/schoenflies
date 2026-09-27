/**
 * Schoenflies
 * Copyright (c) 2026 Ivo Filot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "geometry.h"

#include <algorithm>
#include <stdexcept>

std::unique_ptr<Model3D> Geometry::orbital(
    OrbitalType type,
    bool positive_phase,
    unsigned int latitude_count,
    unsigned int longitude_count) {
    if (type == OrbitalType::None) {
        throw std::invalid_argument("Cannot generate geometry for the None orbital.");
    }
    if (latitude_count < 4 || longitude_count < 8) {
        throw std::invalid_argument("Orbital geometry resolution is too low.");
    }

    auto direction = [latitude_count, longitude_count](unsigned int latitude, unsigned int longitude) {
        const float theta = (float) std::numbers::pi * latitude / latitude_count;
        const float phi = 2.0f * (float) std::numbers::pi * longitude / longitude_count;
        return glm::vec3(
            std::sin(theta) * std::cos(phi),
            std::sin(theta) * std::sin(phi),
            std::cos(theta));
    };

    float maximum = 0.0f;
    for (unsigned int latitude = 0; latitude <= latitude_count; ++latitude) {
        for (unsigned int longitude = 0; longitude < longitude_count; ++longitude) {
            maximum = std::max(maximum, std::fabs(orbital_angular_value(
                type, direction(latitude, longitude))));
        }
    }

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    constexpr float node_epsilon = 1.0e-6f;
    constexpr float area_epsilon_squared = 1.0e-12f;

    auto append_triangle = [&](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        const float va = orbital_angular_value(type, a);
        const float vb = orbital_angular_value(type, b);
        const float vc = orbital_angular_value(type, c);
        const bool phase_matches = positive_phase
            ? va > node_epsilon && vb > node_epsilon && vc > node_epsilon
            : va < -node_epsilon && vb < -node_epsilon && vc < -node_epsilon;
        if (!phase_matches) return;

        auto surface_position = [maximum](const glm::vec3& d, float value) {
            const float radius = std::sqrt(std::fabs(value) / maximum);
            return radius * d;
        };

        glm::vec3 pa = surface_position(a, va);
        glm::vec3 pb = surface_position(b, vb);
        glm::vec3 pc = surface_position(c, vc);
        glm::vec3 face_normal = glm::cross(pb - pa, pc - pa);
        if (glm::dot(face_normal, face_normal) < area_epsilon_squared) return;

        if (glm::dot(face_normal, pa + pb + pc) < 0.0f) {
            std::swap(pb, pc);
        }

        const unsigned int first = vertices.size();
        vertices.insert(vertices.end(), {pa, pb, pc});
        normals.insert(normals.end(), {
            glm::normalize(pa), glm::normalize(pb), glm::normalize(pc)});
        indices.insert(indices.end(), {first, first + 1, first + 2});
    };

    for (unsigned int latitude = 0; latitude < latitude_count; ++latitude) {
        for (unsigned int longitude = 0; longitude < longitude_count; ++longitude) {
            const unsigned int next_longitude = (longitude + 1) % longitude_count;
            const glm::vec3 a = direction(latitude, longitude);
            const glm::vec3 b = direction(latitude + 1, longitude);
            const glm::vec3 c = direction(latitude + 1, next_longitude);
            const glm::vec3 d = direction(latitude, next_longitude);
            append_triangle(a, b, c);
            append_triangle(a, c, d);
        }
    }

    return std::make_unique<Model3D>(vertices, normals, indices);
}
