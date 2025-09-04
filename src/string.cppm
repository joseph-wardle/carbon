export module carbon.string;

import carbon.types;

namespace carbon {

constexpr usize strlen(const char* str) noexcept {
    usize n = 0;
    while (str[n] != '\0') ++n;
    return n;
}

export class StringView {
    const char* data_ = nullptr;
    const usize size_ = 0;

public:
    constexpr StringView() noexcept = default;
    template<usize N>
    constexpr StringView(const char (&s)[N]) noexcept
        : data_{s}, size_{N ? N - 1 : 0} {}
    constexpr explicit StringView(const char* s) noexcept
        : data_{s}, size_{strlen(s)} {}
    constexpr StringView(const char* s, const usize n) noexcept
        : data_{s}, size_{n} {}

    [[nodiscard]] constexpr const char* data()  const noexcept;
    [[nodiscard]] constexpr usize       size()  const noexcept;
    [[nodiscard]] constexpr bool        empty() const noexcept;

    constexpr char operator[](usize index) const noexcept;

    [[nodiscard]] constexpr const char* begin() const noexcept;
    [[nodiscard]] constexpr const char* end()   const noexcept;
};

constexpr const char* StringView::data()  const noexcept { return data_; }
constexpr usize       StringView::size()  const noexcept { return size_; }
constexpr bool        StringView::empty() const noexcept { return size_ == 0; }

constexpr char StringView::operator[](const usize index) const noexcept { return data_[index]; }

constexpr const char* StringView::begin() const noexcept { return data_; }
constexpr const char* StringView::end()   const noexcept { return data_ + size_; }

export template <usize N>
struct SmallString {
    char  s[N]{};
    usize len{};
    [[nodiscard]] constexpr const char* c_str() const noexcept { return s; }
    [[nodiscard]] constexpr StringView view() const noexcept { return {s, len}; }
};

} // namespace carbon