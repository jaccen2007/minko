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

#pragma once

#include "minko/Signal.hpp"

namespace minko
{
    namespace audio
    {
        class Sound;
        class SoundTransform;

        class SoundChannel :
            public std::enable_shared_from_this<SoundChannel>
        {
        public:
            typedef std::shared_ptr<SoundChannel>    Ptr;

            friend class Sound;

            Signal<Ptr>::Ptr
            complete()
            {
                return _complete;
            }

            std::shared_ptr<Sound>
            sound() const
            {
                return _sound;
            }

            virtual
            Ptr
            transform(std::shared_ptr<SoundTransform> value)
            {
                _transform = value;

                return shared_from_this();
            }

            virtual
            std::shared_ptr<SoundTransform>
            transform() const
            {
                return _transform;
            }

            virtual
            void
            stop() = 0;

            ~SoundChannel()
            {
            }

        protected:
            SoundChannel(std::shared_ptr<Sound> sound) :
                _complete(Signal<Ptr>::create()),
                _sound(sound),
                _transform(nullptr)
            {
            }

            Signal<Ptr>::Ptr _complete;
            std::shared_ptr<Sound> _sound;
            std::shared_ptr<SoundTransform> _transform;
        };
    }
}
