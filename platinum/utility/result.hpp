#pragma once
#include <optional>
#include <platinum/log/log.hpp>
#include <platinum/utility/trait.hpp>
#include <variant>
namespace plt
{

struct EmptyRes
{
};
template <class T = plt::EmptyRes>
struct Result : public NoCopy
{
    std::variant<T, std::string> value = {};

    constexpr Result() : value()
    {
    }

    constexpr Result(const T &val)
        : value(val)
    {
    }
    constexpr Result(T &&val)
        : value(std::move(val))
    {
    }

    constexpr Result(const std::string &error)
        : value(error)
    {
    }
    constexpr Result(std::string &&error)
        : value(error)
    {
    }

    constexpr Result(Result<T> &&other)
    {
        std::swap(this->value, other.value);
    }

    constexpr Result &operator=(Result &&other)
    {
        std::swap(this->value, other.value);
        return *this;
    }

    constexpr operator bool() const
    {
        return std::holds_alternative<T>(value);
    }

    void assert() const
    {
        if (!operator bool())
        {
            fatal$("unable to assert success: {}", std::get<std::string>(value));
        }
    }

    // warning: after unwrapping, the value is now invalid,
    // you should not use a Result after unwrapping it
    T &&unwrap()
    {
        if (!std::holds_alternative<T>(value))
        {
            fatal$("trying to unwrap a Result that is not valid !\n - {}", std::get<std::string>(value));
        }

        T &&val = std::get<T>(std::move(value));
        return std::move(val);
    }

    constexpr const T &read() const
    {
        return value.value();
    }

    constexpr std::string read_error() const
    {
        return std::get<std::string>(value);
    }

    static constexpr auto ok(const T &value)
    {
        return Result<T>(value);
    }

    static constexpr auto ok(T &&value)
    {
        return Result<T>(std::move(value));
    }

    template <typename str, typename... S>
    static Result<T> err(str fmt, const S... args)
    {
        return Result<T>(fmt::format(fmt, args...));
    }

    static constexpr Result<T> err(const std::string &error)
    {
        return Result<T>(error);
    }

    template <typename V>
    static Result<T> err(Result<V> &&other)
    {
        if (other)
        {
            fatal$("converting a success to an error !");
        }
        return Result<T>(other.read_error());
    }

    static std::string err_callframe(Result<T> &res, const char *file, int line)
    {
        return fmt::format("try failed: at {}:{}\n {}", file, line, res.read_error());
    }
};

template <class T>
Result<T> success(T &&val)
{
    return Result<T>::ok(std::move(val));
}
template <class T>
Result<T> success(const T &val)
{
    return Result<T>::ok(val);
}

#define try$(expr) ({ auto _result = (expr); if (!_result) { return fmt::format("try failed: at {}:{}\n - {}", __FILE__, __LINE__, _result.read_error()); } _result.unwrap(); })

}; // namespace plt
