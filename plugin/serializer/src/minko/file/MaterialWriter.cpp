/*
Copyright (c) 2014 Aerys

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

#include "minko/file/MaterialWriter.hpp"
#include "minko/log/Logger.hpp"
#include "minko/Flyweight.hpp"
#include "minko/Any.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"

#include "google/sparse_hash_map"

using namespace minko;
using namespace minko::file;
using namespace minko::render;

std::map<const std::type_info*, std::function<std::tuple<uint, std::string>(Any)>> MaterialWriter::_typeToWriteFunction;


MaterialWriter::MaterialWriter()
{
	_magicNumber = 0x0000004D | MINKO_SCENE_MAGIC_NUMBER;

	_typeToWriteFunction[&typeid(math::mat4)]		        = std::bind(&serialize::TypeSerializer::serializeMatrix4x4, std::placeholders::_1);
	_typeToWriteFunction[&typeid(math::vec2)]		        = std::bind(&serialize::TypeSerializer::serializeVector2, std::placeholders::_1);
	_typeToWriteFunction[&typeid(math::vec3)]		        = std::bind(&serialize::TypeSerializer::serializeVector3, std::placeholders::_1);
	_typeToWriteFunction[&typeid(math::vec4)]		        = std::bind(&serialize::TypeSerializer::serializeVector4, std::placeholders::_1);
	_typeToWriteFunction[&typeid(render::Blending::Mode)]	= std::bind(&serialize::TypeSerializer::serializeBlending, std::placeholders::_1);
    _typeToWriteFunction[&typeid(render::TriangleCulling)] = std::bind(&serialize::TypeSerializer::serializeCulling, std::placeholders::_1);
    _typeToWriteFunction[&typeid(std::string)] = std::bind(&serialize::TypeSerializer::serializeString, std::placeholders::_1);
}

std::string
MaterialWriter::embed(std::shared_ptr<AssetLibrary>		assetLibrary,
					  std::shared_ptr<Options>			options,
					  Dependency::Ptr					dependency,
                      std::shared_ptr<WriterOptions>    writerOptions,
                      std::vector<unsigned char>&       embeddedHeaderData)
{
	material::Material::Ptr				material = std::dynamic_pointer_cast<material::Material>(data());
	std::vector<ComplexPropertyTuple>	serializedComplexProperties;
	std::vector<BasicPropertyTuple>		serializedBasicProperties;

	for (const auto& value : material->data()->values())
	{
		std::string propertyName = *value.first;

		if (serializeMaterialValue<uint>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<int>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<unsigned short>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<short>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<unsigned char>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<char>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<float>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<bool>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<render::Blending::Mode>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<render::TriangleCulling>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<math::vec2>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<math::vec3>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<math::vec4>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<math::mat4>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
		else if (serializeMaterialValue<TextureSampler>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
			continue;
        else if (serializeMaterialValue<std::string>(material, propertyName, assetLibrary, &serializedComplexProperties, &serializedBasicProperties, dependency))
            continue;
        else
        {
            LOG_DEBUG(propertyName << " can't be serialized : missing technique");
        }
	}

	msgpack::type::tuple<std::vector<ComplexPropertyTuple>, std::vector<BasicPropertyTuple>> res(
		serializedComplexProperties, serializedBasicProperties);

	std::stringstream sbuf;
	msgpack::pack(sbuf, res);

	return sbuf.str();
}
