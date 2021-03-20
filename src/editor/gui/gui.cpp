
#include "gui.h"
#include "reflection/field.h"
#include "imgui_internal.h"
#include "math/vector.h"
#include "common/color.h"
#include "common/rect.h"
#include "common/rect_int.h"

namespace sprint::editor::gui {

const ImGuiPayload* DragDropTarget(void *ptr_id, const char *type, ImVec2 size) {
    bool hovered = false;
    return DragDropTarget(ptr_id, type, size, hovered);
}

const ImGuiPayload* DragDropTarget(void *ptr_id, const char *type, ImVec2 size, bool& hovered) {
    hovered = false;

    ImGui::PushID(ptr_id);
    ImGui::Dummy(size);
    ImGui::PopID();
    ImRect rect = {ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};

    const ImGuiPayload* payload_delivery = nullptr;
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(type, ImGuiDragDropFlags_AcceptBeforeDelivery)) {
            hovered = true;
            ImGui::RenderFrame(rect.Min, rect.Max, ImGui::GetColorU32(ImGuiCol_HeaderHovered), true);
            if (payload->Delivery) {
                payload_delivery = payload;
            }
        }
        ImGui::EndDragDropTarget();
    }
    return payload_delivery;
}

void SetItemOpen(void *ptr_id, bool open) {
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    ImGuiStorage *storage = window->DC.StateStorage;
    storage->SetInt(ImGui::GetID(ptr_id), open);
}

bool IntField(const char *label, int *v, int step, int step_fast, ImGuiInputTextFlags flags) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    FieldLabel(label);

    SetNextItemWidth(ImMax(20.0f, GetContentRegionAvail().x));
    bool changed = InputInt("", v, step, step_fast, flags);

    PopID();
    EndGroup();
    return changed;
}

bool FloatField(const char *label,
                float *v,
                float v_speed,
                float v_min,
                float v_max,
                const char *format,
                ImGuiSliderFlags flags) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    FieldLabel(label);

    SetNextItemWidth(ImMax(20.0f, GetContentRegionAvail().x));
    bool changed = DragFloat("", v, v_speed, v_min, v_max, format, flags);

    PopID();
    EndGroup();
    return changed;
}

bool DoubleField(const char *label,
                 double *v,
                 double step,
                 double step_fast,
                 const char *format,
                 ImGuiInputTextFlags flags) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    FieldLabel(label);

    SetNextItemWidth(ImMax(20.0f, GetContentRegionAvail().x));
    bool changed = InputDouble("", v, step, step_fast, format, flags);

    PopID();
    EndGroup();
    return changed;
}

bool BoolField(const char *label, bool *v) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    FieldLabel(label);

    SetNextItemWidth(ImMax(20.0f, GetContentRegionAvail().x));
    bool changed = Checkbox("", v);

    PopID();
    EndGroup();
    return changed;
}

void FieldLabel(const char *label, float width) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    const float frame_size = GetFrameHeight();
    float label_width = width > 0.0f ? width : ImMax(frame_size, GetContentRegionAvail().x / 2.8f);
    SetNextItemWidth(label_width);
    LabelText("", "%s", label);
    SameLine(label_width, style.ItemSpacing.x);
}

bool Vector3Field(const char *label, float* x, float *y, float *z, const char *format, ImGuiSliderFlags flags) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    FieldLabel(label);

    bool changed = false;
    float item_w = (GetContentRegionAvail().x - 2.0f * style.ItemSpacing.y) / 3.0f;
    SetNextItemWidth(item_w);
    PushID("x");
    changed |= DragFloat("", x, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    SameLine(0.0f, style.ItemSpacing.y);
    SetNextItemWidth(item_w);
    PushID("y");
    changed |= DragFloat("", y, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    SameLine(0.0f, style.ItemSpacing.y);
    SetNextItemWidth(GetContentRegionAvail().x);
    PushID("z");
    changed |= DragFloat("", z, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    PopID();
    EndGroup();
    return changed;
}

bool TextField(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    FieldLabel(label);

    SetNextItemWidth(ImMax(20.0f, GetContentRegionAvail().x));
    bool changed = InputText("", buf, buf_size, flags, callback, user_data);

    PopID();
    EndGroup();
    return changed;
}

bool Vector2Field(const char *label, float *x, float *y, const char *format, ImGuiSliderFlags flags) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    FieldLabel(label);

    bool changed = false;
    float item_w = (GetContentRegionAvail().x - 1.0f * style.ItemSpacing.y) / 2.0f;
    SetNextItemWidth(item_w);
    PushID("x");
    changed |= DragFloat("", x, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    SameLine(0.0f, style.ItemSpacing.y);
    SetNextItemWidth(GetContentRegionAvail().x);
    PushID("y");
    changed |= DragFloat("", y, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    PopID();
    EndGroup();
    return changed;
}

bool Vector4Field(const char *label,
                  float *x,
                  float *y,
                  float *z,
                  float *w,
                  const char *format,
                  ImGuiSliderFlags flags) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    FieldLabel(label);

    bool changed = false;
    float item_w = (GetContentRegionAvail().x - 3.0f * style.ItemSpacing.y) / 4.0f;
    SetNextItemWidth(item_w);
    PushID("x");
    changed |= DragFloat("", x, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    SameLine(0.0f, style.ItemSpacing.y);
    SetNextItemWidth(item_w);
    PushID("y");
    changed |= DragFloat("", y, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    SameLine(0.0f, style.ItemSpacing.y);
    SetNextItemWidth(item_w);
    PushID("z");
    changed |= DragFloat("", z, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    SameLine(0.0f, style.ItemSpacing.y);
    SetNextItemWidth(GetContentRegionAvail().x);
    PushID("w");
    changed |= DragFloat("", w, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    PopID();
    EndGroup();
    return changed;
}

bool RectField(const char *label, float *x, float *y, float *z, float *w, const char *format, ImGuiSliderFlags flags) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    FieldLabel(label);
    bool changed = false;
    float item_w = (GetContentRegionAvail().x - style.ItemSpacing.x) / 2.0f;

    PushID("X");
    Text("X");
    SameLine(0.0f, style.ItemSpacing.x);
    SetNextItemWidth(item_w - GetItemRectSize().x - style.ItemSpacing.x);
    changed |= DragFloat("", x, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    SameLine(0.0f, style.ItemSpacing.x);

    PushID("Y");
    Text("Y");
    SameLine(0.0f, style.ItemSpacing.x);
    SetNextItemWidth(GetContentRegionAvail().x);
    changed |= DragFloat("", y, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    FieldLabel("");

    PushID("W");
    Text("W");
    SameLine(0.0f, style.ItemSpacing.x);
    SetNextItemWidth(item_w - GetItemRectSize().x - style.ItemSpacing.x);
    changed |= DragFloat("", z, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    SameLine(0.0f, style.ItemSpacing.x);

    PushID("H");
    Text("H");
    SameLine(0.0f, style.ItemSpacing.x);
    SetNextItemWidth(GetContentRegionAvail().x);
    changed |= DragFloat("", w, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    PopID();
    EndGroup();
    return changed;
}

bool Vector2IntField(const char *label, int *x, int *y, int step, int step_fast, ImGuiInputTextFlags flags) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    FieldLabel(label);

    bool changed = false;
    float item_w = (GetContentRegionAvail().x - 2.0f * style.ItemSpacing.y) / 2.0f;
    SetNextItemWidth(item_w);
    PushID("x");
    changed |= InputInt("", x, step, step_fast, flags);
    PopID();

    SameLine(0.0f, style.ItemSpacing.y);
    SetNextItemWidth(GetContentRegionAvail().x);
    PushID("y");
    changed |= InputInt("", y, step, step_fast, flags);
    PopID();

    PopID();
    EndGroup();
    return changed;
}

bool RectIntField(const char *label,
                  int *x,
                  int *y,
                  int *z,
                  int *w,
                  int step,
                  int step_fast,
                  ImGuiInputTextFlags flags) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    FieldLabel(label);

    bool changed = false;
    changed |= Vector2IntField("position", x, y, step, step_fast, flags);

    FieldLabel("");
    changed |= Vector2IntField("size", z, w, step, step_fast, flags);

    PopID();
    EndGroup();
    return changed;
}

template <class T>
bool DrawFieldImpl(meta::Reference &instance, const meta::Field &field, const char* name = nullptr);

template <>
bool DrawFieldImpl<float>(meta::Reference &instance, const meta::Field &field, const char* name) {
    auto& value = field.GetValue<float>(instance);
    return gui::FloatField(name ? name : GetDisplayName(field), &value);
}

template <>
bool DrawFieldImpl<double>(meta::Reference &instance, const meta::Field &field, const char* name) {
    auto& value = field.GetValue<double>(instance);
    return gui::DoubleField(name ? name : GetDisplayName(field), &value);
}

template <>
bool DrawFieldImpl<int>(meta::Reference &instance, const meta::Field &field, const char* name) {
    auto& value = field.GetValue<int>(instance);
    return gui::IntField(name ? name : GetDisplayName(field), &value);
}

template <>
bool DrawFieldImpl<uint32_t>(meta::Reference &instance, const meta::Field &field, const char* name) {
    auto& value = field.GetValue<int>(instance);
    return gui::IntField(name ? name : GetDisplayName(field), &value);
}

template <>
bool DrawFieldImpl<uint64_t>(meta::Reference &instance, const meta::Field &field, const char* name) {
    auto& value = field.GetValue<int>(instance);
    return gui::IntField(name ? name : GetDisplayName(field), &value);
}

template <>
bool DrawFieldImpl<bool>(meta::Reference &instance, const meta::Field &field, const char* name) {
    auto& value = field.GetValue<bool>(instance);
    return gui::BoolField(name ? name : GetDisplayName(field), &value);
}

template <>
bool DrawFieldImpl<vec2>(meta::Reference &instance, const meta::Field &field, const char* name) {
    auto& value = field.GetValue<vec2>(instance);
    return gui::Vector2Field(name ? name : GetDisplayName(field), &value.x, &value.y);
}

template <>
bool DrawFieldImpl<vec3>(meta::Reference &instance, const meta::Field &field, const char* name) {
    auto& value = field.GetValue<vec3>(instance);
    return gui::Vector3Field(name ? name : GetDisplayName(field), &value.x, &value.y, &value.z);
}

template <>
bool DrawFieldImpl<vec4>(meta::Reference &instance, const meta::Field &field, const char* name) {
    auto& value = field.GetValue<vec4>(instance);
    return gui::Vector4Field(name ? name : GetDisplayName(field), &value.x, &value.y, &value.z, &value.w);
}

template <>
bool DrawFieldImpl<Color>(meta::Reference &instance, const meta::Field &field, const char* name) {
    auto& value = field.GetValue<Color>(instance);
    return gui::Vector4Field(name ? name : GetDisplayName(field), &value.r, &value.g, &value.b, &value.a);
}

template <>
bool DrawFieldImpl<Rect>(meta::Reference &instance, const meta::Field &field, const char* name) {
    auto& value = field.GetValue<Rect>(instance);
    return gui::RectField(name ? name : GetDisplayName(field), &value.x, &value.y, &value.width, &value.height);
}

template <>
bool DrawFieldImpl<RectInt>(meta::Reference &instance, const meta::Field &field, const char* name) {
    auto& value = field.GetValue<RectInt>(instance);
    return gui::RectIntField(name ? name : GetDisplayName(field), &value.x, &value.y, &value.width, &value.height);
}

bool DrawField(meta::Reference &instance, const meta::Field &field, const char* name) {
    using method_ptr = bool (*)(meta::Reference &instance, const meta::Field &field, const char*);
    #define DEFINE_DRAWFIELD(__type) { meta::details::GetTypeId<__type>(), DrawFieldImpl<__type> }
    static const std::unordered_map<meta::TypeId, method_ptr> methods = {
        DEFINE_DRAWFIELD(bool),
        DEFINE_DRAWFIELD(int),
        DEFINE_DRAWFIELD(uint32_t),
        DEFINE_DRAWFIELD(uint64_t),
        DEFINE_DRAWFIELD(float),
        DEFINE_DRAWFIELD(double),
        DEFINE_DRAWFIELD(vec2),
        DEFINE_DRAWFIELD(vec3),
        DEFINE_DRAWFIELD(vec4),
        DEFINE_DRAWFIELD(Color),
        DEFINE_DRAWFIELD(Rect),
        DEFINE_DRAWFIELD(RectInt),
    };
    #undef DEFINE_DRAWFIELD

    bool changed = false;
    gui::PushID(field.GetName().c_str());
    meta::Type type(field.GetType());
    if (!type.IsValid()) {

    } else if (auto it = methods.find(type.ID()); it != methods.end()) {
        changed |= it->second(instance, field, name);
    } else {
        gui::Text("%s", name ? name : GetDisplayName(field));
        gui::Indent(10.0f);
        auto ref = field.GetReference(instance);
        for (const auto& inner_field : type.GetFields()) {
            changed |= DrawField(ref, inner_field);
        }
        gui::Unindent(10.0f);
    }
    gui::PopID();
    return changed;
}

static std::string ToDisplayName(const std::string& name) {
    auto begin = name.find_first_not_of('_');
    auto end = name.find_last_not_of('_');
    if (begin == std::string::npos)
        return name;

    std::string display_name(name.substr(begin, end - begin + 1));
    std::replace(display_name.begin(), display_name.end(), '_', ' ');
    auto it = 0;
    while (it != std::string::npos) {
        it = display_name.find_first_not_of(' ', it);
        if (it == std::string::npos)
            break;

        display_name[it] = std::toupper(display_name[it]);
        it = display_name.find_first_of(' ', it);
    }

    return display_name;
}

const char* GetDisplayName(const meta::Field &field) {
    static std::unordered_map<meta::TypeId, std::unordered_map<std::string, std::string>> names;
    meta::TypeId owner_id = field.GetOwnerType().ID();
    const std::string& name = field.GetName();
    if (!names[owner_id].count(name)) {
        names[owner_id][name] = ToDisplayName(name);
    }
    return names[owner_id][name].c_str();
}

}