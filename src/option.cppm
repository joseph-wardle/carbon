export module carbon.option;

namespace carbon {

template<class T>
[[nodiscard]] constexpr T&& move(T& t) noexcept { return static_cast<T &&>(t); }

[[noreturn]] inline void panic(const char *) { __builtin_trap(); }

export template <class T>
class Option {
    alignas(T) unsigned char buf_[sizeof(T)];
    bool has_{false};

    constexpr       T* p()       noexcept {
        return __builtin_launder(reinterpret_cast<T*>(reinterpret_cast<void*>(buf_)));
    }
    constexpr const T* p() const noexcept {
        return __builtin_launder(reinterpret_cast<const T*>(reinterpret_cast<const void*>(buf_)));
    }

    template<class U>
    constexpr void construct(U&& u) { ::new (static_cast<void *>(buf_)) T(static_cast<U &&>(u)); has_ = true; }

    constexpr void destroy() noexcept { if (has_) { p()->~T(); has_ = false; } }

public:
    [[nodiscard]] constexpr bool is_some() const noexcept { return has_; }
    [[nodiscard]] constexpr bool is_none() const noexcept { return !has_; }

    // factories
    [[nodiscard]] static constexpr Option some(const T& v)  { Option o; o.construct(v);          return o; }
    [[nodiscard]] static constexpr Option some(T&& v)       { Option o; o.construct(move(v));    return o; }
    [[nodiscard]] static constexpr Option none() noexcept   { return Option{}; }

    constexpr Option() noexcept = default;

    constexpr Option(const Option& other) {
        if (other.has_) construct(*other.p());
    }

    constexpr Option(Option&& other) noexcept {
        if (other.has_) { construct(move(*other.p())); other.destroy(); }
    }

    constexpr Option& operator=(const Option& rhs) {
        if (this == &rhs) return *this;
        if (has_ && rhs.has_)       { *p() = *rhs.p(); }
        else if (has_ && !rhs.has_) { destroy(); }
        else if (!has_ && rhs.has_) { construct(*rhs.p()); }
        return *this;
    }

    constexpr Option& operator=(Option&& rhs) noexcept {
        if (this == &rhs) return *this;
        if (has_ && rhs.has_)       { *p() = move(*rhs.p()); rhs.destroy(); }
        else if (has_ && !rhs.has_) { destroy(); }
        else if (!has_ && rhs.has_) { construct(move(*rhs.p())); rhs.destroy(); }
        return *this;
    }

    constexpr ~Option() noexcept { destroy(); }

    [[nodiscard]] constexpr       T& peek() &       { if (!has_) panic("peek on None"); return *p(); }
    [[nodiscard]] constexpr const T& peek() const & { if (!has_) panic("peek on None"); return *p(); }

    [[nodiscard]] constexpr T unwrap() && {
        if (!has_) panic("unwrap on None");
        T out = move(*p());
        destroy();
        return out;
    }

    [[nodiscard]] constexpr T unwrap_or(const T& fallback) && {
        if (has_) { T out = move(*p()); destroy(); return out; }
        return fallback; // copy
    }
    [[nodiscard]] constexpr T unwrap_or(T&& fallback) && {
        if (has_) { T out = move(*p()); destroy(); return out; }
        return move(fallback); // move
    }

    [[nodiscard]] constexpr T value() const & {
        if (!has_) panic("value on None");
        return *p();
    }

    [[nodiscard]] constexpr T value_or(T fallback) const & {
        return has_ ? *p() : carbon::move(fallback);
    }
};

} // namespace carbon