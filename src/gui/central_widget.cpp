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

#include "central_widget.h"

/**
 * @brief Construct a new Central Widget object
 *
 * @param mw pointer to MainWindow object
 */
CentralWidget::CentralWidget(MainWindow* mw) {
    QHBoxLayout *layout = new QHBoxLayout;
    this->setLayout(layout);

    this->gl_widget = new GLWidget(this);
    this->gl_widget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    layout->addWidget(this->gl_widget, 3);

    this->side_widget = new QStackedWidget(this);
    this->side_widget->setFrameShape(QFrame::NoFrame);
    layout->addWidget(this->side_widget, 1);

    this->symmetry_widget = new SymmetryWidget(this);
    side_widget->addWidget(this->symmetry_widget);

    this->practice_widget = new PracticeWidget(this);
    side_widget->addWidget(this->practice_widget);

    this->library = std::make_shared<Library>();

    connect(this->symmetry_widget, SIGNAL(animate_operation(Operation)), this->gl_widget, SLOT(start_animation(Operation)));
    connect(this->symmetry_widget, SIGNAL(operation_changed(bool, Operation)), this->gl_widget, SLOT(set_operation(bool, Operation)));
    connect(this->gl_widget, SIGNAL(animation_finished()), this->symmetry_widget, SLOT(request_operation()));
}

/**
 * @brief Set the structure object
 *
 * @param structure
 */
void CentralWidget::set_structure(std::shared_ptr<Structure> structure) {
    this->structure = structure;
    auto symmetry = std::make_shared<Symmetry>(structure);
    this->symmetry = symmetry;

    this->gl_widget->set_structure(structure);
    this->gl_widget->set_structure_rotation(symmetry->get_cartesian_axes());
    this->gl_widget->reset_camera();
    this->gl_widget->unset_operation();

    this->symmetry_widget->set_symmetry(symmetry);
    this->practice_widget->set_symmetry(symmetry);
}

void CentralWidget::set_gui_mode(QAction* action) {
    this->gui_mode = action->data().value<GuiMode>();

    int index;
    switch (this->gui_mode) {
        case GuiMode::SymmetryViewer:
            index = this->side_widget->indexOf(this->symmetry_widget);
            break;
        case GuiMode::Practice:
            index = this->side_widget->indexOf(this->practice_widget);
            break;
        default:
            throw std::runtime_error("Unexpected GUI mode encountered.");
    }
    this->side_widget->setCurrentIndex(index);
}

/**
 * @brief Get the GL widget object
 *
 * @return GLWidget*
 */
GLWidget* CentralWidget::get_gl_widget() {
    return this->gl_widget;
}

/**
 * @brief Get the library object
 *
 * @return std::shared_ptr<Library>&
 */
std::shared_ptr<Library>& CentralWidget::get_library() {
    return this->library;
}
