#pragma once

#include <cstdint>
#include <cstddef>

class Uuid
{
public:
    Uuid();
    Uuid(uint64_t uuid);
    Uuid(const Uuid&) = default;

    operator uint64_t() const
    {
        return m_UUID;
    }

    uint64_t Get()
    {
        return m_UUID;
    }

private:
    uint64_t m_UUID;
};