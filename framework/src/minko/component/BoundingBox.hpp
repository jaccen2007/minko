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

#pragma once

#include "minko/Common.hpp"

#include "minko/component/AbstractComponent.hpp"
#include "minko/Signal.hpp"
#include "minko/math/Vector3.hpp"

namespace minko
{
	namespace component
	{
		class BoundingBox :
			public AbstractComponent,
			public std::enable_shared_from_this<BoundingBox>
		{
		public:
			typedef std::shared_ptr<BoundingBox>	Ptr;

		private:
			typedef std::shared_ptr<scene::Node>		NodePtr;
			typedef std::shared_ptr<AbstractComponent>	AbsCmpPtr;

		private:
			const bool							_fixed;

			std::shared_ptr<math::Box>			_box;
			std::shared_ptr<math::Box>			_worldSpaceBox;

			Signal<AbsCmpPtr, NodePtr>::Slot	_targetAddedSlot;
			Signal<AbsCmpPtr, NodePtr>::Slot	_targetRemovedSlot;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<BoundingBox>(new BoundingBox());
			}

			inline static
			Ptr
			create(float size, std::shared_ptr<math::Vector3> center)
			{
				return create(size, size, size, center);
			}

			inline static
			Ptr
			create(float width, float height, float depth, std::shared_ptr<math::Vector3> center)
			{
				return create(
					math::Vector3::create(
						center->x() - width * .5f, center->y() - height * .5f, center->z() - depth * .5f
					),
					math::Vector3::create(
						center->x() + width * .5f, center->y() + height * .5f, center->z() + depth * .5f
					)
				);
			}

			inline static
			Ptr
			create(std::shared_ptr<math::Vector3> topRight, std::shared_ptr<math::Vector3> bottomLeft)
			{
				return std::shared_ptr<BoundingBox>(new BoundingBox(topRight, bottomLeft));
			}

			inline
			std::shared_ptr<math::AbstractShape>
			shape()
			{
				return std::static_pointer_cast<math::AbstractShape>(_worldSpaceBox);
			}

			inline
			std::shared_ptr<math::Box>
			box()
			{
				return _worldSpaceBox;
			}

			void
			update();

		private:
			BoundingBox(std::shared_ptr<math::Vector3> topRight, std::shared_ptr<math::Vector3> bottomLeft);

			BoundingBox();

			void
			initialize();
		};
	}
}