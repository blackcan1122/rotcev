#pragma once
#include <malloc.h>
#include <memory>
#include <iostream>
#include <cstring>
#include <chrono>

// TODO:
//  Fix Memory Leak
//  Add copy constructor for null refs
//  properly destruct copied objects
//  add some other helper functions
//  return [] pointer or ref but no copy
namespace blck
{

    template <typename T>
    class Rotcev
    {
public:
        Rotcev()
        {
        }

        void push_back(T &Value)
        {
            void *Start = malloc(sizeof(T) * (m_Size + 1));
            if (m_Size > 0)
            {
                if (IsTrivial)
                {
                    std::cout << "Its Trivial" << std::endl;
                    std::memcpy(Start, m_Start, (sizeof(T) * m_Size));
                }
                else
                {
                    for (size_t i = 0; i < m_Size; i++)
                    {
                        new (((T *)Start) + i) T(std::move(m_Start[i]));
                    }
                }

                free(m_Start);
            }

            T *NewInt = ((T *)Start) + m_Size;
            new (NewInt) T(std::forward<T &>(Value));
            m_Start = (T *)Start;
            ++m_Size;
        }

        void push_back(T &&Value)
        {
            void *Start = malloc(sizeof(T) * (m_Size + 1));
            if (m_Size > 0)
            {
                if (IsTrivial)
                {
                    std::cout << "Its Trivial" << std::endl;
                    std::memcpy(Start, m_Start, (sizeof(T) * m_Size));
                }
                else
                {
                    for (size_t i = 0; i < m_Size; i++)
                    {
                        new (((T *)Start) + i) T(std::move(m_Start[i]));
                    }
                }
            }

            T *NewInt = ((T *)Start) + m_Size;
            new (NewInt) T(std::move<T>(Value));
            m_Start = (T *)Start;
            ++m_Size;
        }

        T operator[](int S)
        {
            T *Return = m_Start + S;
            return *Return;
        }

        

private:

    size_t get_growth_factor_index() 
    {
        size_t size = sizeof(T);
        return (size <= 8) ? 0 : (size <= 32) ? 1 : (size <= 128) ? 2 : 3;
    }

    T *m_Start = nullptr;
    size_t m_Size = 0;
    size_t m_Capacity = 0;
    static constexpr bool IsTrivial = std::is_trivially_copyable_v<T>;
    static constexpr std::array<double, 4> growth_factors = {
        10.0,  // tiny objects (1-8 bytes)
        5.0,  // small objects (9-32 bytes) 
        2.0,  // medium objects (33-128 bytes)
        1.5   // large objects (129+ bytes)
    };
    };

} // namespace blcke