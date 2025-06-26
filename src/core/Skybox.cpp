GLuint loadCubemap(const std::vector<std::string>& faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (GLuint i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load cubemap face: " << faces[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

std::vector<std::string> faces = {
    "right.jpg",  // GL_TEXTURE_CUBE_MAP_POSITIVE_X
    "left.jpg",   // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
    "top.jpg",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
    "bottom.jpg", // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
    "front.jpg",  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
    "back.jpg"    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

//disable depth writing
glDepthFunc(GL_LEQUAL);  // Change depth function so skybox passes

skyboxShader.use();
mat4 view = mat4(mat3(camera.getViewMatrix())); // Remove translation
skyboxShader.setMat4("view", view);
skyboxShader.setMat4("projection", projection);

glBindVertexArray(skyboxVAO);
glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
glDrawArrays(GL_TRIANGLES, 0, 36);

glDepthFunc(GL_LESS); // Restore default


// Reflections in fragment shader
vec3 I = normalize(fs_in.FragPos - viewPos);      // Incident view vector
vec3 R = reflect(I, normalize(fs_in.Normal));     // Reflection vector
vec3 reflection = texture(skybox, R).rgb;         // Sample the cubemap