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

#ifndef GUI_MODELS_MODEL_H
#define GUI_MODELS_MODEL_H

#include <vector>
#include <glm/glm.hpp>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include "model_instance.h"

class Model {
protected:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

private:
    bool loaded_vao = false;

    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo[4];

    std::vector<ModelInstance> instances;

public:
    /**
     * @brief Default constructor
     */
    Model();

    /**
     * @brief Construct a new Model object
     *
     * @param positions positions of vertices
     * @param normals normals of vertices
     * @param indices triples of vertices forming triangles
     */
    Model(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> indices);

    /**
     * @brief
     *
     * @return Model
     */
    static Model cylinder();

    /**
     * @brief Destroy the Model object
     */
    ~Model();

    /**
     * @brief Load the model to the vertex array object
     */
    void load_to_vao();

    /**
     * @brief Add an instance of the model to the list of instances
     *
     * @param scale scale of instance
     * @param rotation rotation of instance
     * @param translation translation of instance
     * @param colour colour of instance
     */
    void add_instance(const glm::vec3& scale, const glm::mat4& rotation, const glm::vec3& translation, const glm::vec4& colour);

    /**
     * @brief Remove all instances of the model
     */
    void remove_instances();

    /**
     * @brief Get all model instances
     *
     * @return const std::vector<ModelInstance>&
     */
    const std::vector<ModelInstance>& get_instances() const;

    /**
     * @brief Draw this model
     */
    void draw();
};

#endif  // GUI_MODELS_MODEL_H
