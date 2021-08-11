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
    this->shader_program_manager = std::make_unique<ShaderProgramManager>();

    this->camera_position = QVector3D(0.0, -10.0f, 0.0);

    this->models.push_back(Geometry::sphere());
    this->models.push_back(Geometry::cylinder());

    // TODO load actual model here
    this->models[0]->add_instance(glm::vec3(1.0), glm::mat4(1.0), glm::vec3(0.0), glm::vec4(0.0, 1.0, 0.0, 1.0));
}

/**
 * @brief Initialise OpenGL environment
 */
void GLWidget::initializeGL() {
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
    initializeOpenGLFunctions();

    for (unsigned int i = 0; i < this->models.size(); ++i) {
        this->models[i]->load_to_vao();
    }

    glClearColor(this->bg.redF(), this->bg.greenF(), this->bg.blueF(), 1.0f);

    this->load_shaders();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Render scene
 */
void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // bind program
    this->shader_program_manager->bind("model_shader");

    // set camera
    QVector3D look_at = QVector3D(0.0f, 0.0f, 0.0f);
    this->view.setToIdentity();
    this->view.lookAt(this->camera_position, look_at, QVector3D(0.0f, 0.0f, 1.0f));

    // draw models
    this->paint_models();

    // release program
    this->shader_program_manager->release("model_shader");
}

/**
 * @brief Resize widget
 *
 * @param width widget width
 * @param height widget height
 */
void GLWidget::resizeGL(int width, int height) {
    this->projection.setToIdentity();
    this->projection.perspective(45.0f, GLfloat(width) / height, 0.01f, 1000.0f);
}

/**
 * @brief Paint all instances of models to the screen
 */
void GLWidget::paint_models() {
    ShaderProgram *model_shader = this->shader_program_manager->get_shader_program("model_shader");

    for (unsigned int i = 0; i < this->models.size(); ++i) {
        Model *model = this->models[i].get();

        for (const auto& instance : model->get_instances()) {
            // build model matrix (scale -> rotation -> translation)
            this->model.setToIdentity();
            this->model.translate(-this->camera_translation);
            this->model *= this->rotation_matrix;
            this->model.translate(instance.translation.x, instance.translation.y, instance.translation.z);
            this->model *= QMatrix4x4(glm::value_ptr(instance.rotation)).transposed();
            this->model.scale(instance.scale.x, instance.scale.y, instance.scale.z);

            this->mvp = this->projection * this->view * this->model;

            model_shader->set_uniform("mvp", this->mvp);
            model_shader->set_uniform("view", this->view);
            model_shader->set_uniform("model", this->model);
            model_shader->set_uniform("color", QVector4D(instance.colour.x, instance.colour.y, instance.colour.z, instance.colour.a));
            model_shader->set_uniform("lightpos", QVector3D(0.0f, -1000.0f, 1.0f));

            model->draw();
        }
    }
}


/**
 * @brief Load OpenGL shaders
 */
void GLWidget::load_shaders() {
    this->shader_program_manager->create_shader_program("model_shader", ShaderProgramType::ModelShader, ":/assets/shaders/phong.vs", ":/assets/shaders/phong.fs");
}

/**
 * @brief Cleanup OpenGL environment
 */
void GLWidget::cleanup() {

}
