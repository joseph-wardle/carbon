export module carbon.core;

namespace carbon {

export template<class T>
[[nodiscard]] constexpr T&& move(T& t) noexcept { return static_cast<T&&>(t); }

export template<class U>
[[nodiscard]] constexpr U* launder(U* p) noexcept { return __builtin_launder(p); }

export inline void panic(const char*) { __builtin_trap(); }

} // namespace carbon
