#pragma once

#include "gfx.h"
#include "Log.h"
#include "iterator_range.h"
#include "semaphore.h"

#include <utility>
#include <variant>
#include <thread>

namespace sprint {

namespace gfx {

namespace details {

class Frame;

class RendererAPI {
public:
    RendererAPI() = default;
    RendererAPI(const RendererAPI &) = delete;
    RendererAPI(RendererAPI &&) = delete;

    RendererAPI &operator=(const RendererAPI &) = delete;
    RendererAPI &operator=(RendererAPI &&) = delete;

    virtual ~RendererAPI() = default;

    virtual void CreateVertexBuffer(vertexbuf_handle,
                                    const void *data,
                                    uint32_t data_size,
                                    uint32_t size,
                                    VertexLayout layout) = 0;
    virtual void CreateIndexBuffer(indexbuf_handle, const void *data, uint32_t data_size, uint32_t size) = 0;
    virtual void CreateFrameBuffer(framebuf_handle, texture_handle *, uint32_t num, bool destroy_tex = false) = 0;
    virtual void CreateShader(shader_handle, const std::string &source, const Attribute::BindingPack &bindings) = 0;
    virtual void CreateUniform(uniform_handle, char* name) = 0;
    virtual void CreateTexture(texture_handle,
                               const void *data,
                               uint32_t data_size,
                               uint32_t width,
                               uint32_t height,
                               TextureFormat::Enum,
                               TextureWrap wrap,
                               TextureFilter filter,
                               TextureFlags::Type flags) = 0;

    virtual void UpdateVertexBuffer(vertexbuf_handle handle,
                                    uint32_t offset,
                                    const void *data,
                                    uint32_t data_size) = 0;
    virtual void UpdateIndexBuffer(indexbuf_handle handle, uint32_t offset, const void *data, uint32_t data_size) = 0;

    virtual void Destroy(vertexbuf_handle) = 0;
    virtual void Destroy(indexbuf_handle) = 0;
    virtual void Destroy(framebuf_handle) = 0;
    virtual void Destroy(shader_handle) = 0;
    virtual void Destroy(uniform_handle) = 0;
    virtual void Destroy(texture_handle) = 0;

    virtual void RenderFrame(const Frame &) = 0;
};

std::unique_ptr<RendererAPI> CreateRendererApi(const Config &);

static Config g_config;

template <class ...Types>
struct types_list {
    using variant = std::variant<Types...>;
    template<class T>
    constexpr static bool is_one_of_v = (std::is_same_v<T, Types> || ...);
};

using uniform_types = types_list<int, bool, float, Vec3, Vec4, Color, Matrix>;
using uniform_variant = uniform_types::variant;
struct Uniform : public uniform_variant {
    enum Enum {
        INT  = 0,
        BOOL,
        FLOAT,
        VEC3,
        VEC4,
        COLOR,
        MATRIX
    };

    static_assert(std::is_same_v<int, std::variant_alternative_t<INT, uniform_variant>>);
    static_assert(std::is_same_v<bool, std::variant_alternative_t<BOOL, uniform_variant>>);
    static_assert(std::is_same_v<float, std::variant_alternative_t<FLOAT, uniform_variant>>);
    static_assert(std::is_same_v<Vec3, std::variant_alternative_t<VEC3, uniform_variant>>);
    static_assert(std::is_same_v<Vec4, std::variant_alternative_t<VEC4, uniform_variant>>);
    static_assert(std::is_same_v<Color, std::variant_alternative_t<COLOR, uniform_variant>>);
    static_assert(std::is_same_v<Matrix, std::variant_alternative_t<MATRIX, uniform_variant>>);

    template <class T>
    explicit operator T() const {
        static_assert(uniform_types::is_one_of_v<T>, "Invalid conversion");
        return std::get<T>(*this); 
    }
    
    template <class T>
    Uniform(T value) : uniform_variant(value), type((Enum)uniform_variant::index()) {}

    Uniform() = default;

    Enum type;
};

template<class TCommand, size_t Capacity>
class static_command_queue {
public:
    using iterator = TCommand *;
    using const_iterator = const TCommand *;

public:
    void push(TCommand command) {
        assert(size_ < Capacity);
        commands_[size_++] = std::move(command);
    }

    template <class T>
    T& emplace() {
        assert(size_ < Capacity);
        commands_[size_++] = T{};
        return std::get<T>(commands_[size_-1]);
    }

    void clear() { size_ = 0; }

    iterator begin() { return commands_; }
    iterator end() { return commands_ + size_; }

    [[nodiscard]] const_iterator begin() const { return commands_; }
    [[nodiscard]] const_iterator end() const { return commands_ + size_; }

private:
    TCommand commands_[Capacity];
    size_t size_ = 0;
};

//struct SetUniformCommand {
//    uniform_handle uniform_handle;
//    Uniform value;
//};

struct UniformPair {
    uniform_handle handle;
    Uniform value;
};

//struct SetTextureCommand {
//    texture_handle texture_handle;
//    TexSlotId slot;
//};

//using DrawUnitCommand = std::variant<SetUniformCommand, SetTextureCommand>;
//using DrawUnitCommandQueue = static_command_queue<DrawUnitCommand, static_config::kMaxCommandCountPerDrawCall>;

static const DrawConfig::Options DefaultOptions = DrawConfig::DEPTH_TEST;

struct DrawUnit {
    CameraId camera_id = 0;
    Matrix transform = Matrix::Identity;
    indexbuf_handle ib_handle = {};
    vertexbuf_handle vb_handle = {};
    shader_handle shader_handle = {};
    uint32_t vb_offset = 0;
    uint32_t vb_size = 0;
    uint32_t ib_offset = 0;
    uint32_t ib_size = 0;
    Rect scissor{};
    DrawConfig::Options options = DefaultOptions;
    UniformPair uniforms[static_config::kMaxUniformsPerDrawCall];
    size_t uniforms_size;
    texture_handle textures[static_config::kTextureSlotsCapacity];
    uint32_t texture_slots[static_config::kTextureSlotsCapacity];
    size_t texture_slots_size;
//    DrawUnitCommandQueue command_buffer{};
};

static void Clear(DrawUnit &draw) {
    draw.transform = Matrix::Identity;
    draw.ib_handle = {};
    draw.vb_handle = {};
    draw.shader_handle = {};
    draw.camera_id = 0;
    draw.vb_offset = 0;
    draw.vb_size = 0;
    draw.ib_offset = 0;
    draw.ib_size = 0;
    draw.scissor = {};
    draw.options = DefaultOptions;
    draw.uniforms_size = 0;
    draw.texture_slots_size = 0;
//    draw.command_buffer.clear();
}

struct Camera {
    Matrix view;
    Matrix projection;
    Rect viewport;
    ClearFlag::Type clear_flag = 0;
    Color clear_color;
    framebuf_handle frame_buffer = {};
};

struct CreateIndexBufferCommand {
    indexbuf_handle handle;
    MemoryPtr ptr;
    uint32_t size = 0;

    void Execute(RendererAPI *api) {
        api->CreateIndexBuffer(handle, ptr.get(), ptr.size(), size);
        ptr.reset();
    }
};

struct CreateVertexBufferCommand {
    vertexbuf_handle handle;
    MemoryPtr ptr;
    uint32_t size = 0;
    VertexLayout layout;

    void Execute(RendererAPI *api) {
        api->CreateVertexBuffer(handle, ptr.get(), ptr.size(), size, layout);
        ptr.reset();
    }
};

struct CreateFrameBufferCommand {
    framebuf_handle handle;
    texture_handle handles[static_config::kFrameBufferMaxAttachments];
    uint32_t size = 0;
    bool destroy_tex = false;

    void Execute(RendererAPI *api) {
        api->CreateFrameBuffer(handle, handles, size, destroy_tex);
    }
};

struct CreateUniformCommand {
    uniform_handle handle;
    char name[64];

    void Execute(RendererAPI *api) {
        api->CreateUniform(handle, name);
    }
};

struct UpdateIndexBufferCommand {
    indexbuf_handle handle;
    MemoryPtr ptr;
    uint32_t offset;

    void Execute(RendererAPI *api) {
        api->UpdateIndexBuffer(handle, offset, ptr.get(), ptr.size());
        ptr.reset();
    }
};

struct UpdateVertexBufferCommand {
    vertexbuf_handle handle;
    MemoryPtr ptr;
    uint32_t offset;

    void Execute(RendererAPI *api) {
        api->UpdateVertexBuffer(handle, offset, ptr.get(), ptr.size());
        ptr.reset();
    }
};

struct CreateTextureCommand {
    texture_handle handle;
    MemoryPtr ptr;
    uint32_t width = 0;
    uint32_t height = 0;
    TextureFormat::Enum format = TextureFormat::Enum::RGBA8;
    TextureWrap wrap;
    TextureFilter filter;
    TextureFlags::Type flags;

    void Execute(RendererAPI *api) {
        api->CreateTexture(handle, ptr.get(), ptr.size(), width, height, format, wrap, filter, flags);
    }
};

struct CreateShaderCommand {
    shader_handle handle;
    std::string source;
    Attribute::BindingPack bindings;

    void Execute(RendererAPI *api) {
        api->CreateShader(handle, source, bindings);
    }
};

struct DestroyIndexBufferCommand {
    indexbuf_handle handle;

    void Execute(RendererAPI *api) {
        api->Destroy(handle);
    }
};

struct DestroyFrameBufferCommand {
    framebuf_handle handle;

    void Execute(RendererAPI *api) {
        api->Destroy(handle);
    }
};

struct DestroyVertexBufferCommand {
    vertexbuf_handle handle;

    void Execute(RendererAPI *api) {
        api->Destroy(handle);
    }
};

struct DestroyTextureCommand {
    texture_handle handle;

    void Execute(RendererAPI *api) {
        api->Destroy(handle);
    }
};

struct DestroyUniformCommand {
    uniform_handle handle;

    void Execute(RendererAPI *api) {
        api->Destroy(handle);
    }
};

struct DestroyShaderCommand {
    shader_handle handle;

    void Execute(RendererAPI *api) {
        api->Destroy(handle);
    }
};

using FrameContextCommand =
std::variant<
    CreateIndexBufferCommand,
    CreateVertexBufferCommand,
    CreateFrameBufferCommand,
    CreateTextureCommand,
    CreateUniformCommand,
    CreateShaderCommand,
    UpdateVertexBufferCommand,
    UpdateIndexBufferCommand,
    DestroyIndexBufferCommand,
    DestroyFrameBufferCommand,
    DestroyVertexBufferCommand,
    DestroyTextureCommand,
    DestroyUniformCommand,
    DestroyShaderCommand
>;
using FrameContextCommandQueue = static_command_queue<FrameContextCommand, static_config::kMaxFrameCommandsCount>;

class Frame {
public:
    inline DrawUnit& GetDraw() {
        assert(draws_count_ < static_config::kMaxDrawCallsCount);
        return draws_[draws_count_];
    }

    inline void Next() {
        Clear(draws_[++draws_count_]);
    }

    void Reset() {
        draws_count_ = 0;
        Clear(draws_[draws_count_]);
        command_buffer_.clear();
    }
    void PushCommand(FrameContextCommand command) {
        command_buffer_.push(std::move(command));
    }

    template <class T>
    T& EmplaceCommand() {
        return command_buffer_.emplace<T>();
    }

    void SetCameras(const Camera *src) {
        std::memcpy(cameras_, src, sizeof(Camera) * static_config::kCamerasCapacity);
    }

    [[nodiscard]] const Camera &get_camera(CameraId id) const {
        return cameras_[id];
    }

    iterator_range<FrameContextCommand *> get_commands() {
        return {command_buffer_.begin(), command_buffer_.end()};
    }

    [[nodiscard]] iterator_range<const FrameContextCommand *> get_commands() const {
        return {command_buffer_.begin(), command_buffer_.end()};
    }

    [[nodiscard]] iterator_range<const DrawUnit *> get_draws() const {
        return {draws_, draws_ + draws_count_};
    }

    [[nodiscard]] iterator_range<const Camera *> get_cameras() const {
        return {cameras_, cameras_ + static_config::kCamerasCapacity};
    }

    [[nodiscard]] Vec2Int get_resolution() const { return resolution_; }
    void set_resolution(Vec2Int value) { resolution_ = value; }

private:
    Camera cameras_[static_config::kCamerasCapacity];
    DrawUnit draws_[static_config::kMaxDrawCallsCount];
    uint32_t draws_count_ = 0;
    FrameContextCommandQueue command_buffer_;
    Vec2Int resolution_{};
};

class Renderer {
public:
    Renderer() noexcept = default;

    void Init(const Config &config) {
        assert(!api_);
        api_ = CreateRendererApi(config);
    }

    void Shutdown() {
        assert(api_);
        api_.reset();
    }

    void RenderFrame() {
        for (auto &command : frame_.get_commands()) {
            std::visit([this](auto &c) { c.Execute(api_.get()); }, command);
        }

        frame_.set_resolution(g_config.resolution);
        frame_.SetCameras(cameras_);

        api_->RenderFrame(frame_);

        frame_.Reset();
    }

    void Draw(CameraId camera_id, shader_handle shader_handle) {
        DrawUnit &draw = frame_.GetDraw();
        draw.camera_id = camera_id;
        draw.shader_handle = shader_handle;
        frame_.Next();
    }

    vertexbuf_handle CreateVertexBuffer(MemoryPtr ptr, uint32_t size, VertexLayout layout) {
        vertexbuf_handle handle(vertex_buffer_handles_.get());
        auto& command = frame_.EmplaceCommand<CreateVertexBufferCommand>();
        command.handle = handle;
        command.ptr = std::move(ptr);
        command.size = size;
        command.layout = layout;
        return handle;
    }

    indexbuf_handle CreateIndexBuffer(MemoryPtr ptr, uint32_t size) {
        indexbuf_handle handle(index_buffer_handles_.get());
        auto& command = frame_.EmplaceCommand<CreateIndexBufferCommand>();
        command.handle = handle;
        command.ptr = std::move(ptr);
        command.size = size;
        return handle;
    }

    framebuf_handle CreateFrameBuffer(uint32_t width, uint32_t height, TextureFormat::Enum format, TextureWrap wrap) {
        auto color_tex_handle = CreateTexture({}, width, height, format, wrap);
        auto depth_tex_handle = CreateTexture({}, width, height, TextureFormat::D24S8, {}, {}, TextureFlags::RenderTarget);
        return CreateFrameBuffer({color_tex_handle, depth_tex_handle}, true);
    }

    framebuf_handle CreateFrameBuffer(std::initializer_list<texture_handle> textures, bool destroy_tex) {
        assert(textures.size() < static_config::kFrameBufferMaxAttachments);
        framebuf_handle handle(frame_buffer_handles_.get());
        auto& command = frame_.EmplaceCommand<CreateFrameBufferCommand>();
        command.handle = handle;
        command.destroy_tex = destroy_tex;
        for (auto tex : textures) command.handles[command.size++] = tex;

        return handle;
    }

    uniform_handle CreateUniform(const char* str) {
        uniform_handle handle(uniform_handles_.get());
        auto& command = frame_.EmplaceCommand<CreateUniformCommand>();
        command.handle = handle;
        std::strcpy(command.name, str);

        return handle;
    }

    shader_handle CreateShader(const std::string &source, std::initializer_list<Attribute::Binding::Enum> bindings) {
        shader_handle handle(shader_handles_.get());
        auto& command = frame_.EmplaceCommand<CreateShaderCommand>();
        command.handle = handle;
        command.source = source;
        command.bindings = Attribute::BindingPack(bindings);
        return handle;
    }

    texture_handle CreateTexture(MemoryPtr ptr, uint32_t width, uint32_t height,
                                 TextureFormat::Enum format, TextureWrap wrap,
                                 TextureFilter filter = {TextureFilter::Linear, TextureFilter::Linear,
                                                        TextureFilter::Linear},
                                 TextureFlags::Type flags = TextureFlags::None) {
        texture_handle handle(texture_handles_.get());
        auto& command = frame_.EmplaceCommand<CreateTextureCommand>();
        command.handle = handle;
        command.ptr = std::move(ptr);
        command.width = width;
        command.height = height;
        command.format = format;
        command.wrap = wrap;
        command.filter = filter;
        command.flags = flags;
        return handle;
    }

    void UpdateVertexBuffer(vertexbuf_handle handle, MemoryPtr ptr, uint32_t offset) {
        auto& command = frame_.EmplaceCommand<UpdateVertexBufferCommand>();
        command.handle = handle;
        command.ptr = std::move(ptr);
        command.offset = offset;
    }

    void UpdateIndexBuffer(indexbuf_handle handle, MemoryPtr ptr, uint32_t offset) {
        auto& command = frame_.EmplaceCommand<UpdateIndexBufferCommand>();
        command.handle = handle;
        command.ptr = std::move(ptr);
        command.offset = offset;
    }

    void Destroy(vertexbuf_handle &handle) {
        assert(handle && "Renderer::Destroy failed: Invalid vertex buffer handle");

        auto& command = frame_.EmplaceCommand<DestroyVertexBufferCommand>();
        command.handle = handle;
        vertex_buffer_handles_.erase(handle);
        handle = {};
    }

    void Destroy(indexbuf_handle &handle) {
        assert(handle && "Renderer::Destroy failed: Invalid index buffer handle");

        auto& command = frame_.EmplaceCommand<DestroyIndexBufferCommand>();
        command.handle = handle;
        index_buffer_handles_.erase(handle);
        handle = {};
    }

    void Destroy(framebuf_handle &handle) {
        assert(handle && "Renderer::Destroy failed: Invalid frame buffer handle");

        auto& command = frame_.EmplaceCommand<DestroyFrameBufferCommand>();
        command.handle = handle;
        frame_buffer_handles_.erase(handle);
        handle = {};
    }

    void Destroy(texture_handle &handle) {
        assert(handle && "Renderer::Destroy failed: Invalid texture handle");

        auto& command = frame_.EmplaceCommand<DestroyTextureCommand>();
        command.handle = handle;
        texture_handles_.erase(handle);
        handle = {};
    }

    void Destroy(shader_handle &handle) {
        assert(handle && "Renderer::Destroy failed: Invalid shader handle");

        auto& command = frame_.EmplaceCommand<DestroyShaderCommand>();
        command.handle = handle;
        shader_handles_.erase(handle);
        handle = {};
    }

    void Destroy(uniform_handle& handle) {
        assert(handle && "Renderer::Destroy failed: Invalid uniform handle");

        auto& command = frame_.EmplaceCommand<DestroyUniformCommand>();
        command.handle = handle;
        uniform_handles_.erase(handle);
        handle = {};
    }

    bool IsValid(vertexbuf_handle handle) {
        return vertex_buffer_handles_.valid(handle);
    }

    bool IsValid(indexbuf_handle handle) {
        return index_buffer_handles_.valid(handle);
    }

    bool IsValid(framebuf_handle handle) {
        return frame_buffer_handles_.valid(handle);
    }

    bool IsValid(shader_handle handle) {
        return shader_handles_.valid(handle);
    }

    bool IsValid(uniform_handle handle) {
        return uniform_handles_.valid(handle);
    }

    bool IsValid(texture_handle handle) {
        return texture_handles_.valid(handle);
    }

    template<class T>
    void SetUniform(uniform_handle handle, T value) {
        DrawUnit &draw = frame_.GetDraw();
        auto& uniform = draw.uniforms[draw.uniforms_size++];
        uniform.handle = handle;
        uniform.value = value;
    }

    void SetUniform(uniform_handle handle, texture_handle tex_handle, TexSlotId slot) {
        DrawUnit &draw = frame_.GetDraw();
        draw.textures[slot] = tex_handle;
        draw.texture_slots[draw.texture_slots_size++] = slot;
        SetUniform(handle, (int)slot);
    }

    void SetBuffer(indexbuf_handle handle) {
        DrawUnit &draw = frame_.GetDraw();
        draw.ib_handle = handle;
        draw.ib_offset = 0;
        draw.ib_size = 0;
    }

    void SetBuffer(vertexbuf_handle handle) {
        DrawUnit &draw = frame_.GetDraw();
        draw.vb_handle = handle;
        draw.vb_offset = 0;
        draw.vb_size = 0;
    }

    void SetBuffer(indexbuf_handle handle, uint32_t offset, uint32_t num) {
        DrawUnit &draw = frame_.GetDraw();
        draw.ib_handle = handle;
        draw.ib_offset = offset;
        draw.ib_size = num;
    }

    void SetBuffer(vertexbuf_handle handle, uint32_t offset, uint32_t num) {
        DrawUnit &draw = frame_.GetDraw();
        draw.vb_handle = handle;
        draw.vb_offset = offset;
        draw.vb_size = num;
    }

    void SetTransform(const Matrix &value) {
        DrawUnit &draw = frame_.GetDraw();
        draw.transform = value;
    }

    void SetScissor(Rect rect) {
        DrawUnit &draw = frame_.GetDraw();
        draw.scissor = rect;
    }

    void SetOptions(DrawConfig::Options options) {
        DrawUnit &draw = frame_.GetDraw();
        draw.options = options;
    }

    void SetView(CameraId id, const Matrix &value) {
        cameras_[id].view = value;
    }

    void SetViewRect(CameraId id, const Rect &rect) {
        cameras_[id].viewport = rect;
    }

    void SetProjection(CameraId id, const Matrix &value) {
        cameras_[id].projection = value;
    }

    void SetClear(CameraId id, ClearFlag::Type flag) {
        cameras_[id].clear_flag = flag;
    }

    void SetClearColor(CameraId id, const Color &color) {
        cameras_[id].clear_color = color;
    }

    void SetViewBuffer(CameraId id, framebuf_handle handle) {
        cameras_[id].frame_buffer = handle;
    }

private:
    std::unique_ptr<RendererAPI> api_;

//    template<class>
//    struct handle_config_traits;
//
//    template<>
//    struct handle_config_traits<indexbuf_handle> {
//        static constexpr const size_t capacity = static_config::kIndexBuffersCapacity;
//    };
//    template<>
//    struct handle_config_traits<vertexbuf_handle> {
//        static constexpr const size_t capacity = static_config::kIndexBuffersCapacity;
//    };
//    template<>
//    struct handle_config_traits<framebuf_handle> {
//        static constexpr const size_t capacity = static_config::kIndexBuffersCapacity;
//    };
//    template<>
//    struct handle_config_traits<shader_handle> {
//        static constexpr const size_t capacity = static_config::kIndexBuffersCapacity;
//    };
//    template<>
//    struct handle_config_traits<texture_handle> {
//        static constexpr const size_t capacity = static_config::kIndexBuffersCapacity;
//    };
//    template<>
//    struct handle_config_traits<uniform_handle> {
//        static constexpr const size_t capacity = static_config::kIndexBuffersCapacity;
//    };
//
//    template<class ...Types>
//    struct handles_pool {
//    private:
//        std::tuple<handle_pool<Types, handle_config_traits<Types>::capacity>...> pools_;
//    public:
//        template<class T>
//        T get() { return std::get<handle_pool<T, handle_config_traits<T>::capacity>>(pools_).get(); }
//
//        template<class T>
//        void erase(T val) { std::get<handle_pool<T, handle_config_traits<T>::capacity>>(pools_).erase(val); }
//    };
//    handles_pool<indexbuf_handle, vertexbuf_handle, framebuf_handle, shader_handle, texture_handle, uniform_handle> handles_;

    handle_pool<indexbuf_handle, static_config::kIndexBuffersCapacity> index_buffer_handles_;
    handle_pool<vertexbuf_handle, static_config::kVertexBuffersCapacity> vertex_buffer_handles_;
    handle_pool<framebuf_handle, static_config::kFrameBuffersCapacity> frame_buffer_handles_;
    handle_pool<shader_handle, static_config::kShadersCapacity> shader_handles_;
    handle_pool<texture_handle, static_config::kTexturesCapacity> texture_handles_;
    handle_pool<uniform_handle, static_config::kUniformsCapacity> uniform_handles_;

    Camera cameras_[static_config::kCamerasCapacity];
    Frame frame_;
};

static Renderer g_renderer;


};

};

};
