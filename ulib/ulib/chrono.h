#pragma once

#include <chrono>

namespace ulib
{

#if (__cpp_lib_chrono >= 201907L)
    using days = std::chrono::days;
    using weeks = std::chrono::weeks;
    using years = std::chrono::years;
    using months = std::chrono::months;
#else
    using days = std::chrono::duration<int, std::ratio_multiply<std::ratio<24>, std::chrono::hours::period>>;
    using weeks = std::chrono::duration<int, std::ratio_multiply<std::ratio<7>, std::chrono::days::period>>;
    using years = std::chrono::duration<int, std::ratio_multiply<std::ratio<146097, 400>, std::chrono::days::period>>;
    using months = std::chrono::duration<int, std::ratio_divide<std::chrono::years::period, std::ratio<12>>>;

#if (__cpp_lib_chrono < 201510L)
#error "ulib/chrono.h requires C++17 and greater"
#endif
#endif

    using hours = std::chrono::hours;
    using minutes = std::chrono::minutes;
    using seconds = std::chrono::seconds;
    using milliseconds = std::chrono::milliseconds;
    using microseconds = std::chrono::microseconds;

    template <class DurationT, class ClockT>
    inline DurationT clock_time()
    {
        return std::chrono::floor<DurationT>(ClockT::now().time_since_epoch());
    }

    template <class DurationT>
    inline DurationT system_time()
    {
        return clock_time<DurationT, std::chrono::system_clock>();
    }


#if (__cpp_lib_chrono >= 201907L)

    template <class DurationT>
    inline DurationT local_time()
    {
        auto current = std::chrono::system_clock::now();
        auto local = std::chrono::current_zone()->to_local(current);
        
        return std::chrono::floor<DurationT>(local.time_since_epoch());
    }

#endif

    inline seconds unix_time() { return system_time<seconds>(); }

    namespace chrono
    {
        template <class DurationT>
        inline bool is_negative(DurationT dur)
        {
            return dur.count() < 0;
        }

        template <class DurationT>
        inline bool is_positive(DurationT dur)
        {
            return dur.count() >= 0;
        }
    } // namespace chrono

    template <class DurationT>
    inline years years_in(DurationT dur)
    {
        return std::chrono::floor<years>(dur);
    }

    template <class DurationT>
    inline months months_in(DurationT dur)
    {
        return std::chrono::floor<months>(dur);
    }

    template <class DurationT>
    inline weeks weeks_in(DurationT dur)
    {
        return std::chrono::floor<weeks>(dur);
    }

    template <class DurationT>
    inline days days_in(DurationT dur)
    {
        return std::chrono::floor<days>(dur);
    }

    template <class DurationT>
    inline hours hours_in(DurationT dur)
    {
        return std::chrono::floor<hours>(dur);
    }

    template <class DurationT>
    inline minutes minutes_in(DurationT dur)
    {
        return std::chrono::floor<minutes>(dur);
    }

    template <class DurationT>
    inline seconds seconds_in(DurationT dur)
    {
        return std::chrono::floor<seconds>(dur);
    }

    template <class DurationT>
    inline milliseconds milliseconds_in(DurationT dur)
    {
        return std::chrono::floor<milliseconds>(dur);
    }

    template <class DurationT>
    inline microseconds microseconds_in(DurationT dur)
    {
        return std::chrono::floor<microseconds>(dur);
    }

#if (__cpp_lib_chrono >= 201907L)

    template <class Duration>
    class year_month_day_hh_mm_ss
    {
    public:
        year_month_day_hh_mm_ss() {}
        year_month_day_hh_mm_ss(const std::chrono::year_month_day &ymd, const std::chrono::hh_mm_ss<Duration> &hms)
            : mYmd(ymd), mHms(hms)
        {
        }

        year_month_day_hh_mm_ss(Duration dur)
        {
            days daysInDur = days_in(dur);
            mYmd = std::chrono::year_month_day{std::chrono::sys_days{daysInDur}};

            Duration durInDays = std::chrono::duration_cast<Duration>(daysInDur);
            Duration durForHms = dur - durInDays;

            mHms = std::chrono::hh_mm_ss<Duration>{durForHms};
        }

        const std::chrono::year_month_day &ymd() { return mYmd; }
        const std::chrono::hh_mm_ss<Duration> &hms() { return mHms; }

    private:
        std::chrono::year_month_day mYmd;
        std::chrono::hh_mm_ss<Duration> mHms;
    };

#endif

} // namespace ulib