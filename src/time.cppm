module;
#include <time.h>
export module carbon.time;

import carbon.types;

// ---- Units & layout ----
constexpr u64 NS_PER_SEC   = 1'000'000'000ull;
constexpr u64 SEC_PER_DAY  = 86'400ull;
constexpr u64 SEC_PER_MIN   = 60ull;
constexpr u64 MIN_PER_HOUR  = 60ull;
constexpr u64 SEC_PER_HOUR  = SEC_PER_MIN * MIN_PER_HOUR;
constexpr u32 RFC3339_LEN  = 30;              // "YYYY-MM-DDThh:mm:ss.nnnnnnnnnZ"
constexpr u32 RFC3339_SIZE = RFC3339_LEN + 1; // +NUL

struct YearMonthDay { i32 year; u32 month; u32 day; };

[[nodiscard]] static constexpr YearMonthDay ymd_from_unix_days(u64 days) noexcept {
    const i64 z   = static_cast<i64>(days) + 719468;                 // days since 0000-03-01
    const i64 era = (z >= 0 ? z : z - 146096) / 146097;              // 400-year eras
    const u32 doe = static_cast<u32>(z - era * 146097);              // day-of-era [0,146096]
    const u32 yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365; // year-of-era [0,399]

    i32 year = static_cast<i32>(yoe) + static_cast<i32>(era * 400);

    const u32 doy = doe - (365*yoe + yoe/4 - yoe/100);
    const u32 mp  = (5*doy + 2) / 153;

    const u32 day   = doy - (153*mp + 2)/5 + 1;
    const u32 month = (mp < 10) ? (mp + 3) : (mp - 9);
    if (month <= 2) ++year; // Jan/Feb are months 13/14 of previous year

    return { year, month, day };
}

template <unsigned N>
[[nodiscard]] static inline char* put(char* p, u32 v) noexcept {
    for (unsigned i = 0; i < N; ++i) {
        const u32 digit = v % 10u;
        p[N - 1 - i] = static_cast<char>('0' + digit);
        v /= 10u;
    }
    return p + N;
}

[[nodiscard]] static inline char* put_char(char* p, const char c) noexcept { *p++ = c; return p; }

static inline u32 clamp_year4(const i32 y) noexcept {
    return (y < 0) ? 0u : (y > 9999 ? 9999u : static_cast<u32>(y));
}

namespace carbon {

export struct Instant { u64 ns; };
export struct Steady  { u64 ns; };

export inline Instant now_utc_ns() noexcept {
    timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    const u64 s  = static_cast<u64>(ts.tv_sec);
    const u64 ns = static_cast<u64>(ts.tv_nsec);
    return Instant{ s * NS_PER_SEC + ns };
}

export inline Steady now_mono_ns() noexcept {
    timespec ts{};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    const u64 s  = static_cast<u64>(ts.tv_sec);
    const u64 ns = static_cast<u64>(ts.tv_nsec);
    return Steady{ s * NS_PER_SEC + ns };
}

export [[nodiscard]] char* format_rfc3339_utc(const Instant t) noexcept {
    char out[RFC3339_SIZE];
    const u64 sec = t.ns / NS_PER_SEC;
    const u64 nss = t.ns % NS_PER_SEC;

    const u64 days = sec / SEC_PER_DAY;
    const u64 sod = sec % SEC_PER_DAY;

    const auto [year, month, day] = ymd_from_unix_days(days);
    const u32 hour   = static_cast<u32>(sod / SEC_PER_HOUR);
    const u32 minute = static_cast<u32>((sod % SEC_PER_HOUR) / SEC_PER_MIN);
    const u32 second = static_cast<u32>(sod % SEC_PER_MIN);

    char* q = out;
    q = put<4>(q, clamp_year4(year));
    q = put_char(q, '-');
    q = put<2>(q, month);
    q = put_char(q, '-');
    q = put<2>(q, day);
    q = put_char(q, 'T');
    q = put<2>(q, hour);
    q = put_char(q, ':');
    q = put<2>(q, minute);
    q = put_char(q, ':');
    q = put<2>(q, second);
    q = put_char(q, '.');
    q = put<9>(q, static_cast<u32>(nss));
    q = put_char(q, 'Z');

    *q = '\0';
    return q; // points at the NUL

}
} // namespace carbon