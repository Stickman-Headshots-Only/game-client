#pragma once

#include <algorithm>
#include <iosfwd>
#include <initializer_list>
#include <vector>

class Bitset
{
public:
    Bitset() = default;
    explicit Bitset(std::size_t bitCount, bool initialValue = false) : m_bits(bitCount, initialValue) {}
    Bitset(std::initializer_list<bool> bits) : m_bits(bits) {}

    std::size_t getSize() const noexcept { return m_bits.size(); }

    bool isEmpty() const noexcept { return (std::find(m_bits.cbegin(), m_bits.cend(), true) == m_bits.cend()); }
    std::size_t getEnabledBitCount() const noexcept { return static_cast<size_t>(std::count(m_bits.cbegin(), m_bits.cend(), true)); }
    std::size_t getDisabledBitCount() const noexcept { return m_bits.size() - getEnabledBitCount(); }
    void setBit(std::size_t index, bool value = true)
    {
        if (index < m_bits.size())
        {
            m_bits.resize(index + 1);
        }
        m_bits[index] = value;
    }
    void resize(std::size_t newSize) { m_bits.resize(newSize); }
    void reset() { std::fill(m_bits.begin(), m_bits.end(), false); }
    void clear() noexcept { m_bits.clear(); }

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
    bool operator[](std::size_t index) const noexcept { return m_bits[index]; }
    bool operator==(const Bitset& bitset) const noexcept { return std::equal(m_bits.cbegin(), m_bits.cend(), bitset.m_bits.cbegin()); }
    bool operator!=(const Bitset& bitset) const noexcept { return !(*this == bitset); }
    friend std::ostream& operator<<(std::ostream& stream, const Bitset& bitset);

private:
    std::vector<bool> m_bits{};
};

