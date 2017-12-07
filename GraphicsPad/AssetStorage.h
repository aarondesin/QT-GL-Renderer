#pragma once

#include <global.h>
#include <map>
#include <Texture.h>
#include <ShapeData.h>
#include <Cubemap.h>
#include <Material.h>
#include <Framebuffer.h>
using namespace std;

class AssetStorage
{
public:
	static void AssetStorage::addImage(string name, QImage* image)
	{
		std::pair<string, QImage*> pair =
			std::pair<string, QImage*>(name, image);

		if (images == NULL) images = new map<string, QImage*>;
		images->emplace(pair);
	}

	static void AssetStorage::addGeometry(string name, ShapeData* geo)
	{
		if (geo == NULL)
		{
			cout << "addGeometry: given null geometry!" << endl;
			return;
		}

		if (geometries == NULL) geometries = new map<string, ShapeData*>;
		geometries->emplace(std::pair<string, ShapeData*>(name, geo));
		cout << "Added geometry: " << name << endl;
	}

	static void AssetStorage::addTexture(string name, Texture* tex)
	{
		if (tex == NULL)
		{
			cout << "addTexture: given null texture!" << endl;
			return;
		}

		if (textures == NULL) textures = new map<string, Texture*>;
		textures->emplace(std::pair<string, Texture*>(name, tex));
	}

	static void AssetStorage::addCubemap(string name, Cubemap* cubemap)
	{
		if (cubemap == NULL)
		{
			cout << "addCubemap: given null cubemap!" << endl;
			return;
		}

		if (cubemaps == NULL) cubemaps = new map<string, Cubemap*>;
		cubemaps->emplace(std::pair<string, Cubemap*>(name, cubemap));
	}

	static void AssetStorage::addMaterial(string name, Material* mat)
	{
		if (mat == NULL)
		{
			cout << "addMaterial: given null material!" << endl;
			return;
		}

		if (materials == NULL) materials = new map<string, Material*>;
		materials->emplace(std::pair<string, Material*>(name, mat));
	}

	static void AssetStorage::addFramebuffer(string name, Framebuffer* framebuffer)
	{
		if (framebuffer == NULL)
		{
			cout << "addFramebuffer: given null framebuffer!" << endl;
			return;
		}

		if (framebuffers == NULL) framebuffers = new map<string, Framebuffer*>;
		framebuffers->emplace(std::pair<string, Framebuffer*>(name, framebuffer));
	}

	static ShapeData* AssetStorage::getGeometry(string name)
	{
		if (geometries == NULL)
		{
			cout << "Geometries map has not been initialized!" << endl;
			return NULL;
		}
		ShapeData* result = geometries->at(name);
		return result;
	}

	static Material* AssetStorage::getMaterial(string name)
	{
		if (materials == NULL)
		{
			cout << "Materials map has not been initialized!" << endl;
			return NULL;
		}
		return materials->at(name);
	}

	static Texture* AssetStorage::getTexture(string name)
	{
		if (textures == NULL)
		{
			cout << "Textures map has not been initialized!" << endl;
			return NULL;
		}
		return textures->at(name);
	}

	static Cubemap* AssetStorage::getCubemap(string name)
	{
		if (cubemaps == NULL)
		{
			cout << "Cubemaps map has not been initialized!" << endl;
			return NULL;
		}
		return cubemaps->at(name);
	}
private:
	static map<string, QImage*>* images;
	static map<string, Texture*>* textures;
	static map<string, Cubemap*>* cubemaps;
	static map<string, Framebuffer*>* framebuffers;
	static map<string, ShapeData*>* geometries;
	static map<string, Material*>* materials;
};