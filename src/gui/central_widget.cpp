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

    this->gl_widget = new GLWidget(this);
    this->gl_widget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    layout->addWidget(this->gl_widget, 3);

    QVBoxLayout *symmetry_layout = new QVBoxLayout;

    layout->addLayout(symmetry_layout, 1);

    // TODO text edit is temporary
    this->text_edit = new QTextEdit();
    this->text_edit->setReadOnly(true);
    this->text_edit->setPlainText("Hello, World!");
    symmetry_layout->addWidget(this->text_edit);

    QTreeView *tree_view = new QTreeView();
    this->model = new QStandardItemModel();
    SymmetryOperationItemDelegate *delegate = new SymmetryOperationItemDelegate();

    connect(this->model, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this, SLOT(trigger_animation(QModelIndex)));

    tree_view->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::MinimumExpanding);
    tree_view->setMinimumSize(320, 50);
    tree_view->setEditTriggers(QTreeView::EditTrigger::NoEditTriggers);
    tree_view->setSelectionMode(QTreeView::SelectionMode::NoSelection);
    tree_view->setMouseTracking(true);
    tree_view->setHeaderHidden(true);
    tree_view->setModel(this->model);
    tree_view->setItemDelegate(delegate);

    // TODO temporary items to test layout
    QStandardItem *root = this->model->invisibleRootItem();
    QStandardItem *item, *sub_item;

    item = new QStandardItem("<i>E</i> identity");
    root->appendRow({item});

    item = new QStandardItem("<i>C</i><sub>4</sub> rotations (2)");
    sub_item = new QStandardItem("<i>C</i><sub>4</sub> rotation");
    sub_item->setData("button", SymmetryOperationItemDelegate::ItemDataRole::ButtonRole);
    item->appendRow({sub_item});
    sub_item = new QStandardItem("<i>C</i><sub>4</sub><sup>&minus;1</sup> rotation");
    sub_item->setData("button", SymmetryOperationItemDelegate::ItemDataRole::ButtonRole);
    item->appendRow({sub_item});
    root->appendRow({item});

    item = new QStandardItem("<i>C</i><sub>2</sub> rotation");
    item->setData("button", SymmetryOperationItemDelegate::ItemDataRole::ButtonRole);
    root->appendRow({item});

    symmetry_layout->addWidget(tree_view);

    this->setLayout(layout);
}

/**
 * @brief Set the structure object
 *
 * @param structure
 */
void CentralWidget::set_structure(std::shared_ptr<Structure> structure) {
    this->structure = structure;
    auto symmetry = std::make_shared<Symmetry>(structure);

    this->gl_widget->set_structure(structure);
    this->gl_widget->set_structure_rotation(symmetry->get_cartesian_axes());

    this->text_edit->setPlainText(QString::fromStdString(this->structure->get_description()));

    // temporarily show found symmetry operations in the text edit
    auto operation_manager = symmetry->get_operation_manager();

    for (unsigned int i = 0; i < operation_manager->get_inversions().size(); ++i) {
        auto inversion = operation_manager->get_inversions()[i];
        this->text_edit->append(QString("%1").arg(QString::fromStdString(inversion.get_label().get_name_html())));
    }
    for (unsigned int i = 0; i < operation_manager->get_proper_rotations().size(); ++i) {
        auto rotation = operation_manager->get_proper_rotations()[i];
        auto a = rotation.get_axis();
        this->text_edit->append(QString("%1 (%2, %3, %4)").arg(QString::fromStdString(rotation.get_label().get_name_html()), QString::number(a.x), QString::number(a.y), QString::number(a.z)));
    }
    for (unsigned int i = 0; i < operation_manager->get_improper_rotations().size(); ++i) {
        auto rotation = operation_manager->get_improper_rotations()[i];
        auto a = rotation.get_axis();
        this->text_edit->append(QString("%1 (%2, %3, %4)").arg(QString::fromStdString(rotation.get_label().get_name_html()), QString::number(a.x), QString::number(a.y), QString::number(a.z)));
    }
    for (unsigned int i = 0; i < operation_manager->get_reflections().size(); ++i) {
        auto reflection = operation_manager->get_reflections()[i];
        auto n = reflection.get_axis();
        this->text_edit->append(QString("%1 (%2, %3, %4)").arg(QString::fromStdString(reflection.get_label().get_name_html()), QString::number(n.x), QString::number(n.y), QString::number(n.z)));
    }

    auto point_group = symmetry->get_point_group();
    this->text_edit->append(QString("Point group: %1").arg(QString::fromStdString(point_group.get_label().get_name_html())));

    auto x = symmetry->get_x_axis();
    this->text_edit->append(QString("x axis (%1, %2, %3)").arg(QString::number(x.x), QString::number(x.y), QString::number(x.z)));

    auto y = symmetry->get_y_axis();
    this->text_edit->append(QString("y axis (%1, %2, %3)").arg(QString::number(y.x), QString::number(y.y), QString::number(y.z)));

    auto z = symmetry->get_z_axis();
    this->text_edit->append(QString("z axis (%1, %2, %3)").arg(QString::number(z.x), QString::number(z.y), QString::number(z.z)));
}

/**
 * @brief Trigger the animation of a symmetry operation in the GL widget
 *
 * @param index index of symmetry operation in the data model
 */
void CentralWidget::trigger_animation(QModelIndex index) {
    QStandardItem *item = this->model->itemFromIndex(index);

    if (item->data(SymmetryOperationItemDelegate::ItemDataRole::ButtonClickedRole).toBool()) {
        QString str = item->data(0).toString();

        // TODO perform animation in GL widget
        this->text_edit->setText("Triggered animation playback for " + str);

        item->setData(false, SymmetryOperationItemDelegate::ItemDataRole::ButtonClickedRole);
    }
}
