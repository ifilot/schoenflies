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

#include "main_window.h"

/**
 * @brief Construct a new Main Window object
 */
MainWindow::MainWindow() {
    this->setWindowTitle(QString(PROGRAM_NAME));

    // menu bar
    QMenuBar *menu_bar = new QMenuBar;

    // drop-down menus
    QMenu *menu_file = menu_bar->addMenu(tr("&File"));
    QMenu *menu_help = menu_bar->addMenu(tr("&Help"));

    // actions for file menu
    QAction *action_open = new QAction(menu_file);
    action_open->setText(tr("Open"));
    action_open->setShortcuts(QKeySequence::Open);
    connect(action_open, &QAction::triggered, this, &MainWindow::open);
    menu_file->addAction(action_open);

    QAction *action_exit = new QAction(menu_file);
    action_exit->setText(tr("Exit"));
    action_exit->setShortcuts(QKeySequence::Quit);
    connect(action_exit, &QAction::triggered, this, &MainWindow::exit);
    menu_file->addAction(action_exit);

    // actions for help menu
    QAction *action_about = new QAction(menu_help);
    action_about->setText(tr("About"));
    connect(action_about, &QAction::triggered, this, &MainWindow::about);
    menu_help->addAction(action_about);

    setMenuBar(menu_bar);

    // central widget
    CentralWidget* central_widget = new CentralWidget(this);
    setCentralWidget(central_widget);
}

/**
 * @brief Open a new file
 */
void MainWindow::open() {
    // TODO add action
}

/**
 * @brief Close the application
 */
void MainWindow::exit() {
    QApplication::quit();
}

/**
 * @brief Display about menu
 */
void MainWindow::about() {
    QMessageBox message_box;
    message_box.setText(PROGRAM_NAME ": " PROGRAM_DESC ", version " PROGRAM_VERSION "\n"
                        "Copyright (c) 2021 Luuk Kempen\n"
                        PROGRAM_NAME " is licensed under the GPLv3 license.");
    message_box.setWindowTitle("About " PROGRAM_NAME);
    message_box.exec();
}
