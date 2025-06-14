#include "UniformState.hpp"
#include "UniformDispatcher.hpp"
#include "glError.hpp"

#include <iostream>

static const int TIMEOUT = 50;


std::optional<ApiValue> UniformState::setUniform(
    const std::string& shaderName,
    const std::string& uniformName,
    ApiValue value) {
  
  UniformValue uniformValue = apiToUniform(value);
  PendingUniformUpdate update{shaderName,uniformName, uniformValue};
  
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
      return;
    }

    shader->activate();
    for (auto& u : updates) {
      UniformDispatcher<ShaderProgram> dispatcher{shader, u.uniform};
      std::visit(dispatcher, u.value);

      if(dispatcher.success && !glCheckError(__FILE__, __LINE__)){
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
