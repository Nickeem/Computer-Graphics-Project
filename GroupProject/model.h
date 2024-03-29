#pragma once
// Basic Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

//Includes
#include <GL/glew.h>      
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"


GLint TextureFromFile(const char* path, bool gamma = false);


class Model 
    {
    public:
        Model(char* path)
        {
            loadModel(path);
        }
        void Draw(Shader shader)
        {
            for (GLuint i = 0; i < this->meshes.size(); i++)
                this->meshes[i].Draw(shader);
        }
        vector<Texture> textures_loaded;	// Stores textures loaded (loaded only once)
        vector<Mesh> meshes;

    private:
        // model data
        
        string directory;

        void loadModel(string path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
            string typeName);
    
    
};


void Model::loadModel(string path)
    {
        // Read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        // Check for errors if not 0
        if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
            {
                cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
                return;
            }
        // Retrieve the directory path of the filepath
        this->directory = path.substr(0, path.find_last_of('/'));

        // Process ASSIMP's root node recursively
        this->processNode(scene->mRootNode, scene);
    }




void Model::processNode(aiNode* node, const aiScene* scene)
    {
        // Process each mesh located at the current node
        for(GLuint i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
                this->meshes.push_back(this->processMesh(mesh, scene));			
            }
        
        // each of the children nodes
        for(GLuint i = 0; i < node->mNumChildren; i++)
            {
                this->processNode(node->mChildren[i], scene);
            }
    }



Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;

        for(GLuint i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex;
                glm::vec3 vector;   //Declare a placeholder vector
                // Positions
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.Position = vector;
                
                // Normals
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
                
                // Texture Coordinates
                if(mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
                    {
                        glm::vec2 vec;
                        vec.x = mesh->mTextureCoords[0][i].x; 
                        vec.y = mesh->mTextureCoords[0][i].y;
                        vertex.TexCoords = vec;
                    }
                else
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                
                vertices.push_back(vertex);
            }
        
       
        for(GLuint i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                
                for(GLuint j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }
        
        // Process materials
           if(mesh->mMaterialIndex >= 0)   
            {
               aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
               vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE,
                                                                         "texture_diffuse");
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
                vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR,
                                                                          "texture_specular");
                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            }
        
        // Return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }



// Checks all material textures of a given type and loads the textures 
vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);
                GLboolean skip = false;

                for(GLuint j = 0; j <  textures_loaded.size(); j++)
                    {
                        if(textures_loaded[j].path == str)
                            {
                                textures.push_back(textures_loaded[j]);
                                skip = true;    // A texture with the same filepath has already been found.
                                break;
                            }
                    }
                if(!skip)
                    {   // If texture hasn't been loaded already, load it
                        Texture texture;
                        texture.id = TextureFromFile(str.C_Str());
                        texture.type = typeName;
                        texture.path = str;
                        textures.push_back(texture);
                        this->textures_loaded.push_back(texture);   // not load duplicate textures.
                    }
            }
        return textures;
    }





// THIS PART IS FROM SIR'S Model.h

GLint TextureFromFile(const char* texturePath, bool gamma)
    {
         //Generate texture ID and load texture data 
        string filename = string(texturePath);
        GLuint textureID;
        glGenTextures(1, &textureID);
        int width,height;
        unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
        
        // Assign texture to ID
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, gamma ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB,
                      GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);	

        // Parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        SOIL_free_image_data(image);
        return textureID;
    }
