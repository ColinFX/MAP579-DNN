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
T set_zero(const T& arg)
{return 0.;}

template <>
column_vector set_zero<column_vector>(const column_vector& arg)
{return column_vector(arg.size(), 0.);}

template <>
matrix set_zero<matrix>(const matrix& arg)
{return matrix(arg.get_row(), arg.get_column(), 0.);}


template <class T>
class ReLu_class
{
    public:
        auto value(const T& arg) const
        {
            return arg.value()>0? arg.value():0.;
        }
        auto derivative(const T& arg) const
        {
            return arg.value()>0.? arg.derivative():set_zero<column_vector>(arg.derivative());
        }
};

template <class T>
unary_op<ReLu_class<T>, T> ReLu_function(T arg)
{
    return unary_op<ReLu_class<T>, T>(arg);
}
/*
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
            column_vector 
            return -arg.derivative()* std::exp(-arg.value())/pow(1+std::exp(-arg.value()), 2);
        }
};

template <class T>
unary_op<sigmoid_class<T>, T> sigmoid_function(T arg)
{
    return unary_op<sigmoid_class<T>, T>(arg);
}
*/
template <class T>
variable<double> sigmoid_function(T arg)
{
    return 1./(1.+exp(-arg));
}



/*
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
auto softmax_function(T... arg) //->decltype(std::get<0>(std::tuple(arg...)))
{
    return n_ary_op<softmax_class<T...>,T...>(std::tuple<const T&...>(arg...));
}

template <class... T>
auto softmax_function(std::tuple<T...> arg) //->decltype(std::get<0>(std::tuple(arg...)))
{
    return n_ary_op<softmax_class<T...>,T...>(arg);
}




template <class... T>
n_ary_op<softmax_class<T>, T> softmax(T... arg...)
{
    return softmax(operand_type<const T&...>(arg...));
}
*/


template<class T>
class softmax_class
{
public:
        auto value(const T& arg) const
        {
            std::size_t n_data = arg.size();
            column_vector res(n_data, 0.); 
            double denom = 0.;
            for(std::size_t i=0; i<n_data; i++)
            {
                denom+=std::exp(arg.value()(i));
            }
            for(std::size_t i=0; i<n_data; i++)
            {
                res(i) = std::exp(arg.value()(i))/denom;
            }

            return  res;
        }
        auto derivative(const T& arg) const
        {
            std::size_t n_rows = arg.derivative().get_row();
            std::size_t n_columns = arg.derivative().get_column();
            matrix res(n_rows, n_columns, 0.); 
            double denom = 0.;
            column_vector denom_derivative(n_columns, 0.);
            for(std::size_t i=0; i<n_rows; i++)
            {
                denom+=std::exp(arg.value()(i));
                for(std::size_t j=0; j<n_columns; j++)
                {
                    denom_derivative(j) += arg.derivative()(i, j)*std::exp(arg.value()(i));
                }
            }
            for(std::size_t i=0; i<n_rows; i++)
            {
                for(std::size_t j=0; j<n_columns; j++)
                res(i, j) = (arg.derivative()(i, j)*std::exp(arg.value()(i))/denom - std::exp(arg.value()(i)) * denom_derivative(j)/std::pow(denom, 2));
            }
            return  res;
        }
};
template <class T>
auto softmax_function(const T& arg) 
{
    return unary_op<softmax_class<T>, T>(arg);
}

/////////////////////////////////////////////////////////////
// dictionary of activate functions
/////////////////////////////////////////////////////////////
enum class activation_function_class{softmax, ReLu, sigmoid};

std::ostream& operator<<(std::ostream& out, const activation_function_class& act)
{
    switch(act) {
        case activation_function_class::ReLu :
            return out<<"ReLu";
        case activation_function_class::sigmoid :
            return out<<"sigmoid";
        case activation_function_class::softmax :
            return out<<"softmax";
        default:
            return out<<"Did you pass an activation function as an argument?";
    }
}



template <class T>
T activation_function(activation_function_class act, T args)
{
    if (act==activation_function_class::ReLu)
        {return ReLu_function<variable<double>>(variable<double>(args));}
   else if (act==activation_function_class::sigmoid)
        {return sigmoid_function<variable<double>>(variable<double>(args));}
    else return variable<double>();
}

template <>
variable<column_vector> activation_function(activation_function_class act, variable<column_vector> args)
{
    if (act==activation_function_class::softmax)
        return softmax_function<variable<column_vector>>(args);
    else return variable<column_vector>();
}


#endif
