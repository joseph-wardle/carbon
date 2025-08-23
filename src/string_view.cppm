export module carbon.core.string;

import carbon.types;

constexpr usize strlen(const char* str) noexcept {
    usize n = 0;
    while (str[n] != '\0') ++n;
    return n;
}

namespace carbon {

    export class StringView {
        const char* data_ = nullptr;
        const usize size_ = 0;

    public:
        constexpr StringView() noexcept = default;
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
} // namespace carbon