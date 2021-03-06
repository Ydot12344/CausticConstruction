#include"Model.h"

// Draw model
void Model::Draw(Shader& shader) const{
	for (const auto& mesh : meshes)
		mesh.Draw(shader);
}

// load model from .obj file
void Model::loadModel(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = (importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace));

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find('/'));

	processNode(scene->mRootNode, scene);


}

// parse assimp nodes of scene
void Model::processNode(aiNode* node, const aiScene* scene) {
	for (size_t i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = (scene->mMeshes[node->mMeshes[i]]);
		meshes.push_back(processMesh(mesh, scene));
	}

	for (size_t i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
	}
}

// parse assimp meshes
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex>	verticies;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (size_t i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vert;

		glm::vec3 tmp;
		tmp.x = mesh->mVertices[i].x;
		tmp.y = mesh->mVertices[i].y;
		tmp.z = mesh->mVertices[i].z;
		vert.Position = tmp;



		tmp.x = mesh->mNormals[i].x;
		tmp.y = mesh->mNormals[i].y;
		tmp.z = mesh->mNormals[i].z;
		vert.Normal = tmp;

		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vert.TexCoords = vec;
		}
		else {
			vert.TexCoords = glm::vec2(0.f, 0.f);
		}

		verticies.push_back(vert);
	}

	for (size_t i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = (scene->mMaterials[mesh->mMaterialIndex]);
		std::vector<Texture> diffuseMap = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
		textures.insert(textures.end(), diffuseMap.begin(), diffuseMap.end());

		std::vector<Texture> specularMap = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
		textures.insert(textures.end(), specularMap.begin(), specularMap.end());
	}

	return Mesh(verticies, indices, textures);
}

// load textures for each meshes
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType) {
	std::vector<Texture> textures;

	for (size_t i = 0; i < mat->GetTextureCount(type); ++i) {
		aiString str;
		mat->GetTexture(type, i, &str);

		if (loaded_textures.find(str.C_Str()) != loaded_textures.end()) {
			textures.push_back(loaded_textures[str.C_Str()]);
		}
		else {
			Texture texture;
			texture.type = textureType;
			std::string path = directory + '/' + str.C_Str();
			texture.id = loadTexture(path.c_str());
			texture.path = str;
			textures.push_back(texture);
			loaded_textures[str.C_Str()] = texture;
		}
	}

	return textures;
}
