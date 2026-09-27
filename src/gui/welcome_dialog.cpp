/**
 * Schoenflies
 * Copyright (c) 2026 Ivo Filot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "welcome_dialog.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "../program.h"

namespace {

QPushButton* make_action_button(
    const QString& title,
    const QString& description,
    const QString& icon_path,
    QWidget* parent
) {
    auto* button = new QPushButton(parent);
    button->setText(title + "\n" + description);
    button->setIcon(QIcon(icon_path));
    button->setIconSize(QSize(34, 34));
    button->setMinimumSize(250, 78);
    button->setMinimumWidth(qMax(button->fontMetrics().horizontalAdvance(title),
                                 button->fontMetrics().horizontalAdvance(description)) + 100);
    button->setCursor(Qt::PointingHandCursor);
    button->setStyleSheet(
        "QPushButton {"
        "  background: palette(button); color: palette(button-text); border: 1px solid palette(mid);"
        "  border-radius: 8px; padding: 10px 14px; text-align: left;"
        "  font-size: 12px;"
        "}"
        "QPushButton:hover { background: palette(midlight); border-color: palette(link); }"
        "QPushButton:pressed { background: palette(highlight); color: palette(highlighted-text); }"
    );
    return button;
}

}  // namespace

WelcomeDialog::WelcomeDialog(QWidget* parent): QDialog(parent) {
    setWindowTitle(tr("Welcome to " PROGRAM_NAME));
    setWindowIcon(QIcon(":/assets/icons/schoenflies-v2.ico"));
    setModal(true);
    setMinimumWidth(590);
    setStyleSheet("QDialog { background: palette(window); }");

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 24, 28, 22);
    layout->setSpacing(16);

    auto* heading_layout = new QHBoxLayout;
    auto* logo = new QLabel(this);
    logo->setPixmap(QIcon(":/assets/icons/schoenflies-v2.png").pixmap(72, 72));
    heading_layout->addWidget(logo, 0, Qt::AlignTop);

    auto* heading = new QLabel(
        tr("<span style='font-size:24px; font-weight:600;'>Welcome to %1</span>"
           "<br><span>Explore and understand molecular symmetry.</span>")
            .arg(PROGRAM_NAME),
        this
    );
    heading->setTextFormat(Qt::RichText);
    heading_layout->addWidget(heading, 1, Qt::AlignVCenter);
    layout->addLayout(heading_layout);

    auto* prompt = new QLabel(tr("What would you like to do?"), this);
    prompt->setStyleSheet("font-size: 14px; font-weight: 600; color: palette(window-text);");
    layout->addWidget(prompt);

    auto* actions = new QGridLayout;
    actions->setSpacing(12);

    auto* open_button = make_action_button(
        tr("Open a structure"), tr("Load an XYZ file from your computer"),
        ":/assets/icons/bluecurve/stock-open.svg", this);
    auto* library_button = make_action_button(
        tr("Explore the library"), tr("Choose from built-in molecules"),
        ":/assets/icons/bluecurve/folder-library.svg", this);
    auto* practice_button = make_action_button(
        tr("Start practicing"), tr("Try guided symmetry exercises"),
        ":/assets/icons/bluecurve/stock-execute.svg", this);
    auto* tables_button = make_action_button(
        tr("View character tables"), tr("Browse point-group reference data"),
        ":/assets/icons/bluecurve/stock-insert-table.svg", this);

    actions->addWidget(open_button, 0, 0);
    actions->addWidget(library_button, 0, 1);
    actions->addWidget(practice_button, 1, 0);
    actions->addWidget(tables_button, 1, 1);
    layout->addLayout(actions);

    auto* tip = new QLabel(
        tr("<b>Tip:</b> Drag to rotate a molecule, scroll to zoom, and select a "
           "symmetry operation to animate it. For a guided route to the point "
           "group, open a library molecule and choose <i>Determine point group</i>."),
        this
    );
    tip->setWordWrap(true);
    tip->setStyleSheet(
        "background: palette(alternate-base); color: palette(window-text); border-radius: 7px; padding: 12px;"
    );
    layout->addWidget(tip);

    auto* footer = new QHBoxLayout;
    auto* version = new QLabel(tr("Version %1").arg(PROGRAM_VERSION), this);
    version->setStyleSheet("color: palette(placeholder-text);");
    footer->addWidget(version);
    footer->addStretch();
    auto* dismiss_hint = new QLabel(tr("Press Esc to continue to the application"), this);
    dismiss_hint->setStyleSheet("color: palette(placeholder-text);");
    footer->addWidget(dismiss_hint);
    layout->addLayout(footer);

    connect(open_button, &QPushButton::clicked,
            this, [this]() { done(OpenStructure); });
    connect(library_button, &QPushButton::clicked,
            this, [this]() { done(BrowseLibrary); });
    connect(practice_button, &QPushButton::clicked,
            this, [this]() { done(StartPractice); });
    connect(tables_button, &QPushButton::clicked,
            this, [this]() { done(ViewCharacterTables); });
}
