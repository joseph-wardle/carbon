export module carbon.string;

import carbon.types;
import carbon.string_view;

namespace carbon {

export template <usize N>
struct SmallString {
    char  s[N]{};
    usize len{};
    constexpr const char* c_str() const noexcept { return s; }
    constexpr StringView view() const noexcept { return {s, len}; }
};

} // namespace carbon