#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace logger = SKSE::log;
using namespace std::literals;

namespace stl
{
using namespace SKSE::stl;

template <class T>
void write_thunk_call(std::uintptr_t a_src)
{
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    T::func          = trampoline.write_call<5>(a_src, T::thunk);
}

template <class T>
void write_thunk_call_6(std::uintptr_t a_src)
{
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    T::func          = *(uintptr_t*)trampoline.write_call<6>(a_src, T::thunk);
}

template <class F, size_t index, class T>
void write_vfunc()
{
    REL::Relocation<std::uintptr_t> vtbl{F::VTABLE[index]};
    T::func = vtbl.write_vfunc(T::size, T::thunk);
}

template <std::size_t idx, class T>
void write_vfunc(REL::VariantID id)
{
    REL::Relocation<std::uintptr_t> vtbl{id};
    T::func = vtbl.write_vfunc(idx, T::thunk);
}

template <class T>
void write_thunk_jmp(std::uintptr_t a_src)
{
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    T::func          = trampoline.write_branch<5>(a_src, T::thunk);
}

template <class F, class T>
void write_vfunc()
{
    write_vfunc<F, 0, T>();
}
} // namespace stl

#include <ankerl/unordered_dense.h>

struct StringHash
{
    using is_transparent = void; // enable heterogeneous overloads
    using is_avalanching = void; // mark class as high quality avalanching hash

    [[nodiscard]] auto operator()(std::string_view str) const noexcept -> uint64_t
    {
        return ankerl::unordered_dense::hash<std::string_view>{}(str);
    }
};

template <typename T>
using StringMap = ankerl::unordered_dense::map<std::string, T, StringHash, std::equal_to<>>;