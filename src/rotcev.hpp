#pragma once
#include <malloc.h>
#include <memory>
#include <iostream>
#include <cstring>
#include <chrono>


template <typename T>
class Rotcev {
public:
    Rotcev()
    {
    }

    void push_back(T& Value)
    {
        void* Start = malloc(sizeof(T)*(m_Size + 1));
        if (m_Size > 0)
        {
            if (IsTrivial)
            {
                std::cout << "Its Trivial" << std::endl;
                std::memcpy(Start, m_Start, (sizeof(T)*m_Size));
            }
            else
            {
                for (size_t i = 0; i < m_Size; i++)
                {
                    new(((T*)Start) + i) T(std::move(m_Start[i]));                
                }
            }
        }

        T* NewInt = ((T*)Start) + m_Size;
        new(NewInt) T(std::forward<T&>(Value));
        m_Start = (T*)Start;
        ++m_Size;        
    }

    void push_back(T&& Value)
    {
        void* Start = malloc(sizeof(T)*(m_Size + 1));
        if (m_Size > 0)
        {
            if (IsTrivial)
            {
                std::cout << "Its Trivial" << std::endl;
                std::memcpy(Start, m_Start, (sizeof(T)*m_Size));
            }
            else
            {
                for (size_t i = 0; i < m_Size; i++)
                {
                    new(((T*)Start) + i) T(std::move(m_Start[i]));                
                }
            }
        }

        T* NewInt = ((T*)Start) + m_Size;
        new(NewInt) T(std::move<T>(Value));
        m_Start = (T*)Start;
        ++m_Size;        
    }

    T operator [] (int S)
    {
        T* Return = m_Start + S;
        return *Return;
    }

public:

    T *m_Start = nullptr;
    size_t m_Size = 0;
    size_t m_Capacity = 0;
    static constexpr bool IsTrivial = std::is_trivially_copyable_v<T>;
};