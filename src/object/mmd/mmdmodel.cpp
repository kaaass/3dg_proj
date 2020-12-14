#include "mmdmodel.h"

#include <lighting.h>
#include <game.h>
#include <texture.h>

MmdModel::MmdModel(const std::string &modelPath, const std::string &animPath) {
    // 加载模型
    auto model = std::make_unique<saba::PMXModel>();
    model->Load(modelPath, "./");
    m_mmdModel = std::move(model);
    m_mmdModel->InitializeAnimation();
    // 加载动画
    auto anim = std::make_unique<saba::VMDAnimation>();
    anim->Create(m_mmdModel);
    saba::VMDFile vmdFile;
    saba::ReadVMDFile(&vmdFile, animPath.c_str());
    anim->Add(vmdFile);
    anim->SyncPhysics(0.0f);
    m_vmdAnim = std::move(anim);
}

void MmdModel::init() {
    // 创建 VBO、EBO
    size_t vtxCount = m_mmdModel->GetVertexCount();
    glGenBuffers(1, &m_posVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vtxCount, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_norVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_norVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vtxCount, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_uvVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vtxCount, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    size_t idxSize = m_mmdModel->GetIndexElementSize();
    size_t idxCount = m_mmdModel->GetIndexCount();
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxSize * idxCount, m_mmdModel->GetIndices(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    if (idxSize == 1) {
        m_indexType = GL_UNSIGNED_BYTE;
    } else if (idxSize == 2) {
        m_indexType = GL_UNSIGNED_SHORT;
    } else if (idxSize == 4) {
        m_indexType = GL_UNSIGNED_INT;
    }

    // 创建 VAO
    glGenVertexArrays(1, &m_mmdVAO);
    glBindVertexArray(m_mmdVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const void *) 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_norVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const void *) 0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (const void *) 0);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    glBindVertexArray(0);

    // 加载材质
    for (size_t i = 0; i < m_mmdModel->GetMaterialCount(); i++) {
        const auto mmdMat = &m_mmdModel->GetMaterials()[i];
        Material mat(mmdMat);
        if (!mmdMat->m_texture.empty()) {
            mat.m_texture = Texture::loadTexture(mmdMat->m_texture);
        }
        // 忽略反射、Toon材质
        m_materials.emplace_back(mat);
    }

     idle(0);
}

void MmdModel::draw(Shader *shader) {
    Lighting::getDefault()->useShader(shader);

    // 视图、投影矩阵
    glm::mat4 projection;
    auto &screen = Game::getInstance()->screen;
    auto camera = Game::curStage()->getCamera();
    projection = glm::perspective(glm::radians(camera->zoom), (float) screen.width / (float) screen.height, 0.1f,
                                  100.0f);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3{0, 0, 3});
    modelMat = glm::scale(modelMat, glm::vec3(1.0 / 6));

    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat4("model", modelMat);

    // 绘制模型
    size_t subMeshCount = m_mmdModel->GetSubMeshCount();
    for (size_t i = 0; i < subMeshCount; i++) {
        const auto &subMesh = m_mmdModel->GetSubMeshes()[i];
        const auto &mat = m_materials[subMesh.m_materialID];
        auto mmdMat = mat.m_mmdMat;

        if (mat.m_mmdMat->m_alpha == 0) {
            continue;
        }

        glBindVertexArray(m_mmdVAO);

        shader->setVec3("dirLight.ambient", mmdMat->m_ambient);
        shader->setVec3("dirLight.diffuse", mmdMat->m_diffuse);
        shader->setVec3("dirLight.specular", mmdMat->m_specular);
        shader->setFloat("material.shininess", mmdMat->m_specularPower);

        // 绑定材质
        glActiveTexture(GL_TEXTURE0 + 0);
        shader->setInt("material.texture_diffuse1", 0);
        glBindTexture(GL_TEXTURE_2D, mat.m_texture);

        // 忽略Sp、Toon材质

        size_t offset = subMesh.m_beginIndex * m_mmdModel->GetIndexElementSize();
        glDrawElements(GL_TRIANGLES, subMesh.m_vertexCount, m_indexType, (GLvoid *) offset);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void MmdModel::idle(float delta) {
    auto &animTime = Game::getInstance()->control.animTime;
    // 更新动画
    if (delta > 1.0 / 30.0) {
        delta = 1.0 / 30.0;
    }
    if (Game::getInstance()->control.anim)
        animTime += delta;
    m_mmdModel->BeginAnimation();
    m_mmdModel->UpdateAllAnimation(m_vmdAnim.get(), animTime * 30.0f, delta);
    m_mmdModel->EndAnimation();
    // 更新顶点数据
    m_mmdModel->Update();
    size_t vtxCount = m_mmdModel->GetVertexCount();
    glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vtxCount, m_mmdModel->GetUpdatePositions());
    glBindBuffer(GL_ARRAY_BUFFER, m_norVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vtxCount, m_mmdModel->GetUpdateNormals());
    glBindBuffer(GL_ARRAY_BUFFER, m_uvVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * vtxCount, m_mmdModel->GetUpdateUVs());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MmdModel::resetAnim() {
    Game::getInstance()->control.animTime = 0;
    m_vmdAnim->SyncPhysics(0);
}
