#pragma once

#include <fstream>
#include <platinum/utility/result.hpp>
#include <platinum/utility/trait.hpp>
#include <vector>

namespace plt
{
class File : NoCopy
{

    File(const std::string &path, std::fstream &&stream) : _stream(std::move(stream)), _path(path) {}

public:
    static Result<File> open(const std::string &_path)
    {
        std::fstream file(_path, std::ios::binary | std::ios::in);

        if (!file.is_open())
        {
            return Result<File>::err("Failed to open file: {}", _path);
        }
        return Result<File>::ok(File(_path, std::move(file)));
    }

    static Result<std::vector<char>> read_buf(const std::string &_path)
    {
        auto file = try$(open(_path));
        auto buf = file.read_buffer();
        return Result<std::vector<char>>::ok(buf);
    }

    size_t size()
    {
        size_t size = 0;
        size_t pos = _stream.tellg();
        _stream.seekg(0, std::ios::end);
        size = _stream.tellg();
        _stream.seekg(pos);
        return size;
    }

    std::vector<char> read_buffer()
    {
        std::vector<char> result;
        size_t len = size();
        result.resize(len);
        _stream.seekg(0);
        _stream.read(result.data(), len);
        return result;
    }

    File(File &&other) = default;
    File &operator=(File &&other) = default;

    ~File()
    {
        _stream.close();
    }

private:
    std::fstream _stream;
    std::string _path;
};
} // namespace plt
