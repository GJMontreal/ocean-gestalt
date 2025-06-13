#include "UniformState.hpp"
#include "glError.hpp"

#include <iostream>

static const int TIMEOUT = 50;

std::optional<UniformValue> UniformState::setUniform(
    const std::string& shaderName,
    const std::string& uniformName,
    UniformValue value) {
  
  PendingUniformUpdate update{shaderName,uniformName,value};
  
  auto fut = update.ack.get_future();

  {
    std::lock_guard lock(updateMutex);
    pendingUpdates[shaderName].push_back(std::move(update));
  }
  
  if(fut.wait_for(std::chrono::milliseconds(TIMEOUT)) == std::future_status::ready){
    if(fut.get()){
      return value;
    }else{
      return std::nullopt;
    }
  } else {
    return std::nullopt;
  }
}

void UniformState::renderThreadCallback() {
  std::lock_guard lock(updateMutex);

  for (auto& [shaderName, updates] : pendingUpdates) {
    // clunky
    // can we replace the explicit if with a get shader by name?
    ShaderProgram* shader = nullptr;
    if (shaderName == "mesh") {
      shader = context.meshShader.get();
    } else if (shaderName == "wireframe") {
      shader = context.wireframeShader.get();
    }

    if(!shader){
      std::cerr << "No matching shader for: " << shaderName << std::endl;
    }

    shader->activate();
    for (auto& u : updates) {
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

          if(!glCheckError(__FILE__, __LINE__)){
            uniformStates[u.uniform] = u.value;
            u.ack.set_value(true);
          }else{
            u.ack.set_value(false);
          }
    }
    shader->deactivate();
  }
  pendingUpdates.clear();
}