#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <ostream>

namespace Rei {

    template <typename T, std::size_t Size>
    class Vector;

    template <typename T, std::size_t Size>
    std::ostream& operator<<(std::ostream& stream, const Vector<T, Size>& vec);

    /// Vector class, representing a mathematical vector, with generic type and size.
    /// \tparam T Type of the vector's data.
    /// \tparam Size Vector's size.
    template <typename T, std::size_t Size>
    class Vector {
    public:
        constexpr Vector() noexcept = default;
        constexpr explicit Vector(const Vector<T, Size + 1>& vec) noexcept;
        constexpr Vector(const Vector<T, Size - 1>& vec, T val) noexcept;
        constexpr explicit Vector(T val) noexcept;
        template <typename... Args,
            typename = std::enable_if_t<sizeof...(Args) == Size>, // There can't be more or less values than Size
            typename = std::enable_if_t<(std::is_convertible_v<std::decay_t<Args>, T> && ...)>> // Given values must be of a convertible type
            constexpr explicit Vector(Args&&... args) noexcept : m_data{ static_cast<T>(args)... } {}
        constexpr Vector(const Vector&) noexcept = default;
        constexpr Vector(Vector&&) noexcept = default;

        constexpr std::size_t getSize() const noexcept { return Size; }
        constexpr const std::array<T, Size>& getData() const noexcept { return m_data; }
        constexpr std::array<T, Size>& getData() noexcept { return m_data; }
        constexpr const T* getDataPtr() const noexcept { return m_data.data(); }
        constexpr T* getDataPtr() noexcept { return m_data.data(); }

        /// Gets the vector's X component.
        /// \return Constant reference to the X component.
        constexpr const T& x() const noexcept;
        /// Gets the vector's X component.
        /// \return Reference to the X component.
        constexpr T& x() noexcept;
        /// Gets the vector's Y component.
        /// \return Constant reference to the Y component.
        constexpr const T& y() const noexcept;
        /// Gets the vector's Y component.
        /// \return Reference to the Y component.
        constexpr T& y() noexcept;
        /// Gets the vector's Z component.
        /// \return Constant reference to the Z component.
        constexpr const T& z() const noexcept;
        /// Gets the vector's Z component.
        /// \return Reference to the Z component.
        constexpr T& z() noexcept;
        /// Gets the vector's W component.
        /// \return Constant reference to the W component.
        constexpr const T& w() const noexcept;
        /// Gets the vector's W component.
        /// \return Reference to the W component.
        constexpr T& w() noexcept;
        /// Computes the dot product (also known as scalar or inner product) between the current vector & the given one.
        /// The dot product calculates the projection of one of the vectors onto the other; the order doesn't matter.
        /// On normalized vectors, the returned value represents the cosine of the angle (in radians) between them.
        /// \tparam DotT Type of the dot product's result. For vectors of an integral type, it is defined to a large unsigned integer type to avoid overflows.
        /// \param vec Vector to compute the dot product with.
        /// \return Dot product value.
        template <typename DotT = std::conditional_t<std::is_integral_v<T>, uint64_t, T>>
        constexpr DotT dot(const Vector& vec) const noexcept;
        /// Computes the cross product (also known as vector product) between the current vector & the given one.
        /// The cross product generates a vector which is orthogonal to the two others.
        /// \param vec Vector to compute the cross product with.
        /// \return Computed orthogonal vector.
        constexpr Vector cross(const Vector& vec) const noexcept;
        /// Computes the reflection of the current vector over a direction.
        /// The current vector is assumed to be incident (directed to the surface it is reflected on).
        /// \imageSize{vector_reflect.jpg, height: 20%; width: 20%;}
        /// \image html vector_reflect.jpg "Incident vector reflection"
        /// \param normal Direction to compute the reflection over.
        /// \return Vector's reflection.
        constexpr Vector reflect(const Vector& normal) const noexcept;
        /// Computes the squared length of the vector.
        /// The squared length is equal to the dot product of the vector with itself.
        /// This calculation does not involve a square root; it is then to be preferred over computeLength() for faster operations.
        /// \tparam SqLengthT Type of the squared length value. For vectors of an integral type, it is defined to a large unsigned integer type to avoid overflows.
        /// \return Vector's squared length.
        template <typename SqLengthT = std::conditional_t<std::is_integral_v<T>, uint64_t, T>>
        constexpr SqLengthT computeSquaredLength() const noexcept { return static_cast<SqLengthT>(dot(*this)); }
        /// Computes the length of the vector.
        /// Calculating the actual length requires a square root operation to be involved, which is expensive.
        /// As such, this function should be used only if the actual length is needed; otherwise, prefer computeSquaredLength().
        /// \tparam LengthT Type of the length value. For vectors of an integral type, it is defined to float; otherwise, it is the same as the original vector's.
        /// \return Vector's length.
        template <typename LengthT = std::conditional_t<std::is_integral_v<T>, float, T>>
        constexpr LengthT computeLength() const noexcept { return std::sqrt(static_cast<LengthT>(computeSquaredLength())); }
        /// Computes the normalized vector.
        /// Normalizing a vector makes it of length 1.
        /// \tparam NormedT Type of the normalized vector's values. For vectors of an integral type, it is defined to float;
        ///   otherwise, it is the same as the original vector's.
        /// \return Normalized vector.
        template <typename NormedT = std::conditional_t<std::is_integral_v<T>, float, T>>
        constexpr Vector<NormedT, Size> normalize() const noexcept;
        /// Computes the linear interpolation between vectors, according to a coefficient.
        /// \tparam LerpT Type of the interpolated vector's values. By default, it is the same as the current vector's.
        /// \tparam CoeffT Type of the coefficient. For vectors of an integral type, it is defined to float; otherwise, it is the same as the original vectors'.
        /// \param vec Vector to be interpolated with.
        /// \param coeff Coefficient between 0 (returns the current vector) and 1 (returns the given vector).
        /// \return Linearly interpolated vector.
        template <typename LerpT = T, typename CoeffT = std::conditional_t<std::is_integral_v<T>, float, T>>
        constexpr Vector<LerpT, Size> lerp(const Vector& vec, CoeffT coeff) const noexcept;
        /// Computes the normalized linear interpolation between vectors, according to a coefficient.
        /// \tparam NormedT Type of the normalized interpolated vector's values. For vectors of an integral type, it is defined to float;
        ///   otherwise, it is the same as the original vectors'.
        /// \tparam CoeffT Type of the coefficient. For vectors of an integral type, it is defined to float; otherwise, it is the same as the original vectors'.
        /// \param vec Vector to be interpolated with.
        /// \param coeff Coefficient between 0 (returns the normalized current vector) and 1 (returns the normalized given vector).
        /// \return Normalized linearly interpolated vector.
        template <typename NormedT = std::conditional_t<std::is_integral_v<T>, float, T>, typename CoeffT = std::conditional_t<std::is_integral_v<T>, float, T>>
        constexpr Vector<NormedT, Size> nlerp(const Vector& vec, CoeffT coeff) const noexcept { return lerp<NormedT>(vec, coeff).normalize(); }
        /// Checks for strict equality between the current vector & the given one.
        /// \param vec Vector to be compared with.
        /// \return True if vectors are strictly equal to each other, false otherwise.
        constexpr bool strictlyEquals(const Vector& vec) const noexcept;
        /// Computes the unique hash of the current vector.
        /// \param seed Value to use as a hash seed.
        /// \return Vector's hash.
        constexpr std::size_t hash(std::size_t seed = 0) const noexcept;

        /// Default copy assignment operator.
        /// \return Reference to the copied vector.
        constexpr Vector& operator=(const Vector&) noexcept = default;
        /// Default move assignment operator.
        /// \return Reference to the moved vector.
        constexpr Vector& operator=(Vector&&) noexcept = default;
        /// Vector negation operator.
        /// This unary minus negates the components of the vector, reversing its direction.
        /// \return Negated vector.
        constexpr Vector operator-() const noexcept;
        /// Element-wise vector-vector addition operator.
        /// \param vec Vector to be added.
        /// \return Result of the summed vectors.
        constexpr Vector operator+(const Vector& vec) const noexcept;
        /// Element-wise vector-value addition operator.
        /// \param val Value to be added.
        /// \return Result of the vector summed with the value.
        constexpr Vector operator+(T val) const noexcept;
        /// Element-wise vector-vector substraction operator.
        /// \param vec Vector to be substracted.
        /// \return Result of the substracted vectors.
        constexpr Vector operator-(const Vector& vec) const noexcept;
        /// Element-wise vector-value substraction operator.
        /// \param val Value to be substracted.
        /// \return Result of the vector substracted by the value.
        constexpr Vector operator-(T val) const noexcept;
        /// Element-wise vector-vector multiplication operator (performs an [Hadamard product](https://en.wikipedia.org/wiki/Hadamard_product_(matrices)).
        /// \param vec Vector to be multiplied by.
        /// \return Result of the multiplied vectors.
        constexpr Vector operator*(const Vector& vec) const noexcept;
        /// Element-wise vector-value multiplication operator.
        /// \param val Value to be multiplied by.
        /// \return Result of the vector multiplied by the value.
        constexpr Vector operator*(T val) const noexcept;
        /// Element-wise vector-vector division operator.
        /// \param vec Vector to be divided by.
        /// \return Result of the summed vectors.
        constexpr Vector operator/(const Vector& vec) const noexcept;
        /// Element-wise vector-value division operator.
        /// \param val Value to be divided by.
        /// \return Result of the vector divided by the value.
        constexpr Vector operator/(T val) const noexcept;
        /// Element-wise vector-vector addition assignment operator.
        /// \param vec Vector to be added.
        /// \return Reference to the modified original vector.
        constexpr Vector& operator+=(const Vector& vec) noexcept;
        /// Element-wise vector-value addition assignment operator.
        /// \param val Value to be added.
        /// \return Reference to the modified original vector.
        constexpr Vector& operator+=(T val) noexcept;
        /// Element-wise vector-vector substraction assignment operator.
        /// \param vec Vector to be substracted.
        /// \return Reference to the modified original vector.
        constexpr Vector& operator-=(const Vector& vec) noexcept;
        /// Element-wise vector-value substraction assignment operator.
        /// \param val Value to be substracted.
        /// \return Reference to the modified original vector.
        constexpr Vector& operator-=(T val) noexcept;
        /// Element-wise vector-vector multiplication assignment operator (performs an [Hadamard product](https://en.wikipedia.org/wiki/Hadamard_product_(matrices)).
        /// \param vec Vector to be multiplied by.
        /// \return Reference to the modified original vector.
        constexpr Vector& operator*=(const Vector& vec) noexcept;
        /// Element-wise vector-value multiplication assignment operator.
        /// \tparam ValT Type of the value to be multiplied by.
        /// \param val Value to be multiplied by.
        /// \return Reference to the modified original vector.
        template <typename ValT> constexpr Vector& operator*=(ValT val) noexcept;
        /// Element-wise vector-vector division assignment operator.
        /// \param vec Vector to be divided by.
        /// \return Reference to the modified original vector.
        constexpr Vector& operator/=(const Vector& vec) noexcept;
        /// Element-wise vector-value division assignment operator.
        /// \param val Value to be divided by.
        /// \return Reference to the modified original vector.
        constexpr Vector& operator/=(T val) noexcept;
        /// Element fetching operator given its index.
        /// \param index Element's index.
        /// \return Constant reference to the fetched element.
        constexpr const T& operator[](std::size_t index) const noexcept { return m_data[index]; }
        /// Element fetching operator given its index.
        /// \param index Element's index.
        /// \return Reference to the fetched element.
        constexpr T& operator[](std::size_t index) noexcept { return m_data[index]; }
        /// Vector equality comparison operator.
        /// Uses a near-equality check on floating types to take floating-point errors into account.
        /// \param vec Vector to be compared with.
        /// \return True if vectors are [nearly] equal, else otherwise.
        constexpr bool operator==(const Vector& vec) const noexcept;
        /// Vector inequality comparison operator.
        /// Uses a near-equality check on floating types to take floating-point errors into account.
        /// \param vec Vector to be compared with.
        /// \return True if vectors are different, else otherwise.
        constexpr bool operator!=(const Vector& vec) const noexcept { return !(*this == vec); }
        /// Vector's value type conversion operator.
        /// \tparam T2 Type to convert the vector's values to.
        /// \return Vector object of the new type.
        template <typename T2> constexpr explicit operator Vector<T2, Size>() const noexcept;
        /// Output stream operator.
        /// \param stream Stream to output into.
        /// \param vec Vector to be output.
        friend std::ostream& operator<< <>(std::ostream& stream, const Vector& vec);

    private:
        std::array<T, Size> m_data{};
    };

    /// Element-wise value-vector addition operator (of the form val + vec).
    /// \tparam T Type of the vector's data.
    /// \tparam Size Vector's size.
    /// \param val Value to be added to the vectors's element.
    /// \param vec Vector to be additioned.
    /// \return Additioned vector.
    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> operator+(T val, const Vector<T, Size>& vec) noexcept { return vec + val; }

    /// Element-wise value-vector multiplication operator (of the form val * vec).
    /// \tparam T Type of the vector's data.
    /// \tparam Size Vector's size.
    /// \param val Value to be multiplied by the vectors's element.
    /// \param vec Vector to be multiplied.
    /// \return Multiplied vector.
    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> operator*(T val, const Vector<T, Size>& vec) noexcept { return vec * val; }

    // Deduction guides

    template <typename T, typename... Args>
    Vector(T, Args... args) -> Vector<T, sizeof...(args) + 1>;

    // Aliases

    template <typename T> using Vec2 = Vector<T, 2>;
    template <typename T> using Vec3 = Vector<T, 3>;
    template <typename T> using Vec4 = Vector<T, 4>;

    using Vec2b = Vec2<uint8_t>;
    using Vec3b = Vec3<uint8_t>;
    using Vec4b = Vec4<uint8_t>;

    using Vec2i = Vec2<int>;
    using Vec3i = Vec3<int>;
    using Vec4i = Vec4<int>;

    using Vec2u = Vec2<uint32_t>;
    using Vec3u = Vec3<uint32_t>;
    using Vec4u = Vec4<uint32_t>;

    using Vec2f = Vec2<float>;
    using Vec3f = Vec3<float>;
    using Vec4f = Vec4<float>;

    using Vec2d = Vec2<double>;
    using Vec3d = Vec3<double>;
    using Vec4d = Vec4<double>;

    namespace Axis {

        constexpr Vec3f X(1.f, 0.f, 0.f);
        constexpr Vec3f Y(0.f, 1.f, 0.f);
        constexpr Vec3f Z(0.f, 0.f, 1.f);

    } // namespace Axis

    /// Vector element fetching function for a constant lvalue reference.
    /// \tparam I Index of the element.
    /// \tparam T Type of the vector's data.
    /// \tparam Size Vector's size.
    /// \param vec Vector to get the element from.
    /// \return Constant lvalue reference on the vector's element.
    template <std::size_t I, typename T, std::size_t Size>
    constexpr const T& get(const Vector<T, Size>& vec) noexcept { static_assert(I < Size); return vec[I]; }

    /// Vector element fetching function for a non-constant lvalue reference.
    /// \tparam I Index of the element.
    /// \tparam T Type of the vector's data.
    /// \tparam Size Vector's size.
    /// \param vec Vector to get the element from.
    /// \return Non-constant lvalue reference on the vector's element.
    template <std::size_t I, typename T, std::size_t Size>
    constexpr T& get(Vector<T, Size>& vec) noexcept { static_assert(I < Size); return vec[I]; }

    /// Vector element fetching function for a non-constant rvalue reference.
    /// \tparam I Index of the element.
    /// \tparam T Type of the vector's data.
    /// \tparam Size Vector's size.
    /// \param vec Vector to get the element from.
    /// \return Non-constant rvalue reference on the vector's element.
    template <std::size_t I, typename T, std::size_t Size>
    constexpr T&& get(Vector<T, Size>&& vec) noexcept { static_assert(I < Size); return std::move(vec[I]); }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size>::Vector(const Vector<T, Size + 1>& vec) noexcept {
        for (std::size_t i = 0; i < Size; ++i)
            m_data[i] = vec[i];
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size>::Vector(const Vector<T, Size - 1>& vec, T val) noexcept {
        for (std::size_t i = 0; i < Size - 1; ++i)
            m_data[i] = vec[i];
        m_data.back() = val;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size>::Vector(T val) noexcept {
        for (T& elt : m_data)
            elt = val;
    }

    template <typename T, std::size_t Size>
    constexpr const T& Vector<T, Size>::x() const noexcept {
        static_assert(Size >= 1, "Error: Getting the X component requires the vector to be of size 1 or more.");
        return m_data[0];
    }

    template <typename T, std::size_t Size>
    constexpr T& Vector<T, Size>::x() noexcept {
        return const_cast<T&>(static_cast<const Vector*>(this)->x());
    }

    template <typename T, std::size_t Size>
    constexpr const T& Vector<T, Size>::y() const noexcept {
        static_assert(Size >= 2, "Error: Getting the Y component requires the vector to be of size 2 or more.");
        return m_data[1];
    }

    template <typename T, std::size_t Size>
    constexpr T& Vector<T, Size>::y() noexcept {
        return const_cast<T&>(static_cast<const Vector*>(this)->y());
    }

    template <typename T, std::size_t Size>
    constexpr const T& Vector<T, Size>::z() const noexcept {
        static_assert(Size >= 3, "Error: Getting the Z component requires the vector to be of size 3 or more.");
        return m_data[2];
    }

    template <typename T, std::size_t Size>
    constexpr T& Vector<T, Size>::z() noexcept {
        return const_cast<T&>(static_cast<const Vector*>(this)->z());
    }

    template <typename T, std::size_t Size>
    constexpr const T& Vector<T, Size>::w() const noexcept {
        static_assert(Size >= 4, "Error: Getting the W component requires the vector to be of size 4 or more.");
        return m_data[3];
    }

    template <typename T, std::size_t Size>
    constexpr T& Vector<T, Size>::w() noexcept {
        return const_cast<T&>(static_cast<const Vector*>(this)->w());
    }

    template <typename T, std::size_t Size>
    template <typename DotT>
    constexpr DotT Vector<T, Size>::dot(const Vector& vec) const noexcept {
        DotT res{};
        for (std::size_t i = 0; i < Size; ++i)
            res += static_cast<DotT>(m_data[i]) * static_cast<DotT>(vec[i]);
        return res;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> Vector<T, Size>::cross(const Vector& vec) const noexcept {
        static_assert(Size == 3, "Error: Both vectors must be 3 dimensional to compute a cross product.");
        static_assert(std::is_signed_v<T>, "Error: The cross product can only be computed with vectors of a signed type.");

        Vector<T, Size> res;

        res.m_data[0] = m_data[1] * vec.m_data[2] - m_data[2] * vec.m_data[1];
        res.m_data[1] = -(m_data[0] * vec.m_data[2] - m_data[2] * vec.m_data[0]);
        res.m_data[2] = m_data[0] * vec.m_data[1] - m_data[1] * vec.m_data[0];

        return res;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> Vector<T, Size>::reflect(const Vector& normal) const noexcept {
        static_assert(std::is_signed_v<T>, "Error: The reflected vector can only be computed with vectors of a signed type.");

        return (*this - normal * static_cast<T>(dot(normal)) * 2);
    }

    template <typename T, std::size_t Size>
    template <typename NormedT>
    constexpr Vector<NormedT, Size> Vector<T, Size>::normalize() const noexcept {
        static_assert(std::is_floating_point_v<NormedT>, "Error: The normalized vector's type must be floating-point.");

        const NormedT sqLength = computeSquaredLength<NormedT>();
        Vector<NormedT, Size> res(*this);

        if (sqLength != static_cast<NormedT>(0))
            res /= std::sqrt(sqLength);

        return res;
    }

    template <typename T, std::size_t Size>
    template <typename LerpT, typename CoeffT>
    constexpr Vector<LerpT, Size> Vector<T, Size>::lerp(const Vector& vec, CoeffT coeff) const noexcept {
        static_assert(std::is_floating_point_v<CoeffT>, "Error: The linear interpolation's coefficient type must be floating-point.");
        assert("Error: The interpolation coefficient must be between 0 & 1." && (coeff >= 0 && coeff <= 1));

        const Vector<CoeffT, Size> convertedThis(*this);
        const Vector<CoeffT, Size> lerpVec = convertedThis + (Vector<CoeffT, Size>(vec) - convertedThis) * coeff;
        return Vector<LerpT, Size>(lerpVec);
    }

    template <typename T, std::size_t Size>
    constexpr bool Vector<T, Size>::strictlyEquals(const Vector& vec) const noexcept {
        return std::equal(m_data.cbegin(), m_data.cend(), vec.m_data.cbegin());
    }

    template <typename T, std::size_t Size>
    constexpr std::size_t Vector<T, Size>::hash(std::size_t seed) const noexcept {
        std::hash<T> hasher{};

        for (const T& elt : m_data)
            seed ^= hasher(elt) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);

        return seed;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> Vector<T, Size>::operator-() const noexcept {
        Vector res;
        for (std::size_t i = 0; i < Size; ++i)
            res.m_data[i] = -m_data[i];
        return res;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> Vector<T, Size>::operator+(const Vector& vec) const noexcept {
        Vector res = *this;
        res += vec;
        return res;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> Vector<T, Size>::operator+(T val) const noexcept {
        Vector res = *this;
        res += val;
        return res;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> Vector<T, Size>::operator-(const Vector& vec) const noexcept {
        Vector res = *this;
        res -= vec;
        return res;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> Vector<T, Size>::operator-(T val) const noexcept {
        Vector res = *this;
        res -= val;
        return res;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> Vector<T, Size>::operator*(const Vector& vec) const noexcept {
        Vector res = *this;
        res *= vec;
        return res;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> Vector<T, Size>::operator*(T val) const noexcept {
        Vector res = *this;
        res *= val;
        return res;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> Vector<T, Size>::operator/(const Vector& vec) const noexcept {
        Vector res = *this;
        res /= vec;
        return res;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size> Vector<T, Size>::operator/(T val) const noexcept {
        Vector res = *this;
        res /= val;
        return res;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size>& Vector<T, Size>::operator+=(const Vector& vec) noexcept {
        for (std::size_t i = 0; i < Size; ++i)
            m_data[i] += vec[i];
        return *this;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size>& Vector<T, Size>::operator+=(T val) noexcept {
        for (T& elt : m_data)
            elt += val;
        return *this;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size>& Vector<T, Size>::operator-=(const Vector& vec) noexcept {
        for (std::size_t i = 0; i < Size; ++i)
            m_data[i] -= vec[i];
        return *this;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size>& Vector<T, Size>::operator-=(T val) noexcept {
        for (T& elt : m_data)
            elt -= val;
        return *this;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size>& Vector<T, Size>::operator*=(const Vector& vec) noexcept {
        for (std::size_t i = 0; i < Size; ++i)
            m_data[i] *= vec[i];
        return *this;
    }

    template <typename T, std::size_t Size>
    template <typename ValT>
    constexpr Vector<T, Size>& Vector<T, Size>::operator*=(ValT val) noexcept {
        for (T& elt : m_data)
            elt *= static_cast<T>(val);
        return *this;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size>& Vector<T, Size>::operator/=(const Vector& vec) noexcept {
        if constexpr (std::is_integral_v<T>)
            assert("Error: Integer vector division by 0 is undefined." && (std::find(vec.m_data.cbegin(), vec.m_data.cend(), 0) == vec.m_data.cend()));

        for (std::size_t i = 0; i < Size; ++i)
            m_data[i] /= vec[i];
        return *this;
    }

    template <typename T, std::size_t Size>
    constexpr Vector<T, Size>& Vector<T, Size>::operator/=(T val) noexcept {
        if constexpr (std::is_integral_v<T>)
            assert("Error: Integer vector division by 0 is undefined." && (val != 0));

        for (T& elt : m_data)
            elt /= val;
        return *this;
    }

    template <typename T, std::size_t Size>
    constexpr bool Vector<T, Size>::operator==(const Vector& vec) const noexcept {
        if constexpr (std::is_floating_point_v<T>)
            return FloatUtils::areNearlyEqual(*this, vec);
        else
            return strictlyEquals(vec);
    }

    template <typename T, std::size_t Size>
    template <typename T2>
    constexpr Vector<T, Size>::operator Vector<T2, Size>() const noexcept {
        Vector<T2, Size> res;
        for (std::size_t i = 0; i < Size; ++i)
            res[i] = static_cast<T2>(m_data[i]);
        return res;
    }

    template <typename T, std::size_t Size>
    std::ostream& operator<<(std::ostream& stream, const Vector<T, Size>& vec) {
        using PrintT = std::conditional_t<std::is_same_v<T, uint8_t>, int, T>;

        stream << "[ " << static_cast<PrintT>(vec[0]);

        for (std::size_t i = 1; i < Size; ++i)
            stream << ", " << static_cast<PrintT>(vec[i]);

        stream << " ]";

        return stream;
    }

} // namespace Rei

/// Specialization of std::tuple_size for Vector.
/// \tparam T Type of the vector's data.
/// \tparam Size Vector's size.
template <typename T, std::size_t Size>
struct std::tuple_size<Rei::Vector<T, Size>> : std::integral_constant<std::size_t, Size> {};

/// Specialization of std::tuple_element for Vector.
/// \tparam I Index of the element.
/// \tparam T Type of the vector's data.
/// \tparam Size Vector's size.
template <std::size_t I, typename T, std::size_t Size>
struct std::tuple_element<I, Rei::Vector<T, Size>> { using type = T; };

/// Specialization of std::hash for Vector.
/// \tparam T Type of the vector's data.
/// \tparam Size Vector's size.
template <typename T, std::size_t Size>
struct std::hash<Rei::Vector<T, Size>> {
    /// Computes the hash of the given vector.
    /// \param vec Vector to compute the hash of.
    /// \return Vector's hash value.
    constexpr std::size_t operator()(const Rei::Vector<T, Size>& vec) const noexcept { return vec.hash(); }
};

/// Specialization of std::equal_to for Vector. This performs a strict equality check.
/// \tparam T Type of the vectors' data.
/// \tparam Size Vectors' size.
template <typename T, std::size_t Size>
struct std::equal_to<Rei::Vector<T, Size>> {
    /// Checks that the two given vectors are strictly equal to each other.
    /// \param vec1 First vector to be compared.
    /// \param vec2 Second vector to be compared.
    /// \return True if vectors are strictly equal to each other, false otherwise.
    constexpr bool operator()(const Rei::Vector<T, Size>& vec1, const Rei::Vector<T, Size>& vec2) const noexcept {
        return vec1.strictlyEquals(vec2);
    }
};

/// Specialization of std::less for Vector.
/// \tparam T Type of the vectors' data.
/// \tparam Size Vectors' size.
template <typename T, std::size_t Size>
struct std::less<Rei::Vector<T, Size>> {
    /// Checks that the first given vector is strictly less than the other.
    /// \param vec1 First vector to be compared.
    /// \param vec2 Second vector to be compared.
    /// \return True if the first vector is strictly less than the other, false otherwise.
    constexpr bool operator()(const Rei::Vector<T, Size>& vec1, const Rei::Vector<T, Size>& vec2) const noexcept {
        for (std::size_t i = 0; i < Size; ++i) {
            if (vec1[i] == vec2[i])
                continue;

            return (vec1[i] < vec2[i]);
        }

        return false;
    }
};
