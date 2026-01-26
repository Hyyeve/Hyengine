#include "api_backend.hpp"
#include "stardraw/gl45/backend_impl.hpp"

namespace stardraw
{
    api_backend* api_backend::create(const graphics_api api)
    {
        switch (api)
        {
            case graphics_api::GL45: return new gl45::backend_impl();
        }
        return nullptr;
    }
}
