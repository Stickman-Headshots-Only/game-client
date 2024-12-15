#include "Bitset.h"

#include <ostream>

Bitset Bitset::operator~() const noexcept
{
    Bitset res = *this;

    for (auto bit : res.mBits)
        bit = !bit;

    return res;
}

Bitset Bitset::operator&(const Bitset& bitset) const noexcept
{
    Bitset res(std::min(mBits.size(), bitset.GetSize()));
    std::copy(mBits.cbegin(), mBits.cbegin() + static_cast<std::ptrdiff_t>(res.GetSize()), res.mBits.begin());

    res &= bitset;
    return res;
}

Bitset Bitset::operator|(const Bitset& bitset) const noexcept
{
    Bitset res(std::min(mBits.size(), bitset.GetSize()));
    std::copy(mBits.cbegin(), mBits.cbegin() + static_cast<std::ptrdiff_t>(res.GetSize()), res.mBits.begin());

    res |= bitset;
    return res;
}

Bitset Bitset::operator^(const Bitset& bitset) const noexcept
{
    Bitset res(std::min(mBits.size(), bitset.GetSize()));
    std::copy(mBits.cbegin(), mBits.cbegin() + static_cast<std::ptrdiff_t>(res.GetSize()), res.mBits.begin());

    res ^= bitset;
    return res;
}

Bitset Bitset::operator<<(std::size_t shift) const
{
    Bitset res = *this;
    res <<= shift;
    return res;
}

Bitset Bitset::operator>>(std::size_t shift) const
{
    Bitset res = *this;
    res >>= shift;
    return res;
}

Bitset& Bitset::operator&=(const Bitset& bitset) noexcept
{
    for (std::size_t bitIndex = 0; bitIndex < std::min(mBits.size(), bitset.GetSize()); ++bitIndex)
        mBits[bitIndex] = mBits[bitIndex] && bitset[bitIndex];
    return *this;
}

Bitset& Bitset::operator|=(const Bitset& bitset) noexcept
{
    for (std::size_t bitIndex = 0; bitIndex < std::min(mBits.size(), bitset.GetSize()); ++bitIndex)
        mBits[bitIndex] = mBits[bitIndex] || bitset[bitIndex];
    return *this;
}

Bitset& Bitset::operator^=(const Bitset& bitset) noexcept
{
    for (std::size_t bitIndex = 0; bitIndex < std::min(mBits.size(), bitset.GetSize()); ++bitIndex)
        mBits[bitIndex] = mBits[bitIndex] ^ bitset[bitIndex];
    return *this;
}

Bitset& Bitset::operator<<=(std::size_t shift)
{
    mBits.resize(mBits.size() + shift);
    return *this;
}

Bitset& Bitset::operator>>=(std::size_t shift)
{
    mBits.resize(mBits.size() - shift);
    return *this;
}

std::ostream& operator<<(std::ostream& stream, const Bitset& bitset)
{
    stream << "[ " << bitset[0];

    for (std::size_t i = 1; i < bitset.GetSize(); ++i)
        stream << ", " << bitset[i];

    stream << " ]";

    return stream;
}