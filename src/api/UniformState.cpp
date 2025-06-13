#include "UniformState.hpp"

std::optional<UniformValue> UniformState::setUniform(
    const std::string& shaderName,
    const std::string& uniformName,
    UniformValue value) {
  std::lock_guard lock(updateMutex);
  PendingUniformUpdate update;
  update.uniform = uniformName;
  update.shader = shaderName;
  update.value = value;
  pendingUpdates[shaderName].push_back(std::move(update));
  // TODO: we need to use full paths, until then just echo the value back
  return value;
}

void UniformState::renderThreadCallback() {
  std::lock_guard lock(updateMutex);

  for (auto& [shaderName, updates] : pendingUpdates) {
    // clunky
    // can we replace the explicit if with a get shader by name?
    ShaderProgram* shader;
    if (shaderName == "mesh") {
      shader = context.meshShader.get();
    } else if (shaderName == "wireframe") {
      shader = context.wireframeShader.get();
    }
    shader->activate();
    for (const auto& u : updates) {
      std::visit(
          [&](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::vector<float>>) {
              if (val.size() == 4) {
                auto glmVec = glm::vec4(val[0], val[1], val[2], val[3]);
                shader->setUniform(u.uniform, glmVec);
              }
            }
          },
          u.value);
      uniformStates[u.uniform] = u.value;
    }
    shader->deactivate();
  }
  pendingUpdates.clear();
}