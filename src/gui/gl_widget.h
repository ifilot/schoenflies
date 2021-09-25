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

#ifndef GUI_GL_WIDGET_H
#define GUI_GL_WIDGET_H

#include <memory>
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>
#include <Qt>
#include <QtMath>
#include <QAction>
#include <QColor>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPalette>
#include <QPoint>
#include <QVector3D>
#include <QWheelEvent>
#include <QWidget>
#include <QWindow>
#include "../structure.h"
#include "../periodic_table/element.h"
#include "../periodic_table/periodic_table.h"
#include "../symmetry/operations/operation.h"
#include "../symmetry/operations/operation_label.h"
#include "models/geometry.h"
#include "models/model.h"
#include "models/obj_loader.h"
#include "shaders/shader_program_manager.h"
#include "shaders/shader_program_type.h"
#include "stereoscopic_method.h"

class GLWidget: public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

private:
    QWidget* parent;
    QColor bg;

    std::unique_ptr<ShaderProgramManager> shader_program_manager;

    std::vector<std::unique_ptr<Model>> structure_models;
    std::vector<std::unique_ptr<Model>> operation_models;
    std::unique_ptr<Model> arrow_model;

    float structure_span = 0;

    QMatrix4x4 projection;
    QMatrix4x4 view;
    QMatrix4x4 rotation_matrix;
    QMatrix4x4 model;
    QMatrix4x4 mvp;
    QVector3D camera_position;
    QVector3D camera_translation;

    QMatrix4x4 structure_rotation;

    QMatrix4x4 arcball_rotation;  // temporary rotation while dragging mouse
    bool arcball_rotating = false;  // whether arcball rotation is active
    QPoint mouse_position;  // at start of arcball rotation

    StereoscopicMethod stereoscopic_method = StereoscopicMethod::None;
    std::string stereoscopic_method_name;

public:
    /**
     * @brief Construct a new GLWidget object
     *
     * @param parent pointer to parent widget
     */
    GLWidget(QWidget* parent);

    /**
     * @brief Set the structure displayed in the widget
     *
     * @param structure
     * @param animation_matrix
     */
    void set_structure(std::shared_ptr<Structure> structure, glm::mat3x3 animation_matrix);

    /**
     * @brief Set the rotation of the structure in the GL widget to correctly
     * rotate to Cartesian axes
     *
     * @param cartesian_axes matrix of unit vectors along which the Cartesian
     * axes should lie
     */
    void set_structure_rotation(glm::mat3x3 cartesian_axes);

    /**
     * @brief Set the operation displayed in the widget
     *
     * @param operation
     */
    void set_operation(Operation operation);

    /**
     * @brief Unset the operation displayed in the widget
     */
    void unset_operation();

    /**
     * @brief Reset variables related to the camera
     */
    void reset_camera();

protected:
    /**
     * @brief Initialise OpenGL environment
     */
    void initializeGL() Q_DECL_OVERRIDE;

    /**
     * @brief Render scene
     */
    void paintGL() Q_DECL_OVERRIDE;

    /**
     * @brief Resize widget
     *
     * @param width widget width
     * @param height widget height
     */
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;

    /**
     * @brief Handle mouse press event
     *
     * @param event
     */
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief Handle mouse release event
     *
     * @param event
     */
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief Handle mouse move event
     *
     * @param event
     */
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief Handle mouse wheel event
     *
     * @param event
     */
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;

private:
    /**
     * @brief Paint all instances of structure models to the screen
     */
    void paint_structure_models();

    /**
     * @brief Paint all instances of operation models to the screen
     */
    void paint_operation_models();

    /**
     * @brief Paint axis gizmos
     */
    void paint_gizmos();

    /**
     * @brief Compute the rotation matrix to rotate an object aligned along the
     * z axis towards the given axis
     *
     * @param axis
     * @return glm::mat4x4 rotation matrix
     */
    glm::mat4x4 rotation_matrix_from_axis_vector(glm::vec3 axis);

    /**
     * @brief Load OpenGL shaders
     */
    void load_shaders();

    /**
     * @brief Calculate the arcball vector
     *
     * @param pos position of the mouse cursor
     * @return QVector3D arcball vector
     */
    QVector3D calc_arcball_vector(QPoint pos);

    /**
     * @brief Set arcball vector rotation and update
     *
     * @param angle arcball angle
     * @param vector arcball rotation vector
     */
    void set_arcball_rotation(float angle, const QVector4D& vector);

    /**
     * @brief Remove all instances of models
     */
    void remove_structure_model_instances();

public slots:
    /**
     * @brief Cleanup OpenGL environment
     */
    void cleanup();

    /**
     * @brief Set the stereoscopic method and update
     *
     * @param action action corresponding to stereoscopic method
     */
    void set_stereoscopic_method(QAction* action);
};

#endif  // GUI_GL_WIDGET_H
