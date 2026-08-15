# C++ `<chrono>` Reference

## Clocks

- **`chrono::system_clock`** — wall-clock time (maps to real-world date/time); can go backward (NTP sync) — use for "what time is it right now."
- **`chrono::steady_clock`** — monotonic clock, never goes backward — use for measuring elapsed time/intervals, timeouts.
- **`chrono::high_resolution_clock`** — the clock with the smallest tick period available (often just an alias for `steady_clock` or `system_clock` depending on platform).
- **`chrono::now()`** — static method on any clock; returns a `time_point` representing the current time on that clock.

## Durations

- **`chrono::duration<Rep, Period>`** — the generic template representing a span of time (a count + a tick period, e.g. seconds).
- **`chrono::hours` / `minutes` / `seconds` / `milliseconds` / `microseconds` / `nanoseconds`** — predefined convenience durations, e.g. `chrono::seconds(3)` means 3 seconds.
- **`chrono::duration_cast<T>(d)`** — explicitly converts one duration type to another (e.g. `seconds` → `milliseconds`), needed when the conversion could lose precision.
- **`.count()`** — extracts the raw numeric tick count out of a duration (e.g. `chrono::seconds(5).count()` → `5`).

## Time Points

- **`chrono::time_point<Clock, Duration>`** — a point in time, defined as "this clock's epoch + this duration." What `now()` returns and what you compare/store (like our `endTime`).
- **`time_point + duration`** — arithmetic: adding a duration to a time_point gives a new time_point (used in `startTime + chrono::seconds(duration)`).
- **`time_point - time_point`** — subtracting two time_points gives a `duration` (how much time elapsed between them).
- **`time_point_cast<T>(tp)`** — converts a time_point's duration precision, similar to `duration_cast`.

## Thread/Sleep Utilities (from `<thread>`, but chrono-typed)

- **`this_thread::sleep_for(duration)`** — pauses the current thread for a relative duration (what we used: `sleep_for(chrono::seconds(3))`).
- **`this_thread::sleep_until(time_point)`** — pauses the current thread until an absolute time_point is reached.

## C++20 Additions (good to know, not usable pre-C++20)

- **`chrono::year_month_day`, `chrono::weekday`, `chrono::days`** — calendar types for civil dates (year/month/day arithmetic without manual epoch math).
- **`chrono::utc_clock`, `chrono::tai_clock`, `chrono::gps_clock`** — additional specialized clocks with leap-second-aware conversions.
- **`chrono::format` / `<chrono>` stream formatting** — direct formatting of time_points/durations without converting to `time_t`/`tm` first.