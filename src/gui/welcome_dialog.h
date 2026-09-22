/**
 * Schoenflies
 * Copyright (c) 2026 Ivo Filot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef GUI_WELCOME_DIALOG_H
#define GUI_WELCOME_DIALOG_H

#include <QDialog>

class WelcomeDialog: public QDialog {
    Q_OBJECT

public:
    enum Action {
        None = QDialog::Rejected,
        OpenStructure = 1,
        BrowseLibrary,
        StartPractice,
        ViewCharacterTables
    };

    explicit WelcomeDialog(QWidget* parent = nullptr);
};

#endif  // GUI_WELCOME_DIALOG_H
