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

#include "gl_widget.h"

/**
 * @brief Construct a new GLWidget object
 *
 * @param parent pointer to parent widget
 */
GLWidget::GLWidget(QWidget* parent): QOpenGLWidget(parent) {
    this->bg = parent->palette().color(QPalette::ColorRole::Background);
}

/**
 * @brief Initialise OpenGL environment
 */
void GLWidget::initializeGL() {
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
    initializeOpenGLFunctions();

    glClearColor(this->bg.redF(), this->bg.greenF(), this->bg.blueF(), 1.0f);
}

/**
 * @brief Render scene
 */
void GLWidget::paintGL() {

}

/**
 * @brief Resize widget
 *
 * @param width widget width
 * @param height widget height
 */
void GLWidget::resizeGL(int width, int height) {

}

/**
 * @brief Cleanup OpenGL environment
 */
void GLWidget::cleanup() {

}
