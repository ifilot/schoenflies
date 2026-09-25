/**
 * Schoenflies
 * Copyright (c) 2026 Ivo Filot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ORBITALS_ORBITAL_TYPE_H
#define ORBITALS_ORBITAL_TYPE_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

enum class OrbitalType {
    None,
    S,
    Px,
    Py,
    Pz,
    Dxy,
    Dxz,
    Dyz,
    Dx2Y2,
    Dz2,
    Fxyz,
    FzX2Y2,
    FxX2_3Y2,
    Fy3X2_Y2,
    FxZ2,
    FyZ2,
    Fz3
};

using OrbitalAssignments = std::vector<std::vector<OrbitalType>>;

/** Return all visualisable orbitals, ordered by angular momentum. */
const std::vector<OrbitalType>& orbital_types();

/** Return the conventional lowest-shell label for an orbital. */
const std::string& orbital_type_name(OrbitalType type);

/** Return a stable name suitable for use as an OpenGL model key. */
const std::string& orbital_type_model_name(OrbitalType type);

/** Evaluate the real angular part of an orbital on a unit direction. */
float orbital_angular_value(OrbitalType type, const glm::vec3& direction);

/** Return the angular-momentum quantum number (0=s, 1=p, 2=d, 3=f). */
unsigned int orbital_angular_momentum(OrbitalType type);

#endif  // ORBITALS_ORBITAL_TYPE_H
