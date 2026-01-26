#include "backend_impl_gl.hpp"

namespace stardraw
{
    std::tuple<GLenum, GLuint, bool, bool> gl_vertex_element_data_type(const vertex_element_type& type)
    {
        switch (type)
        {
            case UINT_U8: return { GL_UNSIGNED_BYTE, 1, false, true};
            case UNT2_U8: return { GL_UNSIGNED_BYTE, 2, false, true};
            case UINT3_U8: return { GL_UNSIGNED_BYTE, 3, false, true};
            case UINT4_U8: return { GL_UNSIGNED_BYTE, 4, false, true};
            case UINT_U16: return { GL_UNSIGNED_SHORT, 1, false, true};
            case UINT2_U16: return { GL_UNSIGNED_SHORT, 2, false, true};
            case UINT3_U16: return { GL_UNSIGNED_SHORT, 3, false, true};
            case UINT4_U16: return { GL_UNSIGNED_SHORT, 4, false, true};
            case UINT_U32: return { GL_UNSIGNED_INT, 1, false, true};
            case UINT2_U32: return { GL_UNSIGNED_INT, 2, false, true};
            case UINT3_U32: return { GL_UNSIGNED_INT, 3, false, true};
            case UINT4_U32: return { GL_UNSIGNED_INT, 4, false, true};
            case INT_I8: return { GL_BYTE, 1, false, true};
            case INT2_I8: return { GL_BYTE, 2, false, true};
            case INT3_I8: return { GL_BYTE, 3, false, true};
            case INT4_I8: return { GL_BYTE, 4, false, true};
            case INT_I16: return { GL_SHORT, 1, false, true};
            case INT2_I16: return { GL_SHORT, 2, false, true};
            case INT3_I16: return { GL_SHORT, 3, false, true};
            case INT4_I16: return { GL_SHORT, 4, false, true};
            case INT_I32: return { GL_INT, 1, false, true};
            case INT2_I32: return { GL_INT, 2, false, true};
            case INT3_I32: return { GL_INT, 3, false, true};
            case INT4_I32: return { GL_INT, 4, false, true};
            case FLOAT_U8_NORM: return { GL_UNSIGNED_BYTE, 1, true, false};
            case FLOAT2_U8_NORM: return { GL_UNSIGNED_BYTE, 2, true, false};
            case FLOAT3_U8_NORM: return { GL_UNSIGNED_BYTE, 3, true, false};
            case FLOAT4_U8_NORM: return { GL_UNSIGNED_BYTE, 4, true, false};
            case FLOAT_I8_NORM: return { GL_BYTE, 1, true, false};
            case FLOAT2_I8_NORM: return { GL_BYTE, 2, true, false};
            case FLOAT3_I8_NORM: return { GL_BYTE, 3, true, false};
            case FLOAT4_I8_NORM: return { GL_BYTE, 4, true, false};
            case FLOAT_U16_NORM: return { GL_UNSIGNED_SHORT, 1, true, false};
            case FLOAT2_U16_NORM: return { GL_UNSIGNED_SHORT, 2, true, false};
            case FLOAT3_U16_NORM: return { GL_UNSIGNED_SHORT, 3, true, false};
            case FLOAT4_U16_NORM: return { GL_UNSIGNED_SHORT, 4, true, false};
            case FLOAT_I16_NORM: return { GL_SHORT, 1, true, false};
            case FLOAT2_I16_NORM: return { GL_SHORT, 2, true, false};
            case FLOAT3_I16_NORM: return { GL_SHORT, 3, true, false};
            case FLOAT4_I16_NORM: return { GL_SHORT, 4, true, false};
            case FLOAT_F16: return { GL_HALF_FLOAT, 1, false, false};
            case FLOAT2_F16: return { GL_HALF_FLOAT, 2, false, false};
            case FLOAT3_F16: return { GL_HALF_FLOAT, 3, false, false};
            case FLOAT4_F16: return { GL_HALF_FLOAT, 4, false, false};
            case FLOAT_F32: return { GL_FLOAT, 1, false, false};
            case FLOAT2_F32: return { GL_FLOAT, 2, false, false};
            case FLOAT3_F32: return { GL_FLOAT, 3, false, false};
            case FLOAT4_F32: return { GL_FLOAT, 4, false, false};
        }
        return {-1, -1, false, false};
    }

    [[nodiscard]] status backend_impl_gl::execute_command(const command* cmd)
    {
        if (cmd == nullptr)
        {
            return { status_type::UNEXPECTED_NULL, "Null command" };
        }

        const command_type type = cmd->type();
        switch (type)
        {
            case command_type::DRAW: return execute_draw_cmd(dynamic_cast<const draw_command*>(cmd));
            case command_type::DRAW_INDEXED: return execute_draw_indexed(dynamic_cast<const draw_indexed_command*>(cmd));
            case command_type::DRAW_INDIRECT: return execute_draw_indirect(dynamic_cast<const draw_indirect_command*>(cmd));
            case command_type::DRAW_INDEXED_INDIRECT: return execute_draw_indexed_indirect(dynamic_cast<const draw_indexed_indirect_command*>(cmd));

            case command_type::CONFIG_BLENDING: return execute_config_blending(dynamic_cast<const config_blending_command*>(cmd));
            case command_type::CONFIG_STENCIL: return execute_config_stencil(dynamic_cast<const config_stencil_command*>(cmd));
            case command_type::CONFIG_SCISSOR: return execute_config_scissor(dynamic_cast<const config_scissor_command*>(cmd));
            case command_type::CONFIG_FACE_CULL: return execute_config_face_cull(dynamic_cast<const config_face_cull_command*>(cmd));
            case command_type::CONFIG_DEPTH_TEST: return execute_config_depth_test(dynamic_cast<const config_depth_test_command*>(cmd));
            case command_type::CONFIG_DEPTH_RANGE: return execute_config_depth_range(dynamic_cast<const config_depth_range_command*>(cmd));

            case command_type::BUFFER_UPLOAD: return execute_buffer_upload(dynamic_cast<const buffer_upload_command*>(cmd));
            case command_type::BUFFER_COPY: return execute_buffer_copy(dynamic_cast<const buffer_copy_command*>(cmd));
            case command_type::BUFFER_DOWNLOAD: return status_type::UNIMPLEMENTED; //TODO
            case command_type::BUFFER_ATTACH: return execute_buffer_attach(dynamic_cast<const buffer_attach_command*>(cmd));

            case command_type::CLEAR_WINDOW: return execute_clear_window(dynamic_cast<const clear_window_command*>(cmd));
            case command_type::CLEAR_BUFFER: return status_type::UNIMPLEMENTED; //TODO
        }

        return{ status_type::UNSUPPORTED, "Unsupported command" };
    }

    [[nodiscard]] status backend_impl_gl::execute_command_buffer(const std::string_view& name)
    {
        //Opengl doesn't have any persistant command buffers, so we just execute it like a temporary one without consuming it.
        if (!command_lists.contains(std::string(name))) return status_type::UNKNOWN_NAME;
        const command_list_ptr& refren = command_lists[std::string(name)];

        for (const command* cmd : *refren.get())
        {
            const status result = execute_command(cmd);
            if (is_error_status(result)) return result;
        }

        return status_type::SUCCESS;
    }

    [[nodiscard]] status backend_impl_gl::execute_temp_command_buffer(const command_list_ptr commands)
    {
        if (commands.get() == nullptr)
        {
            return { status_type::UNEXPECTED_NULL, "Null command buffer" };
        }

        for (const command* cmd : *commands)
        {
            const status result = execute_command(cmd);
            if (is_error_status(result)) return result;
        }

        return status_type::SUCCESS;
    }

    [[nodiscard]] status backend_impl_gl::create_command_buffer(const std::string_view& name, command_list_ptr commands)
    {
        if (command_lists.contains(std::string(name))) return  { status_type::DUPLICATE_NAME, std::format("A command buffer named '{0}' already exists", name) };
        command_lists[std::string(name)] = std::move(commands);
        return status_type::SUCCESS;
    }

    [[nodiscard]] status backend_impl_gl::delete_command_buffer(const std::string_view& name)
    {
        if (!command_lists.contains(std::string(name))) return status_type::NOTHING_TO_DO;
        command_list_ptr cmd_list = std::move(command_lists[std::string(name)]);
        command_lists.erase(std::string(name));
        cmd_list.reset();
        return status_type::SUCCESS;
    }

    [[nodiscard]] status backend_impl_gl::create_object(const descriptor* descriptor)
    {
        const descriptor_type type = descriptor->type();
        switch (type)
        {
            case descriptor_type::BUFFER: return create_buffer_state(dynamic_cast<const buffer_descriptor*>(descriptor));
            case descriptor_type::VERTEX_SPECIFICATION: return create_vertex_specification_state(dynamic_cast<const vertex_specification_descriptor*>(descriptor));
        }
        return status_type::UNIMPLEMENTED;
    }

    [[nodiscard]] status backend_impl_gl::create_objects(const descriptor_list_ptr descriptors)
    {
        if (descriptors.get() == nullptr)
        {
            return { status_type::UNEXPECTED_NULL, "Null descriptor list" };
        }

        for (const descriptor* descriptor : *descriptors)
        {
            const status create_status = create_object(descriptor);
            if (is_error_status(create_status)) return create_status;
        }

        return status_type::SUCCESS;
    }

    [[nodiscard]] status backend_impl_gl::delete_object(const std::string_view& name)
    {
        const object_identifier identifier = object_identifier(name);
        if (!objects.contains(identifier.hash)) return status_type::NOTHING_TO_DO;

        const gl_object_state* state = objects[identifier.hash];
        objects.erase(identifier.hash);
        delete state;

        return status_type::SUCCESS;
    }

    [[nodiscard]] signal_status backend_impl_gl::check_signal(const std::string_view& name)
    {
        return wait_signal(name, 0);
    }

    [[nodiscard]] signal_status backend_impl_gl::wait_signal(const std::string_view& name, const uint64_t timeout)
    {
        if (!signals.contains(std::string(name)))
        {
            return signal_status::UNKNOWN_SIGNAL;
        }

        const gl_signal_state& state = signals[std::string(name)];
        const GLenum status = glClientWaitSync(state.sync_point, 0, timeout);
        switch (status)
        {
            case GL_ALREADY_SIGNALED: return signal_status::SIGNALLED;
            case GL_TIMEOUT_EXPIRED: return signal_status::TIMED_OUT;
            case GL_CONDITION_SATISFIED: return signal_status::SIGNALLED;
            default: return signal_status::NOT_SIGNALLED;
        }
    }

    [[nodiscard]] status backend_impl_gl::bind_buffer(const object_identifier& source, const GLenum target)
    {
        const gl_buffer_state* buffer_state = find_gl_buffer_state(source);
        if (buffer_state == nullptr) return { status_type::UNKNOWN_SOURCE, std::format("No buffer with name '{0}' exists in pipeline", source.name) };
        return buffer_state->bind_to(target);
    }

    [[nodiscard]] status backend_impl_gl::create_buffer_state(const buffer_descriptor* descriptor)
    {
        gl_buffer_state* buffer_state = new gl_buffer_state(*descriptor);
        if (!buffer_state->is_valid())
        {
            delete buffer_state;
            return { status_type::BACKEND_FAILURE, std::format("Attempting to create buffer '{0}' resulted in an invalid buffer", descriptor->identifier().name) };
        }

        objects[descriptor->identifier().hash] = buffer_state;

        return status_type::SUCCESS;
    }

    status backend_impl_gl::create_vertex_specification_state(const vertex_specification_descriptor* descriptor)
    {
        gl_vertex_specification_state* vertex_specification_state = new gl_vertex_specification_state();
        if (vertex_specification_state->vertex_array_id == 0)
        {
            delete vertex_specification_state;
            return { status_type::BACKEND_FAILURE, std::format("Attempting to create vertex specification '{0}' resulted in an invalid buffer", descriptor->identifier().name) };
        }

        const vertex_buffers_specification& buffers = descriptor->buffers;
        const vertex_format_specification& format = descriptor->format;
        std::vector<GLsizeiptr> buffer_strides;
        std::unordered_map<std::string, GLuint> buffer_slots;
        std::unordered_map<std::string, gl_buffer_state*> buffer_states;

        GLuint buffer_slot = 0;
        for (std::string buffer_name : buffers.vertex_sources)
        {
            buffer_slots[buffer_name] = buffer_slot;
            gl_buffer_state* buffer_state = find_gl_buffer_state(object_identifier(buffer_name));
            if (buffer_state == nullptr)
            {
                delete vertex_specification_state;
                return { status_type::UNKNOWN_SOURCE, std::format("No buffer named '{0}' found while creating vertex specification '{1}'", buffer_name, descriptor->identifier().name)};
            }
            buffer_states[buffer_name] = buffer_state;
            buffer_slot++;
        }

        buffer_strides.resize(buffer_slot);

        const GLuint vao_id = vertex_specification_state->vertex_array_id;

        for (uint32_t idx = 0; idx < format.elements.size(); idx++)
        {
            const vertex_element& elem = format.elements[idx];
            GLsizeiptr& offset = buffer_strides[buffer_slots[elem.buffer_source]];

            glEnableVertexArrayAttrib(vao_id, idx);
            glVertexArrayAttribBinding(vao_id, idx, buffer_slots[elem.buffer_source]);
            if (elem.instance_divisor > 0) glVertexArrayBindingDivisor(vao_id, idx, elem.instance_divisor);

            auto [type, count, normalized, integer] = gl_vertex_element_data_type(elem.type);

            if (integer)
            {
                glVertexArrayAttribIFormat(vao_id, idx, count, type, offset);
            }
            else if (type == GL_DOUBLE)
            {
                glVertexArrayAttribLFormat(vao_id, idx, count, type, offset);
            }
            else
            {
                glVertexArrayAttribFormat(vao_id, idx, count, type, normalized, offset);
            }

            offset += vertex_element_size(elem.type);
        }

        for (const std::string& vertex_buffer : buffers.vertex_sources)
        {
            const gl_buffer_state* buffer_state = buffer_states[vertex_buffer];
            const status attach_status = vertex_specification_state->attach_vertex_buffer(buffer_slots[vertex_buffer], buffer_state->gl_id(), 0, buffer_strides[buffer_slots[vertex_buffer]]);

            if (is_error_status(attach_status))
            {
                delete vertex_specification_state;
                return attach_status;
            }
        }

        if (!buffers.index_source.empty())
        {
            const gl_buffer_state* index_buffer_state = find_gl_buffer_state(object_identifier(buffers.index_source));
            if (index_buffer_state == nullptr)
            {
                delete vertex_specification_state;
                return { status_type::UNKNOWN_SOURCE, std::format("No buffer named '{0}' found while creating vertex specification '{1}'", buffers.index_source, descriptor->identifier().name)};
            }

            const status attach_status = vertex_specification_state->attach_index_buffer(index_buffer_state->gl_id());

            if (is_error_status(attach_status))
            {
                delete vertex_specification_state;
                return attach_status;
            }
        }

        if (!vertex_specification_state->is_valid())
        {
            delete vertex_specification_state;
            return { status_type::BACKEND_FAILURE, std::format("Creating vertex specification '{0}' resulted in an invalid object", descriptor->identifier().name)};
        }

        objects[descriptor->identifier().hash] = vertex_specification_state;

        return status_type::SUCCESS;
    }

    status backend_impl_gl::bind_vertex_specification_state(const object_identifier& source, GLsizeiptr& out_index_buffer_offset)
    {
        out_index_buffer_offset = 0;
        const gl_vertex_specification_state* state = find_gl_vertex_specification_state(source);
        if (state == nullptr) return { status_type::UNKNOWN_SOURCE, std::format("No vertex specification with name '{0}' exists in pipeline", source.name) };
        if (!state->is_valid()) return { status_type::BROKEN_SOURCE, std::format("Vertex specification object '{0}' is in an invalid state", source.name) };
        return state->bind();
    }
}
