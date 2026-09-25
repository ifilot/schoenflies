/**
 * Schoenflies
 * Copyright (c) 2026 Ivo Filot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "orbital_type.h"

#include <stdexcept>
#include <unordered_map>

namespace {
const std::vector<OrbitalType> types = {
    OrbitalType::S,
    OrbitalType::Px, OrbitalType::Py, OrbitalType::Pz,
    OrbitalType::Dxy, OrbitalType::Dxz, OrbitalType::Dyz,
    OrbitalType::Dx2Y2, OrbitalType::Dz2,
    OrbitalType::Fxyz, OrbitalType::FzX2Y2,
    OrbitalType::FxX2_3Y2, OrbitalType::Fy3X2_Y2,
    OrbitalType::FxZ2, OrbitalType::FyZ2, OrbitalType::Fz3
};

const std::unordered_map<OrbitalType, std::string> names = {
    {OrbitalType::None, "None"},
    {OrbitalType::S, "1s"},
    {OrbitalType::Px, "2px"},
    {OrbitalType::Py, "2py"},
    {OrbitalType::Pz, "2pz"},
    {OrbitalType::Dxy, "3dxy"},
    {OrbitalType::Dxz, "3dxz"},
    {OrbitalType::Dyz, "3dyz"},
    {OrbitalType::Dx2Y2, "3d(x^2-y^2)"},
    {OrbitalType::Dz2, "3dz^2"},
    {OrbitalType::Fxyz, "4fxyz"},
    {OrbitalType::FzX2Y2, "4fz(x^2-y^2)"},
    {OrbitalType::FxX2_3Y2, "4fx(x^2-3y^2)"},
    {OrbitalType::Fy3X2_Y2, "4fy(3x^2-y^2)"},
    {OrbitalType::FxZ2, "4fxz^2"},
    {OrbitalType::FyZ2, "4fyz^2"},
    {OrbitalType::Fz3, "4fz^3"}
};

const std::unordered_map<OrbitalType, std::string> model_names = {
    {OrbitalType::S, "orbital_s"},
    {OrbitalType::Px, "orbital_px"},
    {OrbitalType::Py, "orbital_py"},
    {OrbitalType::Pz, "orbital_pz"},
    {OrbitalType::Dxy, "orbital_dxy"},
    {OrbitalType::Dxz, "orbital_dxz"},
    {OrbitalType::Dyz, "orbital_dyz"},
    {OrbitalType::Dx2Y2, "orbital_dx2_y2"},
    {OrbitalType::Dz2, "orbital_dz2"},
    {OrbitalType::Fxyz, "orbital_fxyz"},
    {OrbitalType::FzX2Y2, "orbital_fz_x2_y2"},
    {OrbitalType::FxX2_3Y2, "orbital_fx_x2_3y2"},
    {OrbitalType::Fy3X2_Y2, "orbital_fy_3x2_y2"},
    {OrbitalType::FxZ2, "orbital_fxz2"},
    {OrbitalType::FyZ2, "orbital_fyz2"},
    {OrbitalType::Fz3, "orbital_fz3"}
};
}

const std::vector<OrbitalType>& orbital_types() {
    return types;
}

const std::string& orbital_type_name(OrbitalType type) {
    return names.at(type);
}

const std::string& orbital_type_model_name(OrbitalType type) {
    if (type == OrbitalType::None) {
        throw std::invalid_argument("The None orbital has no model.");
    }
    return model_names.at(type);
}

float orbital_angular_value(OrbitalType type, const glm::vec3& direction) {
    const float x = direction.x;
    const float y = direction.y;
    const float z = direction.z;

    switch (type) {
        case OrbitalType::None: return 0.0f;
        case OrbitalType::S: return 1.0f;
        case OrbitalType::Px: return x;
        case OrbitalType::Py: return y;
        case OrbitalType::Pz: return z;
        case OrbitalType::Dxy: return 2.0f * x * y;
        case OrbitalType::Dxz: return 2.0f * x * z;
        case OrbitalType::Dyz: return 2.0f * y * z;
        case OrbitalType::Dx2Y2: return x * x - y * y;
        case OrbitalType::Dz2: return 0.5f * (3.0f * z * z - 1.0f);
        case OrbitalType::Fxyz: return x * y * z;
        case OrbitalType::FzX2Y2: return z * (x * x - y * y);
        case OrbitalType::FxX2_3Y2: return x * (x * x - 3.0f * y * y);
        case OrbitalType::Fy3X2_Y2: return y * (3.0f * x * x - y * y);
        case OrbitalType::FxZ2: return x * (5.0f * z * z - 1.0f);
        case OrbitalType::FyZ2: return y * (5.0f * z * z - 1.0f);
        case OrbitalType::Fz3: return z * (5.0f * z * z - 3.0f);
    }

    throw std::invalid_argument("Unknown orbital type.");
}

unsigned int orbital_angular_momentum(OrbitalType type) {
    switch (type) {
        case OrbitalType::S: return 0;
        case OrbitalType::Px:
        case OrbitalType::Py:
        case OrbitalType::Pz: return 1;
        case OrbitalType::Dxy:
        case OrbitalType::Dxz:
        case OrbitalType::Dyz:
        case OrbitalType::Dx2Y2:
        case OrbitalType::Dz2: return 2;
        case OrbitalType::Fxyz:
        case OrbitalType::FzX2Y2:
        case OrbitalType::FxX2_3Y2:
        case OrbitalType::Fy3X2_Y2:
        case OrbitalType::FxZ2:
        case OrbitalType::FyZ2:
        case OrbitalType::Fz3: return 3;
        case OrbitalType::None:
            throw std::invalid_argument("The None orbital has no angular momentum.");
    }
    throw std::invalid_argument("Unknown orbital type.");
}
