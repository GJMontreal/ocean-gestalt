#include "UniformState.hpp"
#include "UniformDispatcher.hpp"
#include "glError.hpp"
#include "Shader.hpp"
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
    pendingUpdatesFront[shaderName].push_back(std::move(update));
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

std::optional<ApiValue> UniformState::getUniform(const std::string& shader, const std::string& uniform) {
  auto it = uniformStates.find(shader + "." + uniform);
  if( it != uniformStates.end()){
    return( uniformToApi(it->second));
  }
  return std::nullopt;
}

void UniformState::renderThreadCallback() {
  {
    std::lock_guard lock(updateMutex);
    std::swap(pendingUpdatesFront, pendingUpdatesBack);
  }

  for (auto& [shaderName, updates] : pendingUpdatesBack) {
    ShaderProgram* shader = nullptr;
    auto shaders = context.getShaders();

    if(auto it = shaders.find(shaderName); it != shaders.end()){
      shader = it->second.get();
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
        uniformStates[u.shader + "." + u.uniform] = u.value;
        u.ack.set_value(true);
      }else{
        u.ack.set_value(false);
      }
    }
    shader->deactivate();
  }
  pendingUpdatesBack.clear();
}

std::unique_ptr<UniformMap> UniformState::dumpUniforms() {
  auto uniformMap = std::make_unique<UniformMap>();
  for(const auto &[key,value] : uniformStates){
    (*uniformMap)[key] = uniformToApi(value);
  }
  return std::move(uniformMap);
}