#include "api_backend.hpp"
#include "stardraw/gl/backend_impl_gl.hpp"

namespace stardraw
{
    api_backend* api_backend::create(const graphics_api api)
    {
        switch (api)
        {
            case graphics_api::GL: return new backend_impl_gl();
        }
        return nullptr;
    }
}
