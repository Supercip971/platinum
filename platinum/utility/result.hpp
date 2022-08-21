#pragma once
#include <optional>
#include <platinum/log/log.hpp>
#include <variant>
namespace plat
{

struct EmptyRes
{
};
template <typename T>
struct Result
{
    std::variant<T, std::string> value;

    Result(T value)
        : value(value)
    {
    }

    Result(T &value)
        : value(value)
    {
    }

    Result(const std::string &error)
        : value(error)
    {
    }

    Result(const Result &other)
        : value(other.value)
    {
    }

    Result(Result<T> &&other)
        : value(std::move(other.value))
    {
    }

    Result &operator=(const Result &other)
    {
        value = other.value;
        return *this;
    }

    Result &operator=(Result &&other)
    {
        value = std::move(other.value);
        return *this;
    }

    operator bool() const
    {
        return std::holds_alternative<T>(value);
    }

    T &unwrap()
    {

        if (!std::holds_alternative<T>(value))
        {
            fatal$("trying to unwrap a Result that is not valid !\n - {}", std::get<std::string>(value));
        }

        T &val = std::get<T>(value);
        value = "already unwrapped";
        return val;
    }

    const T &read() const
    {
        return value.value();
    }

    const std::string &read_error() const
    {
        return std::get<std::string>(value);
    }

    static Result<T> ok(T value)
    {
        return Result<T>(value);
    }

    static Result<T> ok(T &value)
    {
        return Result<T>(value);
    }

    template <typename... S>
    static Result<T> err(const char *fmt, S... args)
    {
        return Result<T>(fmt::format(fmt, args...));
    }

    static Result<T> err(const char *error)
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

#define try$(expr) ({ auto _result = (expr); if (!_result) { return fmt::format("try failed: at {}:{}\n - {}", __FILE__, __LINE__, _result.read_error()); } _result.unwrap(); })

}; // namespace plat
