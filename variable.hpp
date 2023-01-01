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

template <class T>
struct decay_column_vector
{
    using type = T;
};

template <>
struct decay_column_vector<column_vector>
{
    using type = double;
};

template <class T>
using decay_column_vector_t =  typename decay_column_vector<T>::type;





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
template<>
struct derivate_type<double>
{
    using type=column_vector;
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
        variable():m_value(0.), m_derivative(column_vector()){};
        template <class E>
        variable(const E& e);
        T value() const;
        derivative_type_t<T> derivative() const;
        void activate(bool, std::size_t);
        void activate(bool);
        std::size_t size() const;
        double& value(std::size_t i){return m_value(i);}
        double& derivative(std::size_t i, std::size_t j){return m_derivative(i, j);}
    private:
        T m_value;
        derivative_type_t<T> m_derivative;
};
/*
template <>
class variable<column_vector>
{
    public:
    
};

template <>
class variable<double>
{
    public:
    double& value(){return m_value();}
    double& derivative(std::size_t i){return m_derivative(i);}
};
*/

template <class T>
std::size_t variable<T>::size() const {return 1;}
template <>
std::size_t variable<column_vector>::size() const {return m_value.size();}
template <>
variable<column_vector>::variable():m_value(column_vector()), m_derivative(matrix()){};


template <class E>
void print(const E& v);

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

template <class F, class E, class T>
class external_op
{
    public:
    
        external_op(const E&, const T& e);
        auto value() const;
        auto derivative() const;
    private:
        F m_f;
        E m_extern_operand;
        T m_variable_operand;
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
class n_ary_linear_combination
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


template <class E>
class n_ary_linear_combination
{public:

    auto value(const column_vector& w, const E& e) const
    {
        double res_value=0.;
        for(std::size_t i=0; i<e.size(); i++)
        {
            res_value+=w(i)*(e.value())(i);
        }
        return res_value;
    }

    auto derivative(const column_vector& w, const E& e) const
    {
        column_vector res_derivative(e.derivative().get_column(), 0.);
        for(std::size_t j=0; j<e.derivative().get_column(); j++)
        {   
            for(std::size_t i=0; i<e.size(); i++)
            {
                res_derivative(j)+=w(i)*e.derivative()(i, j);
            }
        }
        return res_derivative;
    }
};
template <class E>
class add_double_class
{
    public:
        auto value(double d, const E& e) const
        {
            return d+e.value();
        }

        auto derivative(double d, const E& e) const
        {
            return e.derivative();
        }
};

template <class E>
class multiply_double_class
{
    public:
        auto value(double d, const E& e) const
        {
            return d * e.value();
        }

        auto derivative(double d, const E& e) const
        {
            return d * e.derivative();
        }
};

template <class E>
class variable_divides_double_class
{
    public:
        auto value(double d, const E& e) const
        {
            return d / e.value();
        }

        auto derivative(double d, const E& e) const
        {
            return -d * e.derivative()/pow(e.value(),2);
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
        return (std::get<1>(e)).value() * (std::get<0>(e)).derivative()+(std::get<0>(e)).value() * (std::get<1>(e)).derivative();
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
        std::size_t n_data = (std::get<0>(e)).derivative().size();
        column_vector res_derivative(n_data);
        for(std::size_t i=0; i<e.size(); i++)
        {
            res_derivative(i) = ((std::get<0>(e)).derivative()(i)*(std::get<1>(e)).value()-(std::get<1>(e)).derivative()(i)*(std::get<0>(e)).value())/pow((std::get<1>(e)).value(), 2);
        }
        return res_derivative;
    }
};




template <class E>
class unary_minus
{
    public:
    auto value(const E& e) const
    {
        return -e.value();
    }
    auto derivative(const E& e) const
    {
        return -e.derivative();
    }
};

template <class E>
class unary_exp
{
    public:
    auto value(const E& e) const
    {
        return std::exp(e.value());
    }
    auto derivative(const E& e) const
    {
        return std::exp(e.value()) * e.derivative();
    }
};
template <>
class unary_exp<variable<column_vector>>
{
    public:
    auto value(const variable<column_vector>& e) const
    {
        column_vector res_value(e.size());
        for(std::size_t i=0; i<e.size(); i++) 
            {res_value(i) = std::exp(e.value()(i));}
        return res_value;
    }
    auto derivative(const variable<column_vector>& e) const
    {
        matrix res_derivative(e.derivative().get_row(), e.derivative().get_column());
        for(std::size_t i=0; i<e.derivative().get_row(); i++) 
        {
            for(std::size_t j=0; j<e.derivative().get_column(); j++) 
            {
                res_derivative(i, j) = e.derivative()(i, j) * std::exp(e.value()(i));
            }
        }
        return res_derivative;
    }
};


template <class E>
class unary_log
{
    public:
    auto value(const E& e) const
    {
        return std::log(e.value());
    }
    auto derivative(const E& e) const
    {
        return e.derivative()/e.value();
    }
};

template <>
class unary_log<variable<column_vector>>
{
    public:
    auto value(const variable<column_vector>& e) const
    {
        column_vector res_value(e.size());
        for(std::size_t i=0; i<e.size(); i++) 
            {res_value(i) = std::log(e.value()(i));}
        return res_value;
    }
    auto derivative(const variable<column_vector>& e) const
    {
        matrix res_derivative(e.derivative().get_row(), e.derivative().get_column());
        for(std::size_t i=0; i<e.size(); i++) 
        {
            for(std::size_t j=0; j<e.derivative().get_column(); j++) 
            {
                res_derivative(i, j) = e.derivative()(i, j) /e.value()(i);
            }
        }
        return res_derivative;
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


template <class E>
std::ostream& operator<<(std::ostream& out, const variable<E>& v)
{
   return out<<"value: "<<std::endl<<v.value()<<std::endl<<"derivative: "<<std::endl<<v.derivative()<<std::endl;
}



template <class E>
void print(const E& v)
{
    std::cout<<v;
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

template<class E>
external_op<add_double_class<E>, double, E> operator+(double d, const E& e)
{
    return external_op<add_double_class<E>, double, E> (d, e);
}
template<class E>
external_op<multiply_double_class<E>, double, E> operator*(double d, const E& e)
{
    return external_op<multiply_double_class<E>, double, E>(d, e);
}
template<class E>
external_op<variable_divides_double_class<E>, double, E> operator/(double d, const E& e)
{
    return external_op<variable_divides_double_class<E>, double, E>(d, e);
}


template <class E>
external_op<n_ary_linear_combination<E>, column_vector, E> linear_combination(const column_vector& w, const E& v)
{return external_op<n_ary_linear_combination<E>, column_vector, E>(w, v);}


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

////////////////////////////////////////////////////
//external_op
////////////////////////////////////////////////////


template <class F, class E, class T>
external_op<F, E, T>::external_op(const E& e, const T& v):m_f(), m_extern_operand(e), m_variable_operand(v)
{};
template <class F, class E, class T>
auto external_op<F, E, T>::value() const
{
    return m_f.value(m_extern_operand, m_variable_operand);
}
template <class F, class E, class T>
auto external_op<F, E, T>::derivative() const
{
    return m_f.derivative(m_extern_operand, m_variable_operand);
}

////////////////////////////////////////////////////
//double
////////////////////////////////////////////////////
#endif