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

#ifndef GUI_MAIN_WINDOW_H
#define GUI_MAIN_WINDOW_H

#include <QAction>
#include <QApplication>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include "../program.h"

class MainWindow: public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Construct a new Main Window object
     */
    MainWindow();

private slots:
    /**
     * @brief Open a new file
     */
    void open();

    /**
     * @brief Close the application
     */
    void exit();

    /**
     * @brief Display about menu
     */
    void about();
};

#endif  // GUI_MAIN_WINDOW_H
