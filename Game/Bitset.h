#pragma once

#include <algorithm>
#include <iosfwd>
#include <initializer_list>
#include <vector>

class Bitset
{
public:
    Bitset() = default;
    explicit Bitset(std::size_t bitCount, bool initialValue = false) : mBits(bitCount, initialValue) {}
    Bitset(std::initializer_list<bool> bits) : mBits(bits) {}

    std::size_t GetSize() const noexcept { return mBits.size(); }

    bool IsEmpty() const noexcept { return (std::find(mBits.cbegin(), mBits.cend(), true) == mBits.cend()); }
    std::size_t GetEnabledBitCount() const noexcept { return static_cast<size_t>(std::count(mBits.cbegin(), mBits.cend(), true)); }
    std::size_t GetDisabledBitCount() const noexcept { return mBits.size() - GetEnabledBitCount(); }
    void SetBit(std::size_t index, bool value = true)
    {
        if (index < mBits.size())
        {
            mBits.resize(index + 1);
        }
        mBits[index] = value;
    }
    void Resize(std::size_t newSize) { mBits.resize(newSize); }
    void Reset() { std::fill(mBits.begin(), mBits.end(), false); }
    void Clear() noexcept { mBits.clear(); }

    Bitset operator~() const noexcept;
    Bitset operator&(const Bitset& bitset) const noexcept;
    Bitset operator|(const Bitset& bitset) const noexcept;
    Bitset operator^(const Bitset& bitset) const noexcept;
    Bitset operator<<(std::size_t shift) const;
    Bitset operator>>(std::size_t shift) const;
    Bitset& operator&=(const Bitset& bitset) noexcept;
    Bitset& operator|=(const Bitset& bitset) noexcept;
    Bitset& operator^=(const Bitset& bitset) noexcept;
    Bitset& operator<<=(std::size_t shift);
    Bitset& operator>>=(std::size_t shift);
    bool operator[](std::size_t index) const noexcept { return mBits[index]; }
    bool operator==(const Bitset& bitset) const noexcept { return std::equal(mBits.cbegin(), mBits.cend(), bitset.mBits.cbegin()); }
    bool operator!=(const Bitset& bitset) const noexcept { return !(*this == bitset); }
    friend std::ostream& operator<<(std::ostream& stream, const Bitset& bitset);

private:
    std::vector<bool> mBits{};
};

