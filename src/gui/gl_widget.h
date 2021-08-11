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
#include <QColor>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPalette>
#include <QWidget>

class GLWidget: public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

private:
    QWidget* parent;
    QColor bg;

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

public slots:
    /**
     * @brief Cleanup OpenGL environment
     */
    void cleanup();
};

#endif  // GUI_GL_WIDGET_H
