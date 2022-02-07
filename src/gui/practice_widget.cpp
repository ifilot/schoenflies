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

#include "practice_widget.h"

/**
 * @brief Construct a new PracticeWidget object
 *
 * @param parent pointer to parent widget
 */
PracticeWidget::PracticeWidget(QWidget* parent) {
    this->setFrameShape(QFrame::NoFrame);

    this->flowchart_widget = new PracticeFlowchartWidget(this);
    this->addWidget(this->flowchart_widget);
}

/**
 * @brief Set the symmetry object
 *
 * @param symmetry
 */
void PracticeWidget::set_symmetry(const std::shared_ptr<Symmetry> symmetry) {
    this->symmetry = symmetry;

    this->flowchart_widget->initialize_flowchart(symmetry);
}
