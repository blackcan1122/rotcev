#include "rotcev.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <map>
#include <type_traits>



namespace Func
{
    #define HEADER std::setw(25) << std::right

    template<typename T>
    void FillArray(blck::rotcev<T>& Input)
    {
        for (size_t i = 0 ; i < 10000; i ++)
        {
            if constexpr (std::is_pointer<T>::value) {
                Input.push_back(new std::remove_pointer_t<T>(static_cast<std::remove_pointer_t<T>>(i)));
            }
            if constexpr (std::is_same<T, int>::value) {
                Input.push_back(static_cast<T>(i));
            }
            if constexpr (std::is_same<T, blck::rotcev<int>>::value)
            {
                blck::rotcev<int> TempArray;
                for (size_t j = 0; j < 10000; j++)
                {
                    TempArray.push_back(j);
                }
                Input.push_back(TempArray);
            }
        }
    }

    void Insertions()
    {
        std::cout << HEADER << "Start Insertion Tests" << std::endl;
        std::cout << HEADER << "=====================" << std::endl;
        blck::rotcev<int> IntContainer;
        blck::rotcev<int*> IntPtrContainer;
        blck::rotcev<blck::rotcev<int>> TwoDimInt;
        blck::rotcev<blck::rotcev<int*>> TwoDimIntPtr;

        FillArray<int>(IntContainer);
        FillArray<int*>(IntPtrContainer);
        FillArray<blck::rotcev<int>>(TwoDimInt);

        auto now = std::chrono::high_resolution_clock::now();

        volatile int sum = 0;
        for (size_t i = 0; i < TwoDimInt.Size(); i++)
        {
            for (size_t j = 0; j < TwoDimInt[i]->Size(); j++)
            {
            sum += *(*TwoDimInt[i])[j];
            }
        }

        (void)sum;
        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "looping through: " << TwoDimInt.Size()*10000 << " Elements Took: " << (end - now).count() << " nanoseconds";

        // std::cout << HEADER << "Int Container: " << std::endl;
        // for (size_t i = 0; i<IntContainer.Size(); i++)
        // {
        //     std::cout << "Adress:         " << IntContainer[i] << std::endl;
        //     std::cout << "Value:          " << *IntContainer[i] << std::endl;
        // }
        // std::cout << "" << std::endl;
        // std::cout << HEADER << "Int Pointer Container: " << std::endl;
        // for (size_t i = 0; i<IntContainer.Size(); i++)
        // {
        //     std::cout << "Adress:         " << IntPtrContainer[i] << std::endl;
        //     std::cout << "Value:          " << *IntPtrContainer[i] << std::endl;
        //     std::cout << "Value of Value: " << **IntPtrContainer[i] << std::endl;
        // }
    }
}