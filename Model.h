#pragma once
#include"Shader.h"
#include"Mesh.h"
#include<vector>
#include<SOIL.h>
#include<unordered_map>
#include"Utilitis.h"

//Assimp
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

class Model {
public:
	Model(const std::string& path) {
		loadModel(path);
	}

	void Draw(Shader& shader);

	std::vector<Mesh> meshes;
	std::string directory;
	std::unordered_map<std::string, Texture> loaded_textures;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType);
};