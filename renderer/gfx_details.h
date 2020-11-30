#pragma once

#include "gfx.h"
#include "Log.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <IdAllocator.h>
#include <variant.hpp>
#include "iterator_range.h"

namespace sprint {

namespace gfx {

namespace details {

static Config g_config;

using Uniform = mpark::variant<int, bool, float, Vec3, Vec4, Color, Matrix>;

struct SetUniformCommand {
    ShaderHandle shader_handle;
    std::string name;
    Uniform value;
};

struct SetTextureCommand {
    TextureHandle texture_handle;
    TexSlotId slot;
};

using DrawUnitCommand = mpark::variant<SetUniformCommand, SetTextureCommand>;

class DrawUnitCommandBuffer {
public:
    using iterator = DrawUnitCommand*;
    using const_iterator = const DrawUnitCommand*;

public:
    void Push(DrawUnitCommand command) {
        assert(commands_count_ < static_config::kMaxCommandCountPerDrawCall);
        commands_[commands_count_++] = std::move(command);
    }

    void Clear() {
        commands_count_ = 0;
    }

    iterator begin() { return commands_; }
    iterator end() { return commands_ + commands_count_; }

    const_iterator begin() const { return commands_; }
    const_iterator end() const { return commands_ + commands_count_; }

private:
    DrawUnitCommand commands_[static_config::kMaxCommandCountPerDrawCall];
    uint32_t commands_count_ = 0;
};

struct DrawUnit {
    Matrix transform = Matrix::Identity;
    IndexBufferHandle ib_handle = IndexBufferHandle::Invalid;
    VertexBufferHandle vb_handle = VertexBufferHandle::Invalid;
    ShaderHandle shader_handle = ShaderHandle::Invalid;
    CameraId camera_id = UINT16_MAX;
    DrawUnitCommandBuffer command_buffer;
};

static void Clear(DrawUnit& draw) {
    draw.transform = Matrix::Identity;
    draw.ib_handle = IndexBufferHandle::Invalid;
    draw.vb_handle = VertexBufferHandle::Invalid;
    draw.shader_handle = ShaderHandle::Invalid;
    draw.camera_id = UINT16_MAX;
    draw.command_buffer.Clear();
}

struct Camera {
    Matrix view;
    Matrix projection;
    ClearFlagMask clear_flag;
    Color clear_color;
};

struct RendererContext {
    Camera cameras[static_config::kCamerasCapacity];
};

class RendererAPI {
public:
    RendererAPI() = default;
    RendererAPI(const RendererAPI&) = delete;
    RendererAPI(RendererAPI&&) = delete;

    RendererAPI& operator=(const RendererAPI&) = delete;
    RendererAPI& operator=(RendererAPI&&) = delete;

    virtual ~RendererAPI() = default;

    virtual void CreateVertexBuffer(VertexBufferHandle, const void* data, uint32_t size, VertexLayout layout) = 0;
    virtual void CreateIndexBuffer(IndexBufferHandle, const void* data, uint32_t size) = 0;
    virtual void CreateShader(ShaderHandle, const std::string& source, const Attribute::BindingPack& bindings) = 0;
    virtual void CreateTexture(TextureHandle, const uint8_t* data, uint32_t width, uint32_t height, uint32_t channels) = 0;

    virtual void Destroy(VertexBufferHandle) = 0;
    virtual void Destroy(IndexBufferHandle) = 0;
    virtual void Destroy(ShaderHandle) = 0;
    virtual void Destroy(TextureHandle) = 0;

    virtual void Frame(const RendererContext*, iterator_range<const DrawUnit*>) = 0;
};

std::unique_ptr<RendererAPI> CreateRendererApi(const Config&);

struct CreateIndexBufferCommand {
    IndexBufferHandle handle = IndexBufferHandle::Invalid;
    void* data = nullptr; // TODO
    uint32_t size = 0;
};

struct CreateVertexBufferCommand {
    VertexBufferHandle handle = VertexBufferHandle::Invalid;
    void* data = nullptr;   // TODO
    uint32_t size = 0;
    VertexLayout layout;
};

struct CreateTextureCommand {
    TextureHandle handle = TextureHandle::Invalid;
    uint8_t *data = nullptr;  // TODO
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t channels = 0;
};

struct CreateShaderCommand {
    ShaderHandle handle = ShaderHandle::Invalid;
    std::string source;
    Attribute::BindingPack bindings;
};

struct DestroyIndexBufferCommand {
    IndexBufferHandle handle;
};

struct DestroyVertexBufferCommand {
    VertexBufferHandle handle;
};

struct DestroyTextureCommand {
    TextureHandle handle;
};

struct DestroyShaderCommand {
    ShaderHandle handle;
};

using FrameContextCommand =
    mpark::variant<
        CreateIndexBufferCommand,
        CreateVertexBufferCommand,
        CreateTextureCommand,
        CreateShaderCommand,
        DestroyIndexBufferCommand,
        DestroyVertexBufferCommand,
        DestroyTextureCommand,
        DestroyShaderCommand
    >;

class FrameContextCommandBuffer {
public:
    using iterator = FrameContextCommand*;
    using const_iterator = const FrameContextCommand*;

public:
    void Push(FrameContextCommand command) {
        assert(commands_count_ < static_config::kMaxFrameCommandsCount);
        commands_[commands_count_++] = std::move(command);
    }

    void Clear() {
        commands_count_ = 0;
    }

    iterator begin() { return commands_; }
    iterator end() { return commands_ + commands_count_; }

    const_iterator begin() const { return commands_; }
    const_iterator end() const { return commands_ + commands_count_; }

private:
    FrameContextCommand commands_[static_config::kMaxFrameCommandsCount];
    uint32_t commands_count_ = 0;
};

class Frame {
public:
    DrawUnit& GetDraw() {
        assert(draws_count_ < static_config::kMaxDrawCallsCount);
        return draws_[draws_count_];
    }

    void PushCommand(FrameContextCommand command) {
        command_buffer_.Push(std::move(command));
    }

    void Next() {
        draws_count_++;
        Clear(draws_[draws_count_]);
    }

    void Reset() {
        draws_count_ = 0;
        Clear(draws_[draws_count_]);
        command_buffer_.Clear();
    }

    iterator_range<const FrameContextCommand*> get_commands() const {
        return { command_buffer_.begin(), command_buffer_.end() };
    }

    iterator_range<const DrawUnit*> get_draws() const {
        return { draws_, draws_ + draws_count_ };
    }

private:
    DrawUnit draws_[static_config::kMaxDrawCallsCount];
    FrameContextCommandBuffer command_buffer_;
    uint32_t draws_count_ = 0;
};

class Renderer {
public:
    Renderer() noexcept = default;

    void Init(const Config& config) {
        assert(!api_);
        api_ = CreateRendererApi(config);
    }

    void Shutdown() {
        assert(api_);
        api_.reset();
    }

    void RenderFrame() {
        for (const auto& command : frame_.get_commands()) {
            mpark::visit([this](const auto& c){ ExecuteCommand(c); }, command);
        }

        api_->Frame(&context_, frame_.get_draws());
        frame_.Reset();
    }

    void Draw(CameraId camera_id, ShaderHandle shader_handle) {
        DrawUnit& draw = frame_.GetDraw();
        draw.camera_id = camera_id;
        draw.shader_handle = shader_handle;

        frame_.Next();
    }

    VertexBufferHandle CreateVertexBuffer(void* data, uint32_t size, VertexLayout layout) {
        VertexBufferHandle handle(vertex_buffer_handles_.Alloc());
        frame_.PushCommand(FrameContextCommand { CreateVertexBufferCommand { handle, data, size, layout } });
        return handle;
    }

    IndexBufferHandle CreateIndexBuffer(void* data, uint32_t size) {
        IndexBufferHandle handle(index_buffer_handles_.Alloc());
        frame_.PushCommand(FrameContextCommand { CreateIndexBufferCommand { handle, data, size } });
        return handle;
    }

    ShaderHandle CreateShader(const std::string &source, std::initializer_list<Attribute::Binding::Enum> bindings) {
        ShaderHandle handle(shader_handles_.Alloc());
        frame_.PushCommand(FrameContextCommand { CreateShaderCommand { handle, source, Attribute::BindingPack(bindings) } } );
        return handle;
    }

    TextureHandle CreateTexture(uint8_t *data, uint32_t width, uint32_t height, uint32_t channels) {
        TextureHandle handle(texture_handles_.Alloc());
        frame_.PushCommand( FrameContextCommand { CreateTextureCommand { handle, data, width, height, channels } });
        return handle;
    }

    void Destroy(VertexBufferHandle& handle) {
        assert(handle.IsValid() && "Renderer::Destroy failed: Invalid vertex buffer handle");

        frame_.PushCommand(FrameContextCommand { DestroyVertexBufferCommand { handle } } );
        vertex_buffer_handles_.Free(handle.ID);
        handle = VertexBufferHandle::Invalid;
    }

    void Destroy(IndexBufferHandle& handle) {
        assert(handle.IsValid() && "Renderer::Destroy failed: Invalid index buffer handle");

        frame_.PushCommand(FrameContextCommand { DestroyIndexBufferCommand { handle } } );
        index_buffer_handles_.Free(handle.ID);
        handle = IndexBufferHandle::Invalid;
    }

    void Destroy(TextureHandle& handle) {
        assert(handle.IsValid() && "Renderer::Destroy failed: Invalid texture handle");

        frame_.PushCommand(FrameContextCommand { DestroyTextureCommand { handle } } );
        texture_handles_.Free(handle.ID);
        handle = TextureHandle::Invalid;
    }

    void Destroy(ShaderHandle& handle) {
        assert(handle.IsValid() && "Renderer::Destroy failed: Invalid shader handle");

        frame_.PushCommand(FrameContextCommand { DestroyShaderCommand { handle } } );
        shader_handles_.Free(handle.ID);
        handle = ShaderHandle::Invalid;
    }

    template<class T>
    void SetUniform(ShaderHandle handle, const std::string& name, T value) {
        DrawUnit& draw = frame_.GetDraw();
        draw.command_buffer.Push( DrawUnitCommand { SetUniformCommand { handle, name, value} } );
    }

    void SetUniform(ShaderHandle handle, const std::string& name, TextureHandle value, TexSlotId slot_id) {
        DrawUnit& draw = frame_.GetDraw();
        draw.command_buffer.Push( DrawUnitCommand { SetTextureCommand { value, slot_id } } );
        SetUniform(handle, name, slot_id);
    }

    void SetBuffer(IndexBufferHandle handle) {
        DrawUnit& draw = frame_.GetDraw();
        draw.ib_handle = handle;
    }

    void SetBuffer(VertexBufferHandle handle) {
        DrawUnit& draw = frame_.GetDraw();
        draw.vb_handle = handle;
    }

    void SetTransform(const Matrix& value) {
        DrawUnit& draw = frame_.GetDraw();
        draw.transform = value;
    }

    void SetView(CameraId id, const Matrix& value) {
        context_.cameras[id].view = value;
    }

    void SetProjection(CameraId id, const Matrix& value) {
        context_.cameras[id].projection = value;
    }

    void SetClear(CameraId id, ClearFlagMask flag) {
        context_.cameras[id].clear_flag = flag;
    }

    void SetClearColor(CameraId id, const Color& color) {
        context_.cameras[id].clear_color = color;
    }
private:
    template <class TCommand>
    void ExecuteCommand(const TCommand& command);

    template <>
    void ExecuteCommand<CreateIndexBufferCommand>(const CreateIndexBufferCommand& command) {
        api_->CreateIndexBuffer(command.handle, command.data, command.size);
    }

    template <>
    void ExecuteCommand<CreateVertexBufferCommand>(const CreateVertexBufferCommand& command) {
        api_->CreateVertexBuffer(command.handle, command.data, command.size, command.layout);
    }

    template <>
    void ExecuteCommand<CreateTextureCommand>(const CreateTextureCommand& command) {
        api_->CreateTexture(command.handle, command.data, command.width, command.height, command.channels);
    }

    template <>
    void ExecuteCommand<CreateShaderCommand>(const CreateShaderCommand& command) {
        api_->CreateShader(command.handle, command.source, command.bindings);
    }

    template <>
    void ExecuteCommand<DestroyIndexBufferCommand>(const DestroyIndexBufferCommand& command) {
        api_->Destroy(command.handle);
    }

    template <>
    void ExecuteCommand<DestroyVertexBufferCommand>(const DestroyVertexBufferCommand& command) {
        api_->Destroy(command.handle);
    }

    template <>
    void ExecuteCommand<DestroyTextureCommand>(const DestroyTextureCommand& command) {
        api_->Destroy(command.handle);
    }

    template <>
    void ExecuteCommand<DestroyShaderCommand>(const DestroyShaderCommand& command) {
        api_->Destroy(command.handle);
    }

private:
    std::unique_ptr<RendererAPI> api_;

    template<size_t Size> using handle_alloc = IdAllocator<Size>;

    handle_alloc<static_config::kIndexBuffersCapacity> index_buffer_handles_;
    handle_alloc<static_config::kVertexBuffersCapacity> vertex_buffer_handles_;
    handle_alloc<static_config::kShadersCapacity> shader_handles_;
    handle_alloc<static_config::kTexturesCapacity> texture_handles_;

    RendererContext context_;
    Frame frame_;
};

static Renderer g_renderer;

}

}

}
