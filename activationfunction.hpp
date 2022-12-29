/*This file gives the declarations of the activation functions which we will use in the other files
First we will implement this class as being a functor
*/
#ifndef activationfunction_HPP
#define activationfunction_HPP

#include <cmath>
#include <vector>
#include <iostream>
#include "variable.hpp"

#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#   include <cxxabi.h>
#endif
#include <memory>
#include <string>
#include <cstdlib>


template <class T>
struct remove_const
{
    using type = T;
};
template <class T>
struct remove_const<const T>
{
    using type = T;
};

template <class T>
struct remove_ref
{
    using type = T;
};

template <class T>
struct remove_ref<T&>
{
    using type = T;
};

template <class T>
using remove_const_ref_t = typename remove_const<typename remove_ref<T>::type>::type;

template <class T>
std::string
type_name()
{
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void(*)(void*)> own
           (
#ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                           nullptr, nullptr),
#else
                nullptr,
#endif
                std::free
           );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}


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
using is_vector_t = typename is_vector<T>::type;

template <class T>
struct is_variable
{
    using type = std::false_type;
};
template <class T>
struct is_variable<variable<T>>
{
    using type = std::true_type;
};
template <class T>
using is_variable_t = typename is_variable<T>::type;



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


template <class T>
struct decay_variable
{
    using type = T;
};

template <class T>
struct decay_variable<variable<T>>
{
    using type = T;
};

template <class T>
using decay_variable_t =  remove_const_ref_t<typename decay_variable<remove_const_ref_t<T>>::type>;


template <class T>
struct decay_vector
{
    using type = T;
};

template <class T>
struct decay_vector<std::vector<T>>
{
    using type = T;
};

template <class T>
using decay_vector_t =  typename decay_vector<T>::type;

template <class T>
using decay_vector_variable_t = decay_variable_t<decay_vector_t<T>>;
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
            using type_numeric = decay_variable_t<decltype(std::get<0>(arg))>;
            std::vector<type_numeric> res(n_data, 0); 
            //std::get<0>(arg)
            type_numeric denom = 0;
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
            using type_numeric = decay_variable_t<decltype(std::get<0>(arg))>;
            std::vector<type_numeric> res(n_data, 0); 
            //std::get<0>(std::tuple(arg...))
            type_numeric denom = 0;
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

template <class... T>
auto softmax(std::tuple<T...> arg) //->decltype(std::get<0>(std::tuple(arg...)))
{
    return n_ary_op<softmax_class<T...>,T...>(arg);
}

/*
template <class... T>
n_ary_op<softmax_class<T>, T> softmax(T... arg...)
{
    return softmax(operand_type<const T&...>(arg...));
}
*/
#endif