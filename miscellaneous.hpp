#ifndef miscellaneous_HPP
#define miscellaneous_HPP
#include "variable.hpp"

template <class T>
T distp_value(T objective, expression<T> x, p)
{
    return pow(std::abs(objective - x.value()), p);
}
template <class T>
T distp_value<std::vector<T>>(conststd::vector<T>& objective, const std::vector<exression<T>>&)
{
    T res = 0;
    for(std::size_t i=0; i<x.size(); i++)
    {
        res += pow(std::abs(objective - x[i].value()), p-1);
    }
    return res;
}

template <class T>
T distp_derivative(T objective, expression<T> x, p)
{
    return p * pow(std::abs(objective - x), p-1);
}
template <class T>
T distp_derivative<std::vector<T>>(conststd::vector<T>& objective, const std::vector<exression<T>>& x, p)
{
    T res = 0;
    for(std::size_t i=0; i<x.size(); i++)
    {
        res += p * pow(std::abs(objective - x[i].value()), p-1) * x[i].derivative();
    }
    return res;
}

#endif