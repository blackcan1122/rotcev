#pragma once
#include <malloc.h>
#include <memory>
#include <iostream>
#include <cstring>
#include <chrono>
#include <array>

namespace blck
{
    template <typename Vector>
    class RotcevIterator
    {
    public:
        using ValueType = typename Vector::ValueType;
        using PointerType = ValueType*;
        using ReferenceType = ValueType&;
    public:
        RotcevIterator(PointerType ptr)
            : m_Ptr(ptr) {}

        RotcevIterator& operator++()
        {
            m_Ptr++;
            return *this;
        }

        RotcevIterator operator++(int)
        {
            RotcevIterator iterator = *this;
            ++(*this);
            return iterator;
        }

        RotcevIterator& operator--()
        {
            m_Ptr--;
            return *this;
        }

        RotcevIterator operator--(int)
        {
            RotcevIterator iterator = *this;
            --(*this);
            return iterator;
        }

        ReferenceType operator[](int Index)
        {
            return *(m_Ptr + Index);
        }

        bool operator==(const RotcevIterator& Other) const
        {
            return m_Ptr == Other.m_Ptr;
        }

        bool operator!=(const RotcevIterator& Other) const
        {
            return m_Ptr != Other.m_Ptr;
        }

        ReferenceType operator*() const
        {
            return *m_Ptr;
        }
        
        PointerType operator->() const
        {
            return m_Ptr;
        }

    private:
        PointerType m_Ptr;
    };

    // For header-only library, we don't need export macros
    // The template will be compiled directly into the user's code

    // TODO: Container improvements and missing functionality
    // TODO: Add size() and capacity() methods for standard container interface
    // TODO: Add const versions of operator[] for read-only access
    // TODO: Implement iterators (begin(), end(), cbegin(), cend()) for range-based loops
    // TODO: Add emplace_back() for in-place construction to avoid unnecessary copies
    // TODO: Add pop_back() method for removing last element
    // TODO: Add clear() method to remove all elements while keeping allocated memory
    // TODO: Add reserve() method for pre-allocating memory to avoid reallocations
    // TODO: Add shrink_to_fit() method to reduce capacity to match size
    // TODO: Add empty() method to check if container has no elements
    // TODO: Add front() and back() methods for accessing first and last elements
    // TODO: Add copy constructor and copy assignment operator
    // TODO: Add move constructor and move assignment operator
    // TODO: Add comparison operators (==, !=, <, <=, >, >=) for container comparisons
    // TODO: Add insert() and erase() methods for arbitrary position modifications
    // TODO: Add bounds checking for operator[] in debug builds (at() method)
    // TODO: Add exception safety guarantees and proper RAII
    // TODO: Consider adding small buffer optimization for tiny objects
    // TODO: Add allocator template parameter for custom memory management
    // TODO: Optimize growth factors based on empirical performance testing
    // TODO: Add noexcept specifications where appropriate for better optimization

    template <typename T>
    class rotcev
    {
    public:
        using ValueType = T;
        using Iterator = RotcevIterator<rotcev<T>>;
    private:
        double get_growth_factor_factor()
        {
            size_t size = sizeof(T);
            return growth_factors[
                (size <= 8) ? 0 
               :(size <= 32) ? 1
               :(size <= 128)  ? 2
               :3];
        }

        void MoveRessource(void *NewStart)
        {
            if (IsTrivial)
            {
                std::memcpy(NewStart, m_Start, (sizeof(T) * m_Size));
            }
            else
            {
                for (size_t i = 0; i < m_Size; i++)
                {
                    new (((T *)NewStart) + i) T(std::move(m_Start[i]));
                    m_Start[i].~T();
                }
            }
            free(m_Start);
        }

        template <typename U>
        inline void AllocateNewSpace(U &&Value)
        {
            void *Start = 0x0;
            bool needsReallocation = (m_Capacity < sizeof(T) * (m_Size + 1));

            if (needsReallocation)
            {
                size_t NewAllocationSize = static_cast<size_t>(m_Size * get_growth_factor_factor());
                m_Capacity = sizeof(T) * std::max(NewAllocationSize, m_Size + 1);
                Start = malloc(m_Capacity);

                if (m_Size > 0)
                {
                    MoveRessource(Start);
                }
            }
            else
            {
                Start = (void *)m_Start;
            }

            T *NewOBJ = ((T *)Start) + m_Size;
            new (NewOBJ) T(std::forward<U>(Value));
            m_Start = (T *)Start;
            ++m_Size;
        }

    public:
        rotcev()
        {}
        ~rotcev()
        {
            if (m_Start)
            {
                // Call destructors for non-trivial objects
                if (!IsTrivial)
                {
                    for (size_t i = 0; i < m_Size; i++)
                    {
                        m_Start[i].~T();
                    }
                }
                // Free the memory
                free(m_Start);
            }
        }

        Iterator begin()
        {
            return Iterator(m_Start);
        }
        Iterator end()
        {
            return Iterator(m_Start + m_Size);
        }
        void push_back(const T &Value)
        {
            this->AllocateNewSpace(Value);
        }

        void push_back(T &&Value)
        {
            this->AllocateNewSpace(std::move(Value));
        }

        T &operator[](int S)
        {
            return *(m_Start + S);
        }

        inline size_t Size()
        {
            return m_Size;
        }

        rotcev(const rotcev &other) noexcept
        {
            for (size_t i = 0; i < other.m_Size; i++)
            {
                this->push_back(*(other.m_Start + i));
            }
        }

        inline void pop_back() noexcept
        {
            if (m_Size > 0)
            {
                m_Start[m_Size - 1].~T();
                --m_Size;
            }
        }

    private:
        T *m_Start = nullptr;
        size_t m_Size = 0;
        size_t m_Capacity = 0;
        static constexpr bool IsTrivial = std::is_trivially_copyable_v<T>;
        static constexpr std::array<double, 4> growth_factors = {
            10.0, // tiny objects (1-8 bytes)
            5.0,  // small objects (9-32 bytes)
            2.0,  // medium objects (33-128 bytes)
            1.5   // large objects (129+ bytes)
        };
    };

} // namespace blcke