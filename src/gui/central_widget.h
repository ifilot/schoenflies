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

#ifndef GUI_CENTRAL_WIDGET_H
#define GUI_CENTRAL_WIDGET_H

#include <memory>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QSizePolicy>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QTreeView>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>
#include "../structure.h"
#include "../symmetry/symmetry.h"
#include "gl_widget.h"
#include "main_window.h"
#include "symmetry_operation_item_delegate.h"

class MainWindow;  // forward declaration

class CentralWidget: public QWidget {
    Q_OBJECT

private:
    GLWidget *gl_widget;
    QTextEdit *text_edit;

    QStandardItemModel *model;

    std::shared_ptr<Structure> structure;

public:
    /**
     * @brief Construct a new Central Widget object
     *
     * @param mw pointer to MainWindow object
     */
    CentralWidget(MainWindow* mw);

    /**
     * @brief Set the structure object
     *
     * @param structure
     */
    void set_structure(std::shared_ptr<Structure> structure);

private slots:
    /**
     * @brief Trigger the animation of a symmetry operation in the GL widget
     *
     * @param index index of symmetry operation in the data model
     */
    void trigger_animation(QModelIndex index);
};

#endif  // GUI_CENTRAL_WIDGET_H
