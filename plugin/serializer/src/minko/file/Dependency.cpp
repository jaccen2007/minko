/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/file/Dependency.hpp"
#include "minko/file/GeometryWriter.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/file/MaterialWriter.hpp"

using namespace minko;
using namespace minko::file;


bool
Dependency::hasDependency(std::shared_ptr<render::Effect> effect)
{
	return _effectDependencies.find(effect) != _effectDependencies.end();
}

uint
Dependency::registerDependency(std::shared_ptr<render::Effect> effect)
{
	if (!hasDependency(effect))
		_effectDependencies[effect] = _currentId++;

	return _effectDependencies[effect];
}

bool
Dependency::hasDependency(std::shared_ptr<geometry::Geometry> geometry)
{
	return _geometryDependencies.find(geometry) != _geometryDependencies.end();
}

uint
Dependency::registerDependency(std::shared_ptr<geometry::Geometry> geometry)
{
	if (!hasDependency(geometry))
		_geometryDependencies[geometry] = _currentId++;

	return _geometryDependencies[geometry];
}

bool
Dependency::hasDependency(std::shared_ptr<data::Provider> material)
{
	return _materialDependencies.find(material) != _materialDependencies.end();
}

uint
Dependency::registerDependency(std::shared_ptr<data::Provider> material)
{
	if (!hasDependency(material))
		_materialDependencies[material] = _currentId++;

	return _materialDependencies[material];
}

bool
Dependency::hasDependency(AbsTexturePtr texture)
{
	return _textureDependencies.find(texture) != _textureDependencies.end();
}

uint
Dependency::registerDependency(AbsTexturePtr texture)
{
	if (!hasDependency(texture))
		_textureDependencies[texture] = _currentId++;

	return _textureDependencies[texture];
}

bool
Dependency::hasDependency(std::shared_ptr<scene::Node> subScene)
{
	return _subSceneDependencies.find(subScene) != _subSceneDependencies.end();
}

uint
Dependency::registerDependency(std::shared_ptr<scene::Node> subScene)
{
	if (!hasDependency(subScene))
		_subSceneDependencies[subScene] = _currentId++;

	return _subSceneDependencies[subScene];
}

std::shared_ptr<geometry::Geometry>
Dependency::getGeometryReference(uint geometryId)
{
	return _geometryReferences[geometryId];
}

void
Dependency::registerReference(uint referenceId, std::shared_ptr<geometry::Geometry> geometry)
{
	_geometryReferences[referenceId] = geometry;
}

std::shared_ptr<data::Provider>
Dependency::getMaterialReference(uint materialId)
{
	return _materialReferences[materialId];
}

void
Dependency::registerReference(uint referenceId, std::shared_ptr<data::Provider> material)
{
	_materialReferences[referenceId] = material;
}

Dependency::AbsTexturePtr
Dependency::getTextureReference(uint textureId)
{
	return _textureReferences[textureId];
}

void
Dependency::registerReference(uint referenceId, AbsTexturePtr texture)
{
	_textureReferences[referenceId] = texture;
}

std::shared_ptr<scene::Node>
Dependency::getSubsceneReference(uint subSceneId)
{
	return _subSceneReferences[subSceneId];
}

void
Dependency::registerReference(uint referenceId, std::shared_ptr<scene::Node> subScene)
{
	_subSceneReferences[referenceId] = subScene;
}

void
Dependency::registerReference(uint referenceId, std::shared_ptr<render::Effect> effect)
{
	_effectReferences[referenceId] = effect;
}

std::shared_ptr<render::Effect>
Dependency::getEffectReference(uint effectId)
{
	return _effectReferences[effectId];
}

std::vector<msgpack::type::tuple<short, short, std::string>>
Dependency::serialize(std::shared_ptr<file::AssetLibrary>	assetLibrary,
					  std::shared_ptr<file::Options>		options)
{
	GeometryWriter::Ptr geometryWriter = GeometryWriter::create();
	MaterialWriter::Ptr materialWriter = MaterialWriter::create();

	std::vector<msgpack::type::tuple<short, short, std::string>> serializedAsset;

	auto itGeometry = _geometryDependencies.begin();
	auto itMaterial = _materialDependencies.begin();
	auto itTexture	= _textureDependencies.begin();
	auto itEffect	= _effectDependencies.begin();

	while (itGeometry != _geometryDependencies.end())
	{
        std::string content;

        geometryWriter->data(itGeometry->first);

        if (options->embedAll())
        {
            auto geometryDependency = Dependency::create();

            content = geometryWriter->embed(assetLibrary, options, geometryDependency);
        }
        else
        {
            auto filename = assetLibrary->geometryName(itGeometry->first) + ".geometry";

            geometryWriter->write(filename, assetLibrary, options);

            content = filename;
        }

		msgpack::type::tuple<short, short, std::string> res(0, itGeometry->second, content);
		serializedAsset.push_back(res);
		itGeometry++;
	}

	while (itMaterial != _materialDependencies.end())
	{
        std::string content;

        materialWriter->data(itMaterial->first);

        if (options->embedAll())
        {
            auto materialDependency = Dependency::create();

            content = materialWriter->embed(assetLibrary, options, materialDependency);
        }
        else
        {
            auto filename = assetLibrary->materialName(itMaterial->first) + ".material";

            materialWriter->write(filename, assetLibrary, options);
        }

		msgpack::type::tuple<short, short, std::string> res(1, itMaterial->second, content);
		serializedAsset.push_back(res);
		itMaterial++;
	}

	while (itTexture != _textureDependencies.end())
	{
#ifdef DEBUG
		std::string filenameInput	= "asset/" + assetLibrary->textureName(itTexture->first);
#else
		std::string filenameInput	= assetLibrary->textureName(itTexture->first);
#endif
        std::ifstream source(filenameInput, std::ios::binary);

        std::string content;

        if (options->embedAll())
        {
            content = std::string(std::istreambuf_iterator<char>(source),
                                  std::istreambuf_iterator<char>());
        }
        else
        {
            std::string filenameOutput	= "";

            for (int charIndex = filenameInput.size() - 1;
                 charIndex >= 0 && filenameInput[charIndex] != '/';
                 --charIndex)
            {
                filenameOutput.insert(0, filenameInput.substr(charIndex, 1));
            }

            std::ofstream dst(filenameOutput, std::ios::binary);

            dst << source.rdbuf();

            source.close();
            dst.close();

            content = filenameOutput;
        }

		msgpack::type::tuple<short, short, std::string> res(2, itTexture->second, content);

		serializedAsset.insert(serializedAsset.begin(), res);
		itTexture++;
	}

	while (itEffect != _effectDependencies.end())
	{
#ifdef DEBUG
		std::string filenameInput	= "bin/debug/" + assetLibrary->effectName(itEffect->first);
#else
		std::string filenameInput	= assetLibrary->effectName(itEffect->first);
#endif
		std::ifstream source(filenameInput, std::ios::binary);

        msgpack::type::tuple<short, short, std::string> dependencyRes;

        // TODO effect dependency management
        // copyEffectDependency(source, itEffect->first, dependencyRes, options->embedAll());

        std::string content;

        if (options->embedAll())
        {
            serializedAsset.insert(serializedAsset.begin(), dependencyRes);

            content = std::string(std::istreambuf_iterator<char>(source),
                                  std::istreambuf_iterator<char>());
        }
        else
        {
            std::string filenameOutput	= "";

            for (int charIndex = filenameInput.size() - 1;
                 charIndex >= 0 && filenameInput[charIndex] != '/';
                 --charIndex)
            {
                filenameOutput.insert(0, filenameInput.substr(charIndex, 1));
            }

            std::ofstream dst(filenameOutput, std::ios::binary);

            dst << source.rdbuf();

            source.close();
            dst.close();

            content = filenameOutput;
        }

		msgpack::type::tuple<short, short, std::string> res(3, itEffect->second, content);
		serializedAsset.insert(serializedAsset.begin(), res);
		itEffect++;
	}

	return serializedAsset;
}

void
Dependency::copyEffectDependency(const std::ifstream&                                   source,
                                 std::shared_ptr<render::Effect>                        effect,
                                 msgpack::type::tuple<short, short, std::string>&       result,
                                 bool                                                   embedAll)
{
	std::stringstream	effectContent;
	std::size_t			found;

	effectContent << source.rdbuf();

	uint i = 0;

	while ((found = effectContent.str().find(".glsl", i)) != std::string::npos)
	{
		uint position = found;

		while (effectContent.str()[position - 1] != '"')
			position--;

		std::string dependencyFile = effectContent.str().substr(position, found + 5 - position);

		std::cout << dependencyFile << std::endl;

#ifdef DEBUG
		std::ifstream dependencySource("bin/debug/effect/" + dependencyFile, std::ios::binary);
#else
		std::ifstream dependencySource("effect/" + dependencyFile, std::ios::binary);
#endif

        if (embedAll)
        {
            auto content = std::string(std::istreambuf_iterator<char>(dependencySource),
                                       std::istreambuf_iterator<char>());

            // TODO fill res
        }
        else
        {
            std::ofstream dst(dependencyFile, std::ios::binary);

            dst << dependencySource.rdbuf();

            dependencySource.close();
            dst.close();
        }

		i = (found + 6);
	}
}
