export module carbon.math;

namespace carbon {

export template <typename T>
constexpr T min(T a, T b) noexcept {
    return (a < b) ? a : b;
}

export template <typename T>
constexpr T max(T a, T b) noexcept {
    return (a > b) ? a : b;
}

}