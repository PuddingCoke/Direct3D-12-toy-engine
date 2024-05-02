#pragma once

const std::string assetPath = "E:/Assets/Sponza";

#include"Model.h"
#include"Material.h"

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

class Scene
{
public:

	Scene(const std::string& filePath, ResourceManager* const resManager)
	{
		Assimp::Importer importer;

		const aiScene* const scene = importer.ReadFile(filePath, aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

		for (UINT materialIdx = 0; materialIdx < scene->mNumMaterials; materialIdx++)
		{
			const aiMaterial* const material = scene->mMaterials[materialIdx];

			aiString texturePath;
			std::string diffusePath;
			std::string specularPath;
			std::string normalPath;

			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
				diffusePath = assetPath + texturePath.C_Str();
			}
			else
			{
				diffusePath = assetPath + "/sponza/dummy.dds";
			}

			if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
			{
				material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
				specularPath = assetPath + texturePath.C_Str();
			}
			else
			{
				specularPath = assetPath + "/sponza/dummy_specular.dds";
			}

			if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
			{
				material->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
				normalPath = assetPath + texturePath.C_Str();
			}
			else
			{
				normalPath = assetPath + "/sponza/dummy_ddn.dds";
			}

			materials.push_back(new Material(resManager, diffusePath, specularPath, normalPath));
		}

		std::vector<Vertex> vertices;

		UINT startVertexLocation = 0;

		for (UINT meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++)
		{
			const aiMesh* const mesh = scene->mMeshes[meshIdx];

			const bool hasUV = mesh->HasTextureCoords(0);

			const bool hasTangent = mesh->HasTangentsAndBitangents();

			const UINT vertexCount = mesh->mNumVertices;

			for (UINT vertIdx = 0; vertIdx < mesh->mNumVertices; vertIdx++)
			{
				Vertex vert = {};

				{
					const aiVector3D& position = mesh->mVertices[vertIdx];

					vert.pos = DirectX::XMFLOAT3(position.x, position.y, position.z);
				}

				{
					const aiVector3D& normal = mesh->mNormals[vertIdx];

					vert.normal = DirectX::XMFLOAT3(normal.x, normal.y, normal.z);
				}

				if (hasUV)
				{
					const aiVector3D& uv = mesh->mTextureCoords[0][vertIdx];

					vert.uv = DirectX::XMFLOAT2(uv.x, uv.y);
				}
				else
				{
					vert.uv = DirectX::XMFLOAT2(0.f, 0.f);
				}

				if (hasTangent)
				{
					const aiVector3D& tangent = mesh->mTangents[vertIdx];

					const aiVector3D& binormal = mesh->mBitangents[vertIdx];

					vert.tangent = DirectX::XMFLOAT3(tangent.x, tangent.y, tangent.z);
					vert.binormal = DirectX::XMFLOAT3(binormal.x, binormal.y, binormal.z);
				}
				else
				{
					vert.tangent = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
					vert.binormal = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
				}

				vertices.push_back(vert);
			}

			models.push_back(new Model(mesh->mMaterialIndex, vertexCount, startVertexLocation));

			startVertexLocation += vertexCount;
		}

		modelBuffer = resManager->createStructuredBufferView(sizeof(Vertex), sizeof(Vertex) * vertices.size(), false, false, true, false, true, vertices.data());
	}

	~Scene()
	{
		delete modelBuffer;

		for (UINT i = 0; i < materials.size(); i++)
		{
			delete materials[i];
		}

		for (UINT i = 0; i < models.size(); i++)
		{
			delete models[i];
		}
	}

	void render(GraphicsContext* const context, ID3D12PipelineState* const pipelineState)
	{
		context->setPipelineState(pipelineState);
		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->setVertexBuffers(0, { modelBuffer->getVertexBuffer() });

		for (UINT i = 0; i < models.size(); i++)
		{
			materials[models[i]->materialIndex]->bind(context);
			models[i]->draw(context);
		}
	}

	//rasterize whole scene to a texture cube
	void renderCube(GraphicsContext* const context, ID3D12PipelineState* const pipelineState)
	{
		context->setPipelineState(pipelineState);
		context->setTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->setVertexBuffers(0, { modelBuffer->getVertexBuffer() });

		for (UINT i = 0; i < models.size(); i++)
		{
			materials[models[i]->materialIndex]->bind(context);
			models[i]->drawCube(context);
		}
	}

private:

	std::vector<Material*> materials;

	std::vector<Model*> models;

	BufferView* modelBuffer;

};