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

#ifndef SYMMETRY_POINT_GROUPS_POINT_GROUPS_H
#define SYMMETRY_POINT_GROUPS_POINT_GROUPS_H

#include <vector>
#include "../operations/operation.h"
#include "../operations/operation_label.h"
#include "point_group.h"
#include "point_group_label.h"

typedef PointGroupLabel Label;
typedef PointGroupLabel::Class Class;
typedef OperationLabel O;
typedef OperationLabel::Element E;
typedef OperationLabel::Plane Plane;
typedef OperationLabel::Prime Prime;

class PointGroups final {
public:
    /**
     * @brief List of supported point groups
     */
    static const std::vector<PointGroup> point_groups;
};

#endif  // SYMMETRY_POINT_GROUPS_POINT_GROUPS_H
