#pragma once

#include "gfx.h"
#include "Log.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <IdAllocator.h>
#include <utility>
#include <variant.hpp>
#include <new>
#include "iterator_range.h"

namespace sprint {

namespace gfx {

namespace details {

static Config g_config;

using Uniform = mpark::variant<int, bool, float, Vec3, Vec4, Color, Matrix>;

template<class TCommand, size_t MaxSize>
class static_command_queue {
public:
    using iterator = TCommand*;
    using const_iterator = const TCommand*;

public:
    void push(TCommand command) {
        assert(commands_count_ < MaxSize);
        commands_[commands_count_++] = std::move(command);
    }

    void clear() {
        commands_count_ = 0;
    }

    iterator begin() { return commands_; }
    iterator end() { return commands_ + commands_count_; }

    const_iterator begin() const { return commands_; }
    const_iterator end() const { return commands_ + commands_count_; }

private:
    TCommand commands_[MaxSize];
    size_t commands_count_ = 0;
};

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
using DrawUnitCommandQueue = static_command_queue<DrawUnitCommand, static_config::kMaxCommandCountPerDrawCall>;

static const DrawConfig::Options DefaultOptions = DrawConfig::Option::DEPTH_TEST;

struct DrawUnit {
    Matrix transform = Matrix::Identity;
    IndexBufferHandle ib_handle = IndexBufferHandle::Invalid;
    VertexBufferHandle vb_handle = VertexBufferHandle::Invalid;
    uint32_t vb_offset = 0;
    uint32_t vb_size = 0;
    uint32_t ib_offset = 0;
    uint32_t ib_size = 0;
    Rect scissor;
    DrawConfig::Options options = DefaultOptions;
    ShaderHandle shader_handle = ShaderHandle::Invalid;
    CameraId camera_id = UINT16_MAX;
    DrawUnitCommandQueue command_buffer;
};

static void Clear(DrawUnit& draw) {
    const static Rect zero_scissor = {0,0,0,0};

    draw.transform = Matrix::Identity;
    draw.ib_handle = IndexBufferHandle::Invalid;
    draw.vb_handle = VertexBufferHandle::Invalid;
    draw.shader_handle = ShaderHandle::Invalid;
    draw.camera_id = UINT16_MAX;
    draw.vb_offset = 0;
    draw.vb_size = 0;
    draw.ib_offset = 0;
    draw.ib_size = 0;
    draw.scissor = zero_scissor;
    draw.options = DefaultOptions;
    draw.command_buffer.clear();
}

struct Camera {
    Matrix view;
    Matrix projection;
    ClearFlagMask clear_flag = 0;
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

    virtual void CreateVertexBuffer(VertexBufferHandle, const void* data, uint32_t data_size, uint32_t size, VertexLayout layout) = 0;
    virtual void CreateIndexBuffer(IndexBufferHandle, const void* data, uint32_t data_size, uint32_t size) = 0;
    virtual void CreateShader(ShaderHandle, const std::string& source, const Attribute::BindingPack& bindings) = 0;
    virtual void CreateTexture(TextureHandle, const void* data, uint32_t data_size, uint32_t width, uint32_t height, uint32_t channels) = 0;

    virtual void UpdateVertexBuffer(VertexBufferHandle handle, uint32_t offset, const void* data, uint32_t data_size) = 0;
    virtual void UpdateIndexBuffer(IndexBufferHandle handle, uint32_t offset, const void* data, uint32_t data_size) = 0;

    virtual void Destroy(VertexBufferHandle) = 0;
    virtual void Destroy(IndexBufferHandle) = 0;
    virtual void Destroy(ShaderHandle) = 0;
    virtual void Destroy(TextureHandle) = 0;

    virtual void Frame(const RendererContext*, iterator_range<const DrawUnit*>) = 0;
};

std::unique_ptr<RendererAPI> CreateRendererApi(const Config&);

struct CreateIndexBufferCommand {
    IndexBufferHandle handle = IndexBufferHandle::Invalid;
    MemoryPtr ptr;
    uint32_t size = 0;

    void Execute(RendererAPI* api) {
        api->CreateIndexBuffer(handle, ptr.get(), ptr.size(), size);
        ptr.reset();
    }
};

struct CreateVertexBufferCommand {
    VertexBufferHandle handle = VertexBufferHandle::Invalid;
    MemoryPtr ptr;
    uint32_t size = 0;
    VertexLayout layout;

    void Execute(RendererAPI* api) {
        api->CreateVertexBuffer(handle, ptr.get(), ptr.size(), size, layout);
        ptr.reset();
    }
};

struct UpdateIndexBufferCommand {
    IndexBufferHandle handle = IndexBufferHandle::Invalid;
    MemoryPtr ptr;
    uint32_t offset;

    void Execute(RendererAPI* api) {
        api->UpdateIndexBuffer(handle, offset, ptr.get(), ptr.size());
        ptr.reset();
    }
};

struct UpdateVertexBufferCommand {
    VertexBufferHandle handle = VertexBufferHandle::Invalid;
    MemoryPtr ptr;
    uint32_t offset;

    void Execute(RendererAPI* api) {
        api->UpdateVertexBuffer(handle, offset, ptr.get(), ptr.size());
        ptr.reset();
    }
};

struct CreateTextureCommand {
    TextureHandle handle = TextureHandle::Invalid;
    MemoryPtr ptr;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t channels = 0;

    void Execute(RendererAPI* api) {
        api->CreateTexture(handle, ptr.get(), ptr.size(), width, height, channels);
    }
};

struct CreateShaderCommand {
    ShaderHandle handle = ShaderHandle::Invalid;
    std::string source;
    Attribute::BindingPack bindings;

    void Execute(RendererAPI* api) {
        api->CreateShader(handle, source, bindings);
    }
};

struct DestroyIndexBufferCommand {
    IndexBufferHandle handle;

    void Execute(RendererAPI* api) {
        api->Destroy(handle);
    }
};

struct DestroyVertexBufferCommand {
    VertexBufferHandle handle;

    void Execute(RendererAPI* api) {
        api->Destroy(handle);
    }
};

struct DestroyTextureCommand {
    TextureHandle handle;

    void Execute(RendererAPI* api) {
        api->Destroy(handle);
    }
};

struct DestroyShaderCommand {
    ShaderHandle handle;

    void Execute(RendererAPI* api) {
        api->Destroy(handle);
    }
};

using FrameContextCommand =
    mpark::variant<
        CreateIndexBufferCommand,
        CreateVertexBufferCommand,
        CreateTextureCommand,
        CreateShaderCommand,
        UpdateVertexBufferCommand,
        UpdateIndexBufferCommand,
        DestroyIndexBufferCommand,
        DestroyVertexBufferCommand,
        DestroyTextureCommand,
        DestroyShaderCommand
    >;
using FrameContextCommandQueue = static_command_queue<FrameContextCommand, static_config::kMaxFrameCommandsCount>;

class Frame {
public:
    DrawUnit& GetDraw() {
        assert(draws_count_ < static_config::kMaxDrawCallsCount);
        return draws_[draws_count_];
    }

    void PushCommand(FrameContextCommand command) {
        command_buffer_.push(std::move(command));
    }

    void Next() {
        draws_count_++;
        Clear(draws_[draws_count_]);
    }

    void Reset() {
        draws_count_ = 0;
        Clear(draws_[draws_count_]);
        command_buffer_.clear();
    }

    iterator_range<FrameContextCommand*> get_commands() {
        return { command_buffer_.begin(), command_buffer_.end() };
    }

    iterator_range<const FrameContextCommand*> get_commands() const {
        return { command_buffer_.begin(), command_buffer_.end() };
    }

    iterator_range<const DrawUnit*> get_draws() const {
        return { draws_, draws_ + draws_count_ };
    }

private:
    DrawUnit draws_[static_config::kMaxDrawCallsCount];
    FrameContextCommandQueue command_buffer_;
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
        for (auto& command : frame_.get_commands()) {
            mpark::visit([this](auto& c){ c.Execute(api_.get()); }, command);
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

    VertexBufferHandle CreateVertexBuffer(MemoryPtr ptr, uint32_t size, VertexLayout layout) {
        VertexBufferHandle handle(vertex_buffer_handles_.Alloc());
        frame_.PushCommand(CreateVertexBufferCommand { handle, std::move(ptr), size, layout } );
        return handle;
    }

    IndexBufferHandle CreateIndexBuffer(MemoryPtr ptr, uint32_t size) {
        IndexBufferHandle handle(index_buffer_handles_.Alloc());
        frame_.PushCommand(CreateIndexBufferCommand { handle, std::move(ptr), size });
        return handle;
    }

    ShaderHandle CreateShader(const std::string &source, std::initializer_list<Attribute::Binding::Enum> bindings) {
        ShaderHandle handle(shader_handles_.Alloc());
        frame_.PushCommand(CreateShaderCommand { handle, source, Attribute::BindingPack(bindings) } );
        return handle;
    }

    TextureHandle CreateTexture(MemoryPtr ptr, uint32_t width, uint32_t height, uint32_t channels) {
        TextureHandle handle(texture_handles_.Alloc());
        frame_.PushCommand(CreateTextureCommand { handle, std::move(ptr), width, height, channels } );
        return handle;
    }

    void UpdateVertexBuffer(VertexBufferHandle handle, MemoryPtr ptr, uint32_t offset) {
        frame_.PushCommand(UpdateVertexBufferCommand { handle, std::move(ptr), offset } );
    }

    void UpdateIndexBuffer(IndexBufferHandle handle, MemoryPtr ptr, uint32_t offset) {
        frame_.PushCommand(UpdateIndexBufferCommand { handle, std::move(ptr), offset } );
    }

    void Destroy(VertexBufferHandle& handle) {
        assert(handle.IsValid() && "Renderer::Destroy failed: Invalid vertex buffer handle");

        frame_.PushCommand(DestroyVertexBufferCommand { handle } );
        vertex_buffer_handles_.Free(handle.ID);
        handle = VertexBufferHandle::Invalid;
    }

    void Destroy(IndexBufferHandle& handle) {
        assert(handle.IsValid() && "Renderer::Destroy failed: Invalid index buffer handle");

        frame_.PushCommand(DestroyIndexBufferCommand { handle } );
        index_buffer_handles_.Free(handle.ID);
        handle = IndexBufferHandle::Invalid;
    }

    void Destroy(TextureHandle& handle) {
        assert(handle.IsValid() && "Renderer::Destroy failed: Invalid texture handle");

        frame_.PushCommand(DestroyTextureCommand { handle } );
        texture_handles_.Free(handle.ID);
        handle = TextureHandle::Invalid;
    }

    void Destroy(ShaderHandle& handle) {
        assert(handle.IsValid() && "Renderer::Destroy failed: Invalid shader handle");

        frame_.PushCommand(DestroyShaderCommand { handle } );
        shader_handles_.Free(handle.ID);
        handle = ShaderHandle::Invalid;
    }

    template<class T>
    void SetUniform(ShaderHandle handle, const std::string& name, T value) {
        DrawUnit& draw = frame_.GetDraw();
        draw.command_buffer.push(SetUniformCommand { handle, name, value} );
    }

    void SetUniform(ShaderHandle handle, const std::string& name, TextureHandle value, TexSlotId slot_id) {
        DrawUnit& draw = frame_.GetDraw();
        draw.command_buffer.push(SetTextureCommand { value, slot_id } );
        SetUniform(handle, name, slot_id);
    }

    void SetBuffer(IndexBufferHandle handle) {
        DrawUnit& draw = frame_.GetDraw();
        draw.ib_handle = handle;
        draw.ib_offset = 0;
        draw.ib_size = 0;
    }

    void SetBuffer(VertexBufferHandle handle) {
        DrawUnit& draw = frame_.GetDraw();
        draw.vb_handle = handle;
        draw.vb_offset = 0;
        draw.vb_size = 0;
    }

    void SetBuffer(IndexBufferHandle handle, uint32_t offset, uint32_t num) {
        DrawUnit& draw = frame_.GetDraw();
        draw.ib_handle = handle;
        draw.ib_offset = offset;
        draw.ib_size = num;
    }

    void SetBuffer(VertexBufferHandle handle, uint32_t offset, uint32_t num) {
        DrawUnit& draw = frame_.GetDraw();
        draw.vb_handle = handle;
        draw.vb_offset = offset;
        draw.vb_size = num;
    }

    void SetTransform(const Matrix& value) {
        DrawUnit& draw = frame_.GetDraw();
        draw.transform = value;
    }

    void SetScissor(Rect rect) {
        DrawUnit& draw = frame_.GetDraw();
        draw.scissor = rect;
    }

    void SetOptions(DrawConfig::Options options) {
        DrawUnit& draw = frame_.GetDraw();
        draw.options = options;
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
