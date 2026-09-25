/**
 * Schoenflies
 * Copyright (c) 2026 Ivo Filot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "orbital_dialog.h"

#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <QAbstractItemView>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QScrollArea>
#include <QStringList>
#include <QVBoxLayout>
#include "../periodic_table/periodic_table.h"

namespace {
QColor to_qcolor(const glm::vec3& colour) {
    return QColor::fromRgbF(colour.r, colour.g, colour.b);
}

glm::vec3 to_glm(const QColor& colour) {
    return glm::vec3(colour.redF(), colour.greenF(), colour.blueF());
}

bool contains_orbital(const std::vector<OrbitalType>& orbitals, OrbitalType type) {
    return std::find(orbitals.begin(), orbitals.end(), type) != orbitals.end();
}
}

OrbitalDialog::OrbitalDialog(
    const std::shared_ptr<Structure>& structure,
    const std::shared_ptr<StructureRenderer>& renderer,
    QWidget* parent)
    : QDialog(parent), structure(structure), renderer(renderer) {
    if (!this->structure || !this->renderer) {
        throw std::invalid_argument("A structure and renderer are required.");
    }

    this->original_assignments = this->renderer->get_orbitals();
    this->assignments = this->original_assignments;
    this->original_positive_colour = this->renderer->get_orbital_positive_colour();
    this->original_negative_colour = this->renderer->get_orbital_negative_colour();
    this->positive_colour = to_qcolor(this->original_positive_colour);
    this->negative_colour = to_qcolor(this->original_negative_colour);

    this->setWindowTitle(tr("Atomic orbital basis functions"));
    this->setModal(true);
    this->resize(980, 680);

    QVBoxLayout* main_layout = new QVBoxLayout(this);
    QLabel* instructions = new QLabel(
        tr("Select one or more atoms, then toggle any number of real basis functions. "
           "Changes are previewed immediately. The x, y, and z directions follow "
           "the Cartesian axes shown in the viewer."), this);
    instructions->setWordWrap(true);
    main_layout->addWidget(instructions);

    QHBoxLayout* content_layout = new QHBoxLayout;
    main_layout->addLayout(content_layout, 1);

    QWidget* atom_panel = new QWidget(this);
    QVBoxLayout* atom_layout = new QVBoxLayout(atom_panel);
    atom_layout->setContentsMargins(0, 0, 0, 0);
    QLabel* atom_label = new QLabel(tr("1. Select atoms"), atom_panel);
    QFont heading_font = atom_label->font();
    heading_font.setBold(true);
    atom_label->setFont(heading_font);
    atom_layout->addWidget(atom_label);

    this->atom_table = new QTableWidget(this->structure->get_num_atoms(), 2, atom_panel);
    this->atom_table->setHorizontalHeaderLabels({tr("Atom"), tr("Assigned basis functions")});
    this->atom_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->atom_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->atom_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->atom_table->verticalHeader()->setVisible(false);
    this->atom_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    this->atom_table->horizontalHeader()->setStretchLastSection(true);

    std::unordered_map<unsigned int, unsigned int> element_counts;
    for (unsigned int i = 0; i < this->structure->get_num_atoms(); ++i) {
        const unsigned int atomic_number = this->structure->get_atomic_number(i);
        const Element element = PeriodicTable::get_element(atomic_number);
        const unsigned int element_index = ++element_counts[atomic_number];
        QTableWidgetItem* atom_item = new QTableWidgetItem(
            QString::fromStdString(element.symbol + std::to_string(element_index)));
        atom_item->setTextAlignment(Qt::AlignCenter);
        this->atom_table->setItem(i, 0, atom_item);
        this->atom_table->setItem(i, 1, new QTableWidgetItem);
    }
    atom_layout->addWidget(this->atom_table, 1);

    QHBoxLayout* atom_buttons = new QHBoxLayout;
    QPushButton* select_all_button = new QPushButton(tr("Select all atoms"), atom_panel);
    QPushButton* clear_button = new QPushButton(tr("Clear selected"), atom_panel);
    atom_buttons->addWidget(select_all_button);
    atom_buttons->addWidget(clear_button);
    atom_layout->addLayout(atom_buttons);
    content_layout->addWidget(atom_panel, 5);

    QScrollArea* settings_scroll = new QScrollArea(this);
    settings_scroll->setWidgetResizable(true);
    settings_scroll->setFrameShape(QFrame::NoFrame);
    QWidget* settings_panel = new QWidget(settings_scroll);
    QVBoxLayout* settings_layout = new QVBoxLayout(settings_panel);
    settings_layout->setContentsMargins(4, 0, 4, 0);

    QLabel* orbital_label = new QLabel(tr("2. Choose basis functions"), settings_panel);
    orbital_label->setFont(heading_font);
    settings_layout->addWidget(orbital_label);
    settings_layout->addWidget(this->create_orbital_group(
        tr("s orbitals"), {OrbitalType::S}, settings_panel));
    settings_layout->addWidget(this->create_orbital_group(
        tr("p orbitals"), {OrbitalType::Px, OrbitalType::Py, OrbitalType::Pz}, settings_panel));
    settings_layout->addWidget(this->create_orbital_group(
        tr("d orbitals"), {OrbitalType::Dxy, OrbitalType::Dxz, OrbitalType::Dyz,
            OrbitalType::Dx2Y2, OrbitalType::Dz2}, settings_panel));
    settings_layout->addWidget(this->create_orbital_group(
        tr("f orbitals"), {OrbitalType::Fxyz, OrbitalType::FzX2Y2,
            OrbitalType::FxX2_3Y2, OrbitalType::Fy3X2_Y2,
            OrbitalType::FxZ2, OrbitalType::FyZ2, OrbitalType::Fz3}, settings_panel));

    QGroupBox* colour_group = new QGroupBox(tr("3. Phase colors"), settings_panel);
    QVBoxLayout* colour_layout = new QVBoxLayout(colour_group);
    colour_layout->addWidget(this->create_colour_row(true, colour_group));
    colour_layout->addWidget(this->create_colour_row(false, colour_group));
    settings_layout->addWidget(colour_group);
    settings_layout->addStretch();
    settings_scroll->setWidget(settings_panel);
    content_layout->addWidget(settings_scroll, 6);

    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    main_layout->addWidget(buttons);

    connect(this->atom_table->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this]() { this->update_checkbox_states(); });
    connect(select_all_button, &QPushButton::clicked, this, [this]() {
        this->atom_table->selectAll();
        this->update_checkbox_states();
    });
    connect(clear_button, &QPushButton::clicked, this, [this]() { this->clear_selected_atoms(); });
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &OrbitalDialog::reject);

    this->update_assignment_summaries();
    if (this->atom_table->rowCount() > 0) this->atom_table->selectRow(0);
    this->update_checkbox_states();
}

void OrbitalDialog::reject() {
    this->renderer->set_orbitals(this->original_assignments);
    this->renderer->set_orbital_colours(
        this->original_positive_colour, this->original_negative_colour);
    QDialog::reject();
}

QGroupBox* OrbitalDialog::create_orbital_group(
    const QString& title,
    const std::vector<OrbitalType>& types,
    QWidget* parent) {
    QGroupBox* group = new QGroupBox(title, parent);
    QGridLayout* layout = new QGridLayout(group);
    const int columns = types.size() > 3 ? 2 : 3;

    for (unsigned int i = 0; i < types.size(); ++i) {
        const OrbitalType type = types[i];
        QCheckBox* checkbox = new QCheckBox(
            QString::fromStdString(orbital_type_name(type)), group);
        checkbox->setTristate(true);
        this->orbital_checkboxes[type] = checkbox;
        layout->addWidget(checkbox, i / columns, i % columns);
        connect(checkbox, &QCheckBox::checkStateChanged, this, [this, type](Qt::CheckState state) {
            if (this->updating_controls) return;
            this->set_orbital_checked(type, state != Qt::Unchecked);
        });
    }
    return group;
}

QWidget* OrbitalDialog::create_colour_row(bool positive, QWidget* parent) {
    QWidget* row = new QWidget(parent);
    QHBoxLayout* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel(positive ? tr("Positive") : tr("Negative"), row));

    QPushButton* custom_button = new QPushButton(tr("Custom…"), row);
    custom_button->setMinimumWidth(90);
    if (positive) this->positive_colour_button = custom_button;
    else this->negative_colour_button = custom_button;
    this->update_colour_button(custom_button, positive ? this->positive_colour : this->negative_colour);
    connect(custom_button, &QPushButton::clicked, this, [this, positive]() {
        this->choose_colour(positive);
    });
    layout->addWidget(custom_button);

    const std::vector<QColor> swatches = {
        QColor("#1769e0"), QColor("#e53924"), QColor("#00a878"),
        QColor("#8e44ad"), QColor("#ff8c00"), QColor("#00a6d6"),
        QColor("#d81b60"), QColor("#f4b400")
    };
    for (const QColor& colour : swatches) {
        QPushButton* swatch = new QPushButton(row);
        swatch->setFixedSize(24, 24);
        swatch->setToolTip(colour.name(QColor::HexRgb));
        swatch->setStyleSheet(QString(
            "QPushButton { background: %1; border: 1px solid #555; border-radius: 3px; } "
            "QPushButton:hover { border: 2px solid white; }").arg(colour.name()));
        connect(swatch, &QPushButton::clicked, this, [this, positive, colour]() {
            this->set_colour(positive, colour);
        });
        layout->addWidget(swatch);
    }
    layout->addStretch();
    return row;
}

std::vector<int> OrbitalDialog::selected_atom_rows() const {
    std::vector<int> rows;
    for (const QModelIndex& index : this->atom_table->selectionModel()->selectedRows()) {
        rows.push_back(index.row());
    }
    if (rows.empty() && this->atom_table->currentRow() >= 0) {
        rows.push_back(this->atom_table->currentRow());
    }
    return rows;
}

void OrbitalDialog::update_checkbox_states() {
    const std::vector<int> rows = this->selected_atom_rows();
    this->updating_controls = true;
    for (const auto& [type, checkbox] : this->orbital_checkboxes) {
        unsigned int count = 0;
        for (int row : rows) {
            if (contains_orbital(this->assignments[row], type)) ++count;
        }

        Qt::CheckState state = Qt::Unchecked;
        if (!rows.empty() && count == rows.size()) state = Qt::Checked;
        else if (count > 0) state = Qt::PartiallyChecked;
        checkbox->setCheckState(state);
        checkbox->setEnabled(!rows.empty());
    }
    this->updating_controls = false;
}

void OrbitalDialog::update_assignment_summaries() {
    for (unsigned int row = 0; row < this->assignments.size(); ++row) {
        QStringList labels;
        for (OrbitalType type : orbital_types()) {
            if (contains_orbital(this->assignments[row], type)) {
                labels << QString::fromStdString(orbital_type_name(type));
            }
        }
        this->atom_table->item(row, 1)->setText(
            labels.empty() ? tr("None") : labels.join(", "));
    }
}

void OrbitalDialog::set_orbital_checked(OrbitalType type, bool checked) {
    for (int row : this->selected_atom_rows()) {
        std::vector<OrbitalType>& atom_orbitals = this->assignments[row];
        const auto found = std::find(atom_orbitals.begin(), atom_orbitals.end(), type);
        if (checked && found == atom_orbitals.end()) atom_orbitals.push_back(type);
        if (!checked && found != atom_orbitals.end()) atom_orbitals.erase(found);

        std::sort(atom_orbitals.begin(), atom_orbitals.end(), [](OrbitalType a, OrbitalType b) {
            const auto& types = orbital_types();
            return std::find(types.begin(), types.end(), a) < std::find(types.begin(), types.end(), b);
        });
    }
    this->update_assignment_summaries();
    this->update_checkbox_states();
    this->apply_preview();
}

void OrbitalDialog::clear_selected_atoms() {
    for (int row : this->selected_atom_rows()) this->assignments[row].clear();
    this->update_assignment_summaries();
    this->update_checkbox_states();
    this->apply_preview();
}

void OrbitalDialog::choose_colour(bool positive) {
    const QColor initial = positive ? this->positive_colour : this->negative_colour;
    const QColor colour = QColorDialog::getColor(
        initial, this, positive ? tr("Positive orbital phase") : tr("Negative orbital phase"));
    if (colour.isValid()) this->set_colour(positive, colour);
}

void OrbitalDialog::set_colour(bool positive, const QColor& colour) {
    if (positive) {
        this->positive_colour = colour;
        this->update_colour_button(this->positive_colour_button, colour);
    } else {
        this->negative_colour = colour;
        this->update_colour_button(this->negative_colour_button, colour);
    }
    this->apply_preview();
}

void OrbitalDialog::update_colour_button(QPushButton* button, const QColor& colour) {
    const QString text_colour = colour.lightnessF() < 0.52 ? "white" : "black";
    button->setStyleSheet(QString(
        "QPushButton { background: %1; color: %2; border: 1px solid #555; "
        "border-radius: 3px; padding: 4px 10px; }")
        .arg(colour.name(), text_colour));
}

void OrbitalDialog::apply_preview() {
    this->renderer->set_orbitals(this->assignments);
    this->renderer->set_orbital_colours(
        to_glm(this->positive_colour), to_glm(this->negative_colour));
}
