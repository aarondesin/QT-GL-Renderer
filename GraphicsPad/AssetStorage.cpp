#pragma once

#include <global.h>
#include <AssetStorage.h>
#include <Cubemap.h>
#include <Framebuffer.h>
#include <ShapeData.h>
#include <Material.h>

map<string, QImage*>* AssetStorage::images = NULL;
map<string, Texture*>* AssetStorage::textures = NULL;
map<string, Cubemap*>* AssetStorage::cubemaps = NULL;
map<string, Framebuffer*>* AssetStorage::framebuffers = NULL;
map<string, ShapeData*>* AssetStorage::geometries = NULL;
map<string, Material*>* AssetStorage::materials = NULL;