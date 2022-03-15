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
    this->layout = new QVBoxLayout;
    this->layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    this->subwidgets = new QStackedWidget;
    this->subwidgets->setFrameShape(QFrame::NoFrame);
    this->layout->addWidget(this->subwidgets);

    this->config_widget = new PracticeConfigWidget(this);
    this->subwidgets->addWidget(this->config_widget);
    connect(this->config_widget, SIGNAL(start()), this, SLOT(start_practice()));

    this->flowchart_widget = new PracticeFlowchartWidget(this);
    this->subwidgets->addWidget(this->flowchart_widget);
    connect(this->flowchart_widget, SIGNAL(finished_exercise()), this, SLOT(finished_exercise()));

    this->buttons_widget = new QWidget;
    QHBoxLayout *buttons_layout = new QHBoxLayout;
    buttons_layout->setContentsMargins(0, 0, 0, 0);
    this->buttons_widget->setLayout(buttons_layout);
    this->buttons_widget->setVisible(false);
    this->layout->addWidget(buttons_widget);

    this->stop_button = new QPushButton;
    this->stop_button->setText("Stop");
    buttons_layout->addWidget(this->stop_button);
    connect(this->stop_button, SIGNAL(clicked()), this, SLOT(stop_practice()));

    this->next_button = new QPushButton;
    this->next_button->setText("Skip exercise");
    buttons_layout->addWidget(this->next_button);
    connect(this->next_button, SIGNAL(clicked()), this, SLOT(create_exercise()));
}

/**
 * @brief Set the symmetry object
 *
 * @param symmetry
 */
void PracticeWidget::set_symmetry(const std::shared_ptr<Symmetry> symmetry) {
    this->symmetry = symmetry;
}

/**
 * @brief Configure the practice module and start the practice session
 */
void PracticeWidget::start_practice() {
    this->practice_config = this->config_widget->get_practice_config();
    this->buttons_widget->setVisible(true);
    this->create_exercise();
}

/**
 * @brief Set the practice widget in a state where the exercise is finished
 */
void PracticeWidget::finished_exercise() {
    this->next_button->setText("Next exercise");
}

/**
 * @brief Stop the current practice session
 */
void PracticeWidget::stop_practice() {
    this->buttons_widget->setVisible(false);
    this->subwidgets->setCurrentWidget(this->config_widget);
}

/**
 * @brief Create and show a new exercise
 */
void PracticeWidget::create_exercise() {
    this->next_button->setText("Skip exercise");

    PracticeModule practice_module = this->practice_config->get_next_module();
    if (this->practice_config->get_select_new_structure()) {
        emit request_new_structure();
    }

    switch (practice_module) {
        case PracticeModule::Flowchart:
            this->flowchart_widget->initialize_flowchart(this->symmetry);
            this->subwidgets->setCurrentWidget(this->flowchart_widget);
            break;
        default:
            std::runtime_error("Invalid practice module encountered.");
    }
}
