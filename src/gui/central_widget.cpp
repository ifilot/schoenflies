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

    this->point_group_label = new QLabel();
    this->point_group_label->setTextFormat(Qt::TextFormat::RichText);
    this->point_group_label->setVisible(false);
    symmetry_layout->addWidget(this->point_group_label);

    QTreeView *tree_view = new QTreeView();
    this->model = new QStandardItemModel();
    SymmetryOperationItemDelegate *delegate = new SymmetryOperationItemDelegate();

    connect(this->model, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this, SLOT(trigger_animation(QModelIndex)));

    tree_view->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::MinimumExpanding);
    tree_view->setMinimumSize(220, 50);
    tree_view->setEditTriggers(QTreeView::EditTrigger::NoEditTriggers);
    tree_view->setMouseTracking(true);
    tree_view->setHeaderHidden(true);
    tree_view->setModel(this->model);
    tree_view->setItemDelegate(delegate);

    QItemSelectionModel *selection_model = tree_view->selectionModel();

    connect(selection_model, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(tree_view_selection_changed(const QItemSelection&)));

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
    this->symmetry = symmetry;

    this->structure_animating = false;
    this->operation_selected = false;

    this->gl_widget->set_structure(structure, glm::mat3x3(1.0f));
    this->gl_widget->set_structure_rotation(symmetry->get_cartesian_axes());
    this->gl_widget->reset_camera();

    this->send_operation_to_gl();

    QString point_group_name = QString::fromStdString(symmetry->get_point_group().get_label().get_name_html());
    this->point_group_label->setText(QString("Point group: %1").arg(point_group_name));
    this->point_group_label->setVisible(true);

    this->update_operations_model();
}

void CentralWidget::update_operations_model() {
    this->model->clear();
    QStandardItem *root = this->model->invisibleRootItem();

    const auto operation_manager = this->symmetry->get_operation_manager();
    const auto point_group_operations_order = operation_manager->get_point_group_operations_order();

    // add identity operation, which is always present but not tracked
    root->appendRow(new QStandardItem("<span><i>E</i> identity</span>"));

    // add all other operations
    for (auto operation_group : point_group_operations_order) {
        if (operation_group.size() == 1) {
            // add operation on top level
            int operation_id = operation_group[0];
            auto operation = operation_manager->get_point_group_operation(operation_id);

            QStandardItem *item = new QStandardItem(QString("<span>%1</span>").arg(
                QString::fromStdString(operation.get_label().get_name_html())));
            item->setData(operation_id, SymmetryOperationItemDelegate::ItemDataRole::ButtonRole);

            root->appendRow(item);
        } else if (operation_group.size() > 1) {
            // add operation group
            // get name of first operation for group title
            auto first_op = operation_manager->get_point_group_operation(operation_group[0]);
            QString title = QString("<span>%1s (%2)</span>").arg(
                QString::fromStdString(first_op.get_label().get_name_html()),
                QString::number(operation_group.size())
            );
            QStandardItem *item = new QStandardItem(title);

            for (auto operation_id : operation_group) {
                auto operation = operation_manager->get_point_group_operation(operation_id);

                QStandardItem *sub_item = new QStandardItem(QString("<span>%1</span>").arg(
                    QString::fromStdString(operation.get_label().get_name_html())));
                sub_item->setData(operation_id, SymmetryOperationItemDelegate::ItemDataRole::ButtonRole);

                item->appendRow(sub_item);
            }

            root->appendRow(item);
        }
    }
}

/**
 * @brief Send the operation to the GL widget
 */
void CentralWidget::send_operation_to_gl() {
    // only update if the structure is not animating
    if (!this->structure_animating) {
        if (this->operation_selected) {
            this->gl_widget->set_operation(this->selected_operation);
        } else {
            this->gl_widget->unset_operation();
        }
    }
}

/**
 * @brief Update the operation shown in the GL widget based on the
 * selection from the tree view
 *
 * @param selected information about selected items
 */
void CentralWidget::tree_view_selection_changed(const QItemSelection& selected) {
    QModelIndexList indices = selected.indexes();
    if (indices.size() < 1) {
        this->operation_selected = false;
        this->send_operation_to_gl();
        return;
    }

    QStandardItem *item = this->model->itemFromIndex(indices[0]);

    if (item->data(SymmetryOperationItemDelegate::ItemDataRole::ButtonRole).toBool()) {
        unsigned int operation_index = item->data(SymmetryOperationItemDelegate::ItemDataRole::ButtonRole).toInt();

        this->selected_operation = this->symmetry->get_operation_manager()->get_point_group_operation(operation_index);
        this->operation_selected = true;
    } else {
        this->operation_selected = false;
    }

    this->send_operation_to_gl();
}

/**
 * @brief Trigger the animation of a symmetry operation in the GL widget
 *
 * @param index index of symmetry operation in the data model
 */
void CentralWidget::trigger_animation(QModelIndex index) {
    QStandardItem *item = this->model->itemFromIndex(index);

    if (item->data(SymmetryOperationItemDelegate::ItemDataRole::ButtonClickedRole).toBool()) {
        unsigned int operation_index = item->data(SymmetryOperationItemDelegate::ItemDataRole::ButtonRole).toInt();

        this->animation_operation = this->symmetry->get_operation_manager()->get_point_group_operation(operation_index);
        this->animation_start_time = std::chrono::high_resolution_clock::now();
        this->structure_animating = true;

        item->setData(false, SymmetryOperationItemDelegate::ItemDataRole::ButtonClickedRole);

        this->process_animations();
    }
}

/**
 * @brief Process any running animations
 */
void CentralWidget::process_animations() {
    if (!this->structure_animating) return;

    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> runtime = now - this->animation_start_time;
    double seconds = runtime.count();
    double f = seconds / 1.0;  // TODO make animation duration variable

    glm::mat3x3 animation_matrix;

    if (f > 1) {
        f = 1;
        this->structure_animating = false;
        animation_matrix = glm::mat3x3(1.0f);  // reset to identity matrix
        this->send_operation_to_gl();  // update operation if it has changed
    } else {
        animation_matrix = this->animation_operation.calculate_fractional_matrix(f);
    }

    this->gl_widget->set_structure(this->structure, animation_matrix);
}
