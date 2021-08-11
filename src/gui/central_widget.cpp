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

    GLWidget *gl_widget = new GLWidget(this);
    gl_widget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    layout->addWidget(gl_widget);

    // TODO text edit is temporary
    this->text_edit = new QTextEdit();
    this->text_edit->setReadOnly(true);
    this->text_edit->setPlainText("Hello, World!");
    layout->addWidget(this->text_edit);

    this->setLayout(layout);
}

/**
 * @brief Set the structure object
 *
 * @param structure
 */
void CentralWidget::set_structure(std::shared_ptr<Structure> structure) {
    this->structure = structure;

    this->text_edit->setPlainText(QString::fromStdString(this->structure->get_description()));
}
