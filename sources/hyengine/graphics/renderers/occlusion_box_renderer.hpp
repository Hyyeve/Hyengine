#pragma once

namespace hyengine
{
    class occlusion_box_renderer
    {
    public:
        explicit occlusion_box_renderer();

        occlusion_box_renderer(occlusion_box_renderer& other) = delete;                //COPY CONSTRUCTOR
        occlusion_box_renderer(occlusion_box_renderer&& other) = delete;                     //MOVE CONSTRUCTOR
        occlusion_box_renderer& operator=(const occlusion_box_renderer& other) = delete;     //COPY ASSIGNMENT
        occlusion_box_renderer& operator=(occlusion_box_renderer&& other) noexcept = delete; //MOVE ASSIGNMENT

        ~occlusion_box_renderer();

        //[[nodiscard]] bool allocate(const u32 memory_budget_mb);
        void free();

        //void update_shader_uniforms(const f32 interpolation_delta, const camera& cam) const;
        void reload_shaders() const;

        void bind() const;
        void draw() const;

    private:
    };
}
