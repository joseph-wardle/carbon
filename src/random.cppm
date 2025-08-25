export module carbon.random;

import carbon.types;

namespace carbon {

constexpr u64 splitmix64(u64& x) noexcept {
    u64 z = (x += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}

constexpr u64 rotl(const u64 v, const int k) noexcept {
    const unsigned r = static_cast<unsigned>(k) & 63u;
    return (v << r) | (v >> ((64u - r) & 63u));
}

export struct rng {
    u64 s0, s1, s2, s3;

    constexpr explicit rng(const u64 seed = 0x243F6A8885A308D3ull) noexcept {
        reseed(seed);
    }

    constexpr void reseed(const u64 seed) noexcept {
        u64 x = seed;
        s0 = splitmix64(x);
        s1 = splitmix64(x);
        s2 = splitmix64(x);
        s3 = splitmix64(x);

        if ((s0 | s1 | s2 | s3) == 0) s0 = 1;
    }

    static constexpr rng from_stream(const u64 seed, const u64 stream_id) noexcept {
        rng r(seed ^ (stream_id * 0x9E3779B97F4A7C15ull));
        (void)r.next_u64();
        return r;
    }

    [[nodiscard]] constexpr u64 next_u64() noexcept {
        const u64 result = rotl(s1 * 5ull, 7) * 9ull;
        const u64 t = s1 << 17;

        s2 ^= s0;
        s3 ^= s1;
        s1 ^= s2;
        s0 ^= s3;

        s2 ^= t;
        s3 = rotl(s3, 45);
        return result;
    }

    [[nodiscard]] constexpr u32 next_u32() noexcept {
        return static_cast<u32>(next_u64() >> 32);
    }

    [[nodiscard]] constexpr f64 next_f64_01() noexcept {
        const u64 mant = next_u64() >> 11;
        return static_cast<f64>(mant) * (1.0 / 9007199254740992.0);
    }

    [[nodiscard]] constexpr f32 next_f32_01() noexcept {
        const u32 mant = static_cast<u32>(next_u64() >> 40);
        return static_cast<f32>(mant) * (1.0f / 16777216.0f);
    }
};
} // namespace carbon
