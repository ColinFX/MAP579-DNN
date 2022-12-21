/*This file gives the declarations of the activation functions which we will use in the other files
First we will implement this class as being a functor
*/
#ifndef activationfunction_HPP
#define activationfunction_HPP

#include <cmath>
#include <vector>
#include <iostream>
#include "variable.hpp"
template <class T>
struct is_vector
{
    using type = std::false_type;
};
template <class T>
struct is_vector<std::vector<T>>
{
    using type = std::true_type;
};
template <class T>
using is_vector_t = typename is_vector<std::decay<T>>::type;

template <class T>
    
struct arg_type
{
    using type = typename std::conditional<is_vector_t<T>::value, const T &, T>::type;
};

template <class T>
struct return_type
{
    using type = typename std::conditional<is_vector_t<T>::value, std::decay<T>,T>::type;
};
template <class T>
using arg_type_t = typename arg_type<T>::type;
template <class T>
using return_type_t = typename return_type<T>::type;

/*
template <class T>
class activate_function
{
    public:
        virtual T value(const T& arg);
        virtual T derivate(const T& arg);
};
*/
template <class T>
class ReLu_class//: activate_function<T>
{
    public:
        auto value(const T& arg) const
        {
            return arg.value()>0? arg.value():0;
        }
        auto derivative(const T& arg) const
        {
            return arg.value()>0? arg.derivative():0;
        }
};

template <class T>
unary_op<ReLu_class<T>, T> ReLu(T arg)
{
    return unary_op<ReLu_class<T>, T>(arg);
}

template <class T>
class sigmoid_class//: //activate_function<T>
{
    public:
        auto value(const T& arg) const
        {
            return 1/(1+std::exp(-arg.value()));
        }
        auto derivative(const T& arg) const
        {
            return -arg.derivative()* std::exp(-arg.value())/pow(1+std::exp(-arg.value()), 2);
        }
};

template <class T>
unary_op<sigmoid_class<T>, T> sigmoid(T arg)
{
    return unary_op<sigmoid_class<T>, T>(arg);
}

template <class... T>
class softmax_class//: //activate_function<std::tuple<T>>
{
    public:
        auto value(const std::tuple<T...>& arg) const
        {
            std::size_t n_data = (sizeof arg)/sizeof std::get<0>(arg);//std::tuple_size<decltype(arg)>::value;
            std::vector<decltype(std::get<0>(arg))> res(n_data, 0); 
            //std::get<0>(arg)
            auto denom = 0;
            for(std::size_t i=0; i<n_data; i++)
            {
                denom+=std::exp(runtime_get(arg, i).value());
            }
            for(std::size_t i=0; i<n_data; i++)
            {
                res[i] = std::exp(runtime_get(arg, i).value())/denom;
            }
            return  res;
        }
        auto derivative(const std::tuple<T...>& arg) const
        {
            std::size_t n_data = (sizeof arg)/sizeof std::get<0>(arg);//std::tuple_size<decltype(arg)>::value;
            std::vector<decltype(std::get<0>(arg))> res(n_data, 0); 
            //std::get<0>(std::tuple(arg...))
            auto denom = 0;
            for(std::size_t i=0; i<n_data; i++)
            {
                denom+=std::exp(runtime_get(arg, i).value());
            }
            for(std::size_t i=0; i<n_data; i++)
            {
                res[i] = runtime_get(arg, i).derivative() * std::exp(runtime_get(arg, i).value())/std::pow(denom, 2);
            }
            return  res;
        }
};


template <class... T>
auto softmax(T... arg) //->decltype(std::get<0>(std::tuple(arg...)))
{
    
    return n_ary_op<softmax_class<T...>,T...>(std::tuple<const T&...>(arg...));
}

/*
template <class... T>
n_ary_op<softmax_class<T>, T> softmax(T... arg...)
{
    return softmax(operand_type<const T&...>(arg...));
}
*/
#endif