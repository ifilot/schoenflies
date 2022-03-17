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

#ifndef GUI_PRACTICE_IRREPS_WIDGET_H
#define GUI_PRACTICE_IRREPS_WIDGET_H

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <Qt>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include "../practice/practice_irreps.h"
#include "../symmetry/symmetry.h"

class PracticeIrrepsWidget: public QScrollArea {
    Q_OBJECT

private:
    QVBoxLayout *layout;
    QWidget *container;

    QWidget *part1;
    QWidget *part2;

    QLabel *q1;

    QGridLayout *operation_characters_grid;
    QPushButton *submit_operation_characters;

    QGridLayout *irrep_coefficients_grid;
    QLabel *irrep_sum;
    QPushButton *submit_irrep_coefficients;

    PracticeIrreps irreps;

public:
    /**
     * @brief Construct a new PracticeIrrepsWidget object
     *
     * @param parent pointer to parent widget
     */
    PracticeIrrepsWidget(QWidget* parent);

    /**
     * @brief Initialize a new exercise
     *
     * @param symmetry
     */
    void initialize(std::shared_ptr<Symmetry> symmetry);

private slots:
    /**
     * @brief Scroll the widget to the bottom
     */
    void scroll_to_bottom();

    /**
     * @brief Show the second part of the exercise
     */
    void show_part2();

    /**
     * @brief Show the correct answers
     */
    void show_answers();

    /**
     * @brief Update the label showing the sum of the irreps
     */
    void update_irrep_sum();

signals:
    /**
     * @brief Emitted when the user has finished the exercise
     */
    void finished_exercise();
};

#endif  // GUI_PRACTICE_IRREPS_WIDGET_H
