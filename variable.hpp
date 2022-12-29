/* the goal of this file is to implement a template class variable + auto-diff structure we will then add these to neurons
*/
#ifndef VARIABLE_HPP
#define VARIABLE_HPP


#include "matrix.hpp"
#include<vector>
#include<iostream>
#include<cmath>
#include<tuple>
template <class... T>
using operand_type = std::tuple<T...>;

template<
    typename Tuple,
    typename Indices=std::make_index_sequence<std::tuple_size<Tuple>::value>>
struct runtime_get_func_table;

template<typename Tuple,size_t ... Indices>
    struct runtime_get_func_table<Tuple,std::index_sequence<Indices...>>{
    using return_type=typename std::tuple_element<0,Tuple>::type&;
    using get_func_ptr=return_type (*)(Tuple&) noexcept;
    static constexpr get_func_ptr table[std::tuple_size<Tuple>::value]={
        &std::get<Indices>...
    };
};

template<typename Tuple,size_t ... Indices>
constexpr typename
runtime_get_func_table<Tuple,std::index_sequence<Indices...>>::get_func_ptr
runtime_get_func_table<Tuple,std::index_sequence<Indices...>>::table[
    std::tuple_size<Tuple>::value];	
template<typename Tuple>
constexpr
typename std::tuple_element<0,typename std::remove_reference<Tuple>::type>::type&
runtime_get(Tuple&& t,size_t index){
    using tuple_type=typename std::remove_reference<Tuple>::type;
    if(index>=std::tuple_size<tuple_type>::value)
        throw std::runtime_error("Out of range");
    return runtime_get_func_table<tuple_type>::table[index](t);
}


template <class T>
struct derivate_type
{
    using type=T;
};
template <>
struct derivate_type<column_vector>
{
    using type=matrix;
};

template <class T>
using derivative_type_t = typename derivate_type<T>::type;



template <class T>
class variable
{
    public:
        variable(const T& value_output, const derivative_type_t<T>& derivative_output);
        explicit variable(const T& value_output);
        template <class E>
        variable(const E& e);
        T value() const;
        derivative_type_t<T> derivative() const;
        void activate(bool, std::size_t);
        void activate(bool);
        std::size_t size();
        template <class E>
        E& operator()(std::size_t i);
    private:
        T m_value;
        derivative_type_t<T> m_derivative;
};

template <class T>
void print(const variable<T>& v);

template <class F, class... T>
class n_ary_op
{
    public:
    
        n_ary_op(operand_type<const T&... > e);
        auto value() const;
        auto derivative() const;
    private:
        F m_f;
        operand_type<T... > m_operands;
};

template <class F, class T>
class unary_op
{
    public:
    
        unary_op(const T& e);
        auto value() const;
        auto derivative() const;
    private:
        F m_f;
        T m_operands;
};

template <class E1, class E2>
class binary_add
{
public:
    auto value(std::tuple<const E1&, const E2&> e) const
    {

        return (std::get<0>(e)).value() + (std::get<1>(e)).value();
    }

    auto derivative(std::tuple<const E1&, const E2&> e) const
    {
        return (std::get<0>(e)).derivative() + (std::get<1>(e)).derivative();
    }
};

template <class E1, class E2>
class binary_substract
{
public:
    auto value(std::tuple<const E1&, const E2&> e) const
    {

        return (std::get<0>(e)).value() - (std::get<1>(e)).value();
    }

    auto derivative(std::tuple<const E1&, const E2&> e) const
    {
        return (std::get<0>(e)).derivative() - (std::get<1>(e)).derivative();
    }
};


/*
template <class E, class W, class... Etail, class... Wtail>
class linear_combination
{public:

    auto value(const E& e, const Etail...& etail, const W& w, const Wtail...& wtail) const
    {
        return w.value() * e1.value() + value(etail, wtail);
    }

    auto derivative(const E& e, const Etail...& etail, const W& w, const Wtail...& wtail) const
    {
        return w.value()*e.derivative()+w.value()*e.derivative() + derivative(etail, wtail);
    }
};

template <class Eonly, class Wonly>
class linearcombination<Eonly, Wonly>
{
    auto value(const Eonly& e, const Wonly w)
    {
        return e.value()*w.value();
    }
    auto derivative(const Eonly& e, const Wonly w)
    {
        return e.value()*w.derivative()+e.derivative()*w.value();
    }
}*/


template <class E, class W>
class linear_combination
{public:

    auto value(const std::vector<W>& w, const E& e) const
    {
        W res_value=0.;///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for(std::size_t i=0; i<e.size(); i++)
        {
            res_value+=w[i]*e.value()[i];
        }
        return res_value;
    }

    auto derivative(const std::vector<W>& w, const E& e) const
    {
        W res_derivative=0.;///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for(std::size_t i=0; i<e.size(); i++)
        {
            res_derivative+=w[i]*e.derivative()[i];
        }
        return res_derivative;
    }
};





template <class E1, class E2>
class binary_multiply
{
public:
    auto value(std::tuple<const E1&, const E2&> e) const
    {

        return (std::get<0>(e)).value() * (std::get<1>(e)).value();
    }

    auto derivative(std::tuple<const E1&, const E2&> e) const
    {
        return (std::get<0>(e)).derivative()*(std::get<1>(e)).value()+(std::get<1>(e)).derivative()*(std::get<0>(e)).value();
    }
};

template <class E1, class E2>
class binary_divide
{
public:
    auto value(std::tuple<const E1&, const E2&> e) const
    {

        return (std::get<0>(e)).value() / (std::get<1>(e)).value();
    }

    auto derivative(std::tuple<const E1&, const E2&> e) const
    {
        return ((std::get<0>(e)).derivative()*(std::get<1>(e)).value()-(std::get<1>(e)).derivative()*(std::get<0>(e)).value())/pow((std::get<1>(e)).value(), 2);
    }
};




template <class E>
class unary_minus
{
    public:
    auto value(std::tuple<const E&> e) const
    {
        return -(std::get<0>(e)).value();
    }
    auto derivative(std::tuple<const E&> e) const
    {
        return -(std::get<0>(e)).derivative();
    }
};

template <class E>
class unary_exp
{
    public:
    auto value(std::tuple<const E&> e) const
    {
        return std::exp((std::get<0>(e)).value());
    }
    auto derivative(std::tuple<const E&> e) const
    {
        return (std::get<0>(e)).derivative()*std::exp((std::get<0>(e)).value());
    }
};

template <class E>
class unary_log
{
    public:
    auto value(std::tuple<const E&> e) const
    {
        return std::log((std::get<0>(e)).value());
    }
    auto derivative(std::tuple<const E&> e) const
    {
        return (std::get<0>(e)).derivative()/e.value();
    }

};


template <class E1, class E2>
n_ary_op<binary_add<E1, E2>, E1, E2> operator+(const E1& e1, const E2& e2);
template <class E1, class E2>
n_ary_op<binary_multiply<E1, E2>, E1, E2> operator*(const E1& e1, const E2& e2);
template <class E1, class E2>
n_ary_op<binary_substract<E1, E2>, E1, E2> operator-(const E1& e1, const E2& e2);
template <class E1, class E2>
n_ary_op<binary_divide<E1, E2>, E1, E2> operator/(const E1& e1, const E2& e2);
template <class E>
unary_op<unary_minus<E>, E> operator-(const E& e);
template <class E>
unary_op<unary_exp<E>, E> exp(const E& e);
template <class E>
unary_op<unary_log<E>, E> log(const E& e);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Implementation of header files
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////
//Variable
////////////////////////////////////////////////////

template <class T>
variable<T>::variable(const T& value_output, const derivative_type_t<T>& derivative_output):m_value(value_output), m_derivative(derivative_output)
{};
template<class T>
variable<T>::variable(const T& value_output): m_value(value_output), m_derivative(0)
{};
template <class T>
template <class E>    
variable<T>::variable(const E& e):m_value(e.value()), m_derivative(e.derivative())
{};

template <class T> 
void variable<T>::activate(bool b)
{
    m_derivative=b;
}

template <class T> 
void variable<T>::activate(bool b, std::size_t i)
{
    m_derivative[i]=b;
}

template <class T>
T variable<T>::value() const{return m_value;}
template <class T>
derivative_type_t<T> variable<T>::derivative() const{return m_derivative;}


template <class T>
std::ostream& operator<<(std::ostream& out, const variable<T>& v)
{
   return out<<"value: "<<std::endl<<v.value()<<"derivative: "<<std::endl<<v.derivative();
}

template <class T>
template <class E>
E& variable<T>::operator()(std::size_t i){return *this;}

template <>
template <class E>
E& variable<column_vector>::operator()(std::size_t i){return variable(m_value(i), m_derivative(i));}


template <class T>
std::size_t variable<T>::size(){return 1;}
template <>
std::size_t variable<column_vector>::size(){return m_value.size();}


template <class T>
void print(const variable<T>& v)
{
    std::cout<<v<<std::endl;
}
////////////////////////////////////////////////////
//operations on variable
////////////////////////////////////////////////////
template<class E1, class E2>
n_ary_op<binary_add<E1, E2>, E1, E2> operator+(const E1& e1, const E2& e2)
{
    return n_ary_op<binary_add<E1, E2>, E1, E2>(std::tuple<const E1&, const E2&> (e1, e2));
}
template <class E1, class E2>
n_ary_op<binary_multiply<E1, E2>, E1, E2> operator*(const E1& e1, const E2& e2)
{return n_ary_op<binary_multiply<E1, E2>, E1, E2>(std::tuple<const E1&, const E2&> (e1, e2));}
template <class E1, class E2>
n_ary_op<binary_substract<E1, E2>, E1, E2> operator-(const E1& e1, const E2& e2)
{return n_ary_op<binary_substract<E1, E2>, E1, E2>(std::tuple<const E1&, const E2&> (e1, e2));}
template <class E1, class E2>
n_ary_op<binary_divide<E1, E2>, E1, E2> operator/(const E1& e1, const E2& e2)
{return n_ary_op<binary_divide<E1, E2>, E1, E2>(std::tuple<const E1&, const E2&> (e1, e2));}

template <class E>
unary_op<unary_minus<E>, E> operator-(const E& e)
{return unary_op<unary_minus<E>, E>(e);}

template <class E>
unary_op<unary_exp<E>, E> exp(const E& e)
{return unary_op<unary_exp<E>, E>(e);}

template <class E>
unary_op<unary_log<E>, E> log(const E& e)
{return unary_op<unary_log<E>, E>(e);}


////////////////////////////////////////////////////
//n_ary_op
////////////////////////////////////////////////////


template <class F, class... T>
n_ary_op<F, T...>::n_ary_op(operand_type<const T&... > e):m_f(), m_operands(e)
{};
template <class F, class... T>
auto n_ary_op<F, T...>::value() const
{
    return m_f.value(m_operands);
}
template <class F, class... T>
auto n_ary_op<F, T...>::derivative() const
{
    return m_f.derivative(m_operands);
}


////////////////////////////////////////////////////
//unary_op
////////////////////////////////////////////////////


template <class F, class T>
unary_op<F, T>::unary_op(const T& e):m_f(), m_operands(e)
{};
template <class F, class T>
auto unary_op<F, T>::value() const
{
    return m_f.value(m_operands);
}
template <class F, class T>
auto unary_op<F, T>::derivative() const
{
    return m_f.derivative(m_operands);
}
#endif