export module carbon.option;

namespace carbon {

template<class T>
[[nodiscard]] constexpr T&& move(T& t) noexcept { return static_cast<T &&>(t); }

[[noreturn]] inline void panic(const char *) { __builtin_trap(); }

export template <class T>
class option {
    alignas(T) unsigned char buf_[sizeof(T)];
    bool has_{false};

    constexpr       T* p()       noexcept { return static_cast<T *>(buf_); }
    constexpr const T* p() const noexcept { return static_cast<const T *>(buf_); }

    template<class U>
    constexpr void construct(U&& u) { ::new (static_cast<void *>(buf_)) T(static_cast<U &&>(u)); has_ = true; }

    constexpr void destroy() noexcept { if (has_) { p()->~T(); has_ = false; } }

public:
    [[nodiscard]] constexpr bool is_some() const noexcept { return has_; }
    [[nodiscard]] constexpr bool is_none() const noexcept { return !has_; }

    // factories
    [[nodiscard]] static constexpr option some(const T& v)  { option o; o.construct(v);          return o; }
    [[nodiscard]] static constexpr option some(T&& v)       { option o; o.construct(move(v));    return o; }
    [[nodiscard]] static constexpr option none() noexcept   { return option{}; }

    constexpr option() noexcept = default;

    constexpr option& operator=(const option& rhs) {
        if (this == &rhs) return *this;
        if (has_ && rhs.has_)       { *p() = *rhs.p(); }
        else if (has_ && !rhs.has_) { destroy(); }
        else if (!has_ && rhs.has_) { construct(*rhs.p()); }
        return *this;
    }

    constexpr option& operator=(option&& rhs) noexcept {
        if (this == &rhs) return *this;
        if (has_ && rhs.has_)       { *p() = move(*rhs.p()); rhs.destroy(); }
        else if (has_ && !rhs.has_) { destroy(); }
        else if (!has_ && rhs.has_) { construct(move(*rhs.p())); rhs.destroy(); }
        return *this;
    }

    constexpr ~option() noexcept { destroy(); }

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
};

} // namespace carbon