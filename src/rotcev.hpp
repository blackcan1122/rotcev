#pragma once
#include <malloc.h>
#include <memory>
#include <iostream>
#include <cstring>
#include <chrono>
#include <array>

// Define export macros for cross-platform compatibility
#ifdef _WIN32
    #ifdef ROTCEV_EXPORTS
        #define ROTCEV_API __declspec(dllexport)
    #else
        #define ROTCEV_API __declspec(dllimport)
    #endif
#else
    // Linux/GCC: Use visibility attributes
    #define ROTCEV_API __attribute__((visibility("default")))

#endif

// TODO:

namespace blck
{

    template <typename T>
    class ROTCEV_API rotcev
    {

private:
    double get_growth_factor_factor() 
    {
        size_t size = sizeof(T);
        return growth_factors[(size <= 8) ? 0 : (size <= 32) ? 1 : (size <= 128) ? 2 : 3];
    }

    void MoveRessource(void* NewStart)
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

public:
        rotcev()
        {
        }
        ~rotcev()
        {
            if (m_Start) {
                // Call destructors for non-trivial objects
                if (!IsTrivial) {
                    for (size_t i = 0; i < m_Size; i++) {
                        m_Start[i].~T();
                    }
                }
                // Free the memory
                free(m_Start);
            }
        }

        void push_back(T &Value) 
        {
            void *Start = 0x0;
            bool needsReallocation = (m_Capacity < sizeof(T) * (m_Size + 1));

            if (needsReallocation) {
                size_t NewAllocationSize = static_cast<size_t>(m_Size * get_growth_factor_factor());
                m_Capacity = sizeof(T) * std::max(NewAllocationSize, m_Size + 1);
                Start = malloc(m_Capacity);

                if (m_Size > 0) {
                    MoveRessource(Start);
                }
            } else {
                Start = (void*)m_Start;
            }
        
            T *NewOBJ = ((T *)Start) + m_Size;
            new (NewOBJ) T(Value);
            m_Start = (T *)Start;
            ++m_Size;
        }

        void push_back(const T &Value)
        {
            void *Start = 0x0;
            bool needsReallocation = (m_Capacity < sizeof(T) * (m_Size + 1));

            if (needsReallocation) {
                size_t NewAllocationSize = static_cast<size_t>(m_Size * get_growth_factor_factor());
                m_Capacity = sizeof(T) * std::max(NewAllocationSize, m_Size + 1);
                Start = malloc(m_Capacity);

                if (m_Size > 0) {
                    MoveRessource(Start);
                }
            } else {
                Start = (void*)m_Start;
            }
        
            T *NewOBJ = ((T *)Start) + m_Size;
            new (NewOBJ) T(Value);
            m_Start = (T *)Start;
            ++m_Size; 
        }

        void push_back(T &&Value)
        {
            void *Start = 0x0;
            bool needsReallocation = (m_Capacity < sizeof(T) * (m_Size + 1));

            if (needsReallocation) {
                size_t NewAllocationSize = static_cast<size_t>(m_Size * get_growth_factor_factor());
                m_Capacity = sizeof(T) * std::max(NewAllocationSize, m_Size + 1);
                Start = malloc(m_Capacity);

                if (m_Size > 0) {
                    MoveRessource(Start);
                }
            } else {
                Start = (void*)m_Start;
            }
        
            T *NewOBJ = ((T *)Start) + m_Size;
            new (NewOBJ) T(std::move(Value));
            m_Start = (T *)Start;
            ++m_Size;
        }

        T* operator[](int S)
        {
            return (m_Start + S);
        }

        

private:

    

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