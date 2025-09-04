export module carbon.result;

import carbon.types;
import carbon.core;

namespace carbon {

export template<class T, class E>
class Result {
    // Single storage big enough for T or E, with max alignment.
    static constexpr usize kSize  = sizeof(T) > sizeof(E) ? sizeof(T) : sizeof(E);
    static constexpr usize kAlign = alignof(T) > alignof(E) ? alignof(T) : alignof(E);

    alignas(kAlign) unsigned char storage_[kSize];

    enum class state_t : unsigned char { none, ok, err };
    state_t state_{ state_t::none };

    // Typed views into storage (valid only when that arm is active).
    constexpr       T* tptr()       noexcept { return launder(reinterpret_cast<T*>(reinterpret_cast<void*>(storage_))); }
    constexpr const T* tptr() const noexcept { return launder(reinterpret_cast<const T*>(reinterpret_cast<const void*>(storage_))); }
    constexpr       E* eptr()       noexcept { return launder(reinterpret_cast<E*>(reinterpret_cast<void*>(storage_))); }
    constexpr const E* eptr() const noexcept { return launder(reinterpret_cast<const E*>(reinterpret_cast<const void*>(storage_))); }

    template<class U> constexpr void emplace_ok(U&& v)  { ::new (static_cast<void*>(storage_)) T(static_cast<U&&>(v)); state_ = state_t::ok; }
    template<class G> constexpr void emplace_err(G&& e) { ::new (static_cast<void*>(storage_)) E(static_cast<G&&>(e)); state_ = state_t::err; }

    constexpr void destroy_() noexcept {
        if      (state_ == state_t::ok)  tptr()->~T();
        else if (state_ == state_t::err) eptr()->~E();
        state_ = state_t::none;
    }

    constexpr Result() noexcept = default;

public:
    [[nodiscard]] constexpr bool is_ok()  const noexcept { return state_ == state_t::ok;  }
    [[nodiscard]] constexpr bool is_err() const noexcept { return state_ == state_t::err; }

    [[nodiscard]] static constexpr Result ok(const T& v)  { Result r; r.emplace_ok(v);        return r; }
    [[nodiscard]] static constexpr Result ok(T&& v)       { Result r; r.emplace_ok(move(v));  return r; }
    [[nodiscard]] static constexpr Result err(const E& e) { Result r; r.emplace_err(e);       return r; }
    [[nodiscard]] static constexpr Result err(E&& e)      { Result r; r.emplace_err(move(e)); return r; }

    constexpr Result(const Result&)            = delete;
    constexpr Result& operator=(const Result&) = delete;

    constexpr Result(Result&& other) noexcept {
        if      (other.state_ == state_t::ok)  { emplace_ok(move(*other.tptr())); other.destroy_(); }
        else if (other.state_ == state_t::err) { emplace_err(move(*other.eptr())); other.destroy_(); }
    }

    constexpr Result& operator=(Result&& rhs) noexcept {
        if (this == &rhs) return *this;
        destroy_();
        if      (rhs.state_ == state_t::ok)  { emplace_ok(move(*rhs.tptr())); rhs.destroy_(); }
        else if (rhs.state_ == state_t::err) { emplace_err(move(*rhs.eptr())); rhs.destroy_(); }
        return *this;
    }

    constexpr ~Result() noexcept { destroy_(); }

    // ---- borrows (keep result alive) -------------------------------------
    [[nodiscard]] constexpr       T& peek_ok()  &       { if (!is_ok())  panic("peek_ok on Err");  return *tptr(); }
    [[nodiscard]] constexpr const T& peek_ok()  const & { if (!is_ok())  panic("peek_ok on Err");  return *tptr(); }
    [[nodiscard]] constexpr       E& peek_err() &       { if (!is_err()) panic("peek_err on Ok");  return *eptr(); }
    [[nodiscard]] constexpr const E& peek_err() const & { if (!is_err()) panic("peek_err on Ok");  return *eptr(); }

    // ---- consumers (move out and clear) ----------------------------------
    [[nodiscard]] constexpr T unwrap() && {
        if (!is_ok()) panic("unwrap on Err");
        T out = move(*tptr());
        destroy_();
        return out;
    }

    [[nodiscard]] constexpr T unwrap_or(const T& fallback) && {
        if (is_ok()) { T out = move(*tptr()); destroy_(); return out; }
        return fallback; // copy
    }
    [[nodiscard]] constexpr T unwrap_or(T&& fallback) && {
        if (is_ok()) { T out = move(*tptr()); destroy_(); return out; }
        return move(fallback); // move
    }

    // Symmetric err unwrapping (optional but handy)
    [[nodiscard]] constexpr E unwrap_err() && {
        if (!is_err()) panic("unwrap_err on Ok");
        E out = move(*eptr());
        destroy_();
        return out;
    }
};

// ---- specialization for T = void ----
export template<class E>
class Result<void, E> {
    alignas(alignof(E)) unsigned char storage_[sizeof(E)];
    enum class state_t : unsigned char { ok, err };
    state_t state_{ state_t::ok };

    constexpr       E* eptr()       noexcept { return launder(reinterpret_cast<E*>(static_cast<void*>(storage_))); }
    constexpr const E* eptr() const noexcept { return launder(reinterpret_cast<const E*>(static_cast<const void*>(storage_))); }

    template<class G> constexpr void emplace_err(G&& e) { ::new (static_cast<void*>(storage_)) E(static_cast<G&&>(e)); state_ = state_t::err; }

public:
    constexpr Result() noexcept = default;

    [[nodiscard]] static constexpr Result ok()                  { return Result{}; }
    [[nodiscard]] static constexpr Result err(const E& e)       { Result r; r.emplace_err(e);       return r; }
    [[nodiscard]] static constexpr Result err(E&& e)            { Result r; r.emplace_err(move(e)); return r; }

    [[nodiscard]] constexpr bool is_ok()  const noexcept { return state_ == state_t::ok; }
    [[nodiscard]] constexpr bool is_err() const noexcept { return state_ == state_t::err; }

    constexpr Result(const Result& other) {
        if (other.state_ == state_t::err) emplace_err(*other.eptr());
        else state_ = state_t::ok;
    }
    constexpr Result(Result&& other) noexcept {
        if (other.state_ == state_t::err) { emplace_err(move(*other.eptr())); other.state_ = state_t::ok; }
        else state_ = state_t::ok;
    }
    constexpr Result& operator=(Result&& rhs) noexcept {
        if (this == &rhs) return *this;
        if (state_ == state_t::err) eptr()->~E();
        if (rhs.state_ == state_t::err) { emplace_err(move(*rhs.eptr())); rhs.state_ = state_t::ok; }
        else state_ = state_t::ok;
        return *this;
    }
    constexpr ~Result() noexcept { if (state_ == state_t::err) eptr()->~E(); }

    [[nodiscard]] constexpr       E& peek_err() &       { if (!is_err()) panic("peek_err on Ok");  return *eptr(); }
    [[nodiscard]] constexpr const E& peek_err() const & { if (!is_err()) panic("peek_err on Ok");  return *eptr(); }
    [[nodiscard]] constexpr E unwrap_err() && {
        if (!is_err()) panic("unwrap_err on Ok");
        E out = move(*eptr());
        state_ = state_t::ok;
        return out;
    }
};

} // namespace carbon
