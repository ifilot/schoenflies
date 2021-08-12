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

    this->arrow_model = ObjLoader::load_from_obj(":/assets/models/arrow.obj");
}

/**
 * @brief Set the structure displayed in the widget
 *
 * @param structure
 */
void GLWidget::set_structure(std::shared_ptr<Structure> structure) {
    this->remove_model_instances();

    for (unsigned int i = 0; i < structure->get_num_atoms(); ++i) {
        Element el = PeriodicTable::get_element(structure->get_atomic_number(i));
        this->models[0]->add_instance(glm::vec3(el.radius), glm::mat4(1.0), structure->get_coordinates(i), glm::vec4(el.colour, 1.0f));
    }

    auto pairs = structure->calculate_bond_pairs();
    for (auto pair : pairs) {
        // generate two cylinders for each bond
        Element el_a = PeriodicTable::get_element(structure->get_atomic_number(pair.first));
        Element el_b = PeriodicTable::get_element(structure->get_atomic_number(pair.second));

        glm::vec3 v = structure->get_coordinates(pair.second) - structure->get_coordinates(pair.first);
        glm::vec3 vn = glm::normalize(v);
        float vl = glm::length(v);

        float scale_factor = .5 + (el_a.radius - el_b.radius) / vl / 2;

        glm::vec3 trans_a = structure->get_coordinates(pair.first);
        glm::vec3 trans_b = trans_a + scale_factor * v;

        glm::vec3 scale_a = {0.05f, 0.05f, scale_factor * vl};
        glm::vec3 scale_b = {0.05f, 0.05f, (1 - scale_factor) * vl};

        glm::mat4 rotation(1.0f);
        if (qFabs(vn.z) > .9999f) {
            if (vn.z < -.5f) {
                rotation = glm::rotate(glm::mat4(1.0f), -(float) M_PI, glm::vec3(0.0f, 1.0f, 0.0f));
            }
        } else {
            float angle = qAcos(vn.z);
            glm::vec3 axis_angle = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), v));
            rotation = glm::rotate(glm::mat4(1.0), angle, axis_angle);
        }

        this->models[1]->add_instance(scale_a, rotation, trans_a, glm::vec4(el_a.colour, 1.0f));
        this->models[1]->add_instance(scale_b, rotation, trans_b, glm::vec4(el_b.colour, 1.0f));
    }

    this->update();
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

    this->arrow_model->load_to_vao();

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

    // set camera
    QVector3D look_at = QVector3D(0.0f, 0.0f, 0.0f);
    this->view.setToIdentity();
    this->view.lookAt(this->camera_position, look_at, QVector3D(0.0f, 0.0f, 1.0f));

    // draw models
    this->paint_models();

    // draw axes
    this->paint_gizmos();
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
 * @brief Handle mouse press event
 *
 * @param event
 */
void GLWidget::mousePressEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::MouseButton::LeftButton) {
        this->arcball_rotating = true;
        this->mouse_position = event->pos();
    }
}

/**
 * @brief Handle mouse release event
 *
 * @param event
 */
void GLWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (this->arcball_rotating && !(event->buttons() & Qt::MouseButton::LeftButton)) {
        // apply arcball rotation to rotation matrix, and reset arcball rotation
        this->rotation_matrix = this->arcball_rotation * this->rotation_matrix;
        this->arcball_rotation.setToIdentity();
        this->arcball_rotating = false;
    }
}

/**
 * @brief Handle mouse move event
 *
 * @param event
 */
void GLWidget::mouseMoveEvent(QMouseEvent* event) {
    if (this->arcball_rotating) {
        // implementation from
        // https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
        if (event->x() != this->mouse_position.x() || event->y() != this->mouse_position.y()) {
            // calculate arcball vectors
            QVector3D va = this->calc_arcball_vector(this->mouse_position);
            QVector3D vb = this->calc_arcball_vector(event->pos());

            // calculate angle between vectors
            float dotprod = QVector3D::dotProduct(va, vb);
            if (qFabs(dotprod) > .9999f) return;
            float angle = qAcos(qMin(1.0f, dotprod));

            // rotation vector in camera space
            QVector4D axis_cam_space = QVector4D(QVector3D::crossProduct(va, vb).normalized());

            // matrix to change basis from camera to model space
            QMatrix3x3 cam_to_model_trans = this->view.inverted().toGenericMatrix<3, 3>();

            // rotation vector in model space
            QVector4D axis_model_space = QMatrix4x4(cam_to_model_trans) * axis_cam_space;

            // set rotation
            this->set_arcball_rotation(qRadiansToDegrees(angle), axis_model_space);
        }
    }
}

/**
 * @brief Handle mouse wheel event
 *
 * @param event
 */
void GLWidget::wheelEvent(QWheelEvent* event) {
    this->camera_position += event->delta() * 0.01f * QVector3D(0, 1, 0);

    // prevent zooming in too far
    if (this->camera_position[1] > -5.0) this->camera_position[1] = -5.0;

    this->update();
}

/**
 * @brief Paint all instances of models to the screen
 */
void GLWidget::paint_models() {
    ShaderProgram *model_shader = this->shader_program_manager->get_shader_program("model_shader");
    model_shader->bind();

    for (unsigned int i = 0; i < this->models.size(); ++i) {
        Model *model = this->models[i].get();

        for (const auto& instance : model->get_instances()) {
            // build model matrix (scale -> rotation -> translation)
            this->model.setToIdentity();
            this->model.translate(-this->camera_translation);
            this->model *= this->arcball_rotation * this->rotation_matrix;
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

    model_shader->release();
}

/**
 * @brief Paint axis gizmos
 */
void GLWidget::paint_gizmos() {
    ShaderProgram *axes_shader = this->shader_program_manager->get_shader_program("axes_shader");
    axes_shader->bind();

    const QVector3D r(1.0f      , 0.2117647f, 0.3254902f);
    const QVector3D g(0.5411765f, 0.854902f , 0.0235294f);
    const QVector3D b(0.172549f , 0.5607843f, 1.0f      );

    // set viewport, projection, and view matrices
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glViewport(0.75f * this->geometry().width(), 0.0f, 0.25f * this->geometry().width(), 0.25f * this->geometry().height());

    QMatrix4x4 projection_ortho;
    projection_ortho.setToIdentity();
    float ratio = (float) this->geometry().height() / (float) this->geometry().width();
    static const float size = 25.0f;
    projection_ortho.ortho(-size, size, -size * ratio, size * ratio, 0.1f, 1000.0f);

    this->view.setToIdentity();
    this->view.lookAt(QVector3D(0.0f, -10.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f));
    axes_shader->set_uniform("view", this->view);

    // draw the three axes
    QMatrix4x4 axis_rotation;

    // z axis
    axis_rotation.setToIdentity();
    this->model = this->arcball_rotation * this->rotation_matrix * axis_rotation;
    this->mvp = projection_ortho * this->view * this->model;
    axes_shader->set_uniform("model", this->model);
    axes_shader->set_uniform("mvp", this->mvp);
    axes_shader->set_uniform("color", b);
    this->arrow_model->draw();

    // y axis
    axis_rotation.setToIdentity();
    axis_rotation.rotate(-90.0f, QVector3D(1.0f, 0.0f, 0.0f));
    this->model = this->arcball_rotation * this->rotation_matrix * axis_rotation;
    this->mvp = projection_ortho * this->view * this->model;
    axes_shader->set_uniform("model", this->model);
    axes_shader->set_uniform("mvp", this->mvp);
    axes_shader->set_uniform("color", g);
    this->arrow_model->draw();

    // x axis
    axis_rotation.setToIdentity();
    axis_rotation.rotate(90.0f, QVector3D(0.0f, 1.0f, 0.0f));
    this->model = this->arcball_rotation * this->rotation_matrix * axis_rotation;
    this->mvp = projection_ortho * this->view * this->model;
    axes_shader->set_uniform("model", this->model);
    axes_shader->set_uniform("mvp", this->mvp);
    axes_shader->set_uniform("color", r);
    this->arrow_model->draw();

    axes_shader->release();
}

/**
 * @brief Load OpenGL shaders
 */
void GLWidget::load_shaders() {
    this->shader_program_manager->create_shader_program("model_shader", ShaderProgramType::ModelShader, ":/assets/shaders/phong.vs", ":/assets/shaders/phong.fs");
    this->shader_program_manager->create_shader_program("axes_shader", ShaderProgramType::AxesShader, ":/assets/shaders/axes.vs", ":/assets/shaders/axes.fs");
}

/**
 * @brief Calculate the arcball vector
 *
 * @param pos position of the mouse cursor
 * @return QVector3D arcball vector
 */
QVector3D GLWidget::calc_arcball_vector(QPoint pos) {
    QVector3D P = QVector3D(1.0f * (float) pos.x() / (float) this->geometry().width() * 2.0f - 1.0f,
                            -(1.0f * (float) pos.y() / (float) this->geometry().height() * 2.0f - 1.0f),
                            0.0f);

    float OP_squared = P[0] * P[0] + P[1] * P[1];

    if (OP_squared <= 1.0f) {
        P[2] = qSqrt(1.0f - OP_squared);
    } else {
        P = P.normalized();
    }
    return P;
}

/**
 * @brief Set arcball vector rotation and update
 *
 * @param angle arcball angle
 * @param vector arcball rotation vector
 */
void GLWidget::set_arcball_rotation(float angle, const QVector4D& vector) {
    this->arcball_rotation.setToIdentity();
    this->arcball_rotation.rotate(angle, QVector3D(vector));
    this->update();
}

/**
 * @brief Remove all instances of models
 */
void GLWidget::remove_model_instances() {
    for (unsigned int i = 0; i < this->models.size(); ++i) {
        this->models[i]->remove_instances();
    }
}

/**
 * @brief Cleanup OpenGL environment
 */
void GLWidget::cleanup() {

}
