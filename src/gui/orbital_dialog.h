/**
 * Schoenflies
 * Copyright (c) 2026 Ivo Filot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef GUI_ORBITAL_DIALOG_H
#define GUI_ORBITAL_DIALOG_H

#include <memory>
#include <unordered_map>
#include <vector>
#include <QCheckBox>
#include <QColor>
#include <QDialog>
#include <QGroupBox>
#include <QPushButton>
#include <QTableWidget>
#include "../orbitals/orbital_type.h"
#include "../structure.h"
#include "../structure_renderer.h"

class OrbitalDialog final: public QDialog {
private:
    std::shared_ptr<Structure> structure;
    std::shared_ptr<StructureRenderer> renderer;

    OrbitalAssignments original_assignments;
    OrbitalAssignments assignments;
    glm::vec3 original_positive_colour;
    glm::vec3 original_negative_colour;
    QColor positive_colour;
    QColor negative_colour;

    QTableWidget* atom_table;
    std::unordered_map<OrbitalType, QCheckBox*> orbital_checkboxes;
    QPushButton* positive_colour_button;
    QPushButton* negative_colour_button;
    bool updating_controls = false;

public:
    OrbitalDialog(
        const std::shared_ptr<Structure>& structure,
        const std::shared_ptr<StructureRenderer>& renderer,
        QWidget* parent = nullptr);

    /** Restore the state from before the dialog opened. */
    void reject() override;

private:
    QGroupBox* create_orbital_group(
        const QString& title,
        const std::vector<OrbitalType>& types,
        QWidget* parent);
    QWidget* create_colour_row(bool positive, QWidget* parent);
    std::vector<int> selected_atom_rows() const;
    void update_checkbox_states();
    void update_assignment_summaries();
    void set_orbital_checked(OrbitalType type, bool checked);
    void clear_selected_atoms();
    void choose_colour(bool positive);
    void set_colour(bool positive, const QColor& colour);
    void update_colour_button(QPushButton* button, const QColor& colour);
    void apply_preview();
};

#endif  // GUI_ORBITAL_DIALOG_H
