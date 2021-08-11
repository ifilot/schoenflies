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
#include <QColor>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPalette>
#include <QPoint>
#include <QVector3D>
#include <QWidget>
#include "models/geometry.h"
#include "models/model.h"
#include "shaders/shader_program_manager.h"
#include "shaders/shader_program_type.h"

class GLWidget: public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

private:
    QWidget* parent;
    QColor bg;

    std::unique_ptr<ShaderProgramManager> shader_program_manager;

    std::vector<std::unique_ptr<Model>> models;

    QMatrix4x4 projection;
    QMatrix4x4 view;
    QMatrix4x4 rotation_matrix;
    QMatrix4x4 model;
    QMatrix4x4 mvp;
    QVector3D camera_position;
    QVector3D camera_translation;

    QMatrix4x4 arcball_rotation;  // temporary rotation while dragging mouse
    bool arcball_rotating = false;  // whether arcball rotation is active
    QPoint mouse_position;  // at start of arcball rotation

public:
    /**
     * @brief Construct a new GLWidget object
     *
     * @param parent pointer to parent widget
     */
    GLWidget(QWidget* parent);

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

private:
    /**
     * @brief Paint all instances of models to the screen
     */
    void paint_models();

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

public slots:
    /**
     * @brief Cleanup OpenGL environment
     */
    void cleanup();
};

#endif  // GUI_GL_WIDGET_H
