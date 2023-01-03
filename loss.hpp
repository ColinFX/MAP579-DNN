#ifndef LOSS_HPP
#define LOSS_HPP

#include "variable.hpp"
////////////////////////////////////////////////////////////////////////////////////////
// Loss classes
////////////////////////////////////////////////////////////////////////////////////////

class binary_cross_entropy_class
{
    public:
        auto value(const std::vector<double>& objective, const std::vector<variable<column_vector>>& arg) const
        {
            double res_value = 0;
            std::size_t n = objective.size();
            for(std::size_t i=0; i<n; i++)
            {
                res_value+=objective[i]*std::log(arg[i].value(0))+(1-objective[i]) * std::log(1 - arg[i].value(0));
            }
            return -res_value/double(n);
        }
    
        auto derivative(const std::vector<double>& objective, const std::vector<variable<column_vector>>& arg) const
        {
            column_vector res_derivative(arg[0].derivative().get_column(), 0.);
            std::size_t n = objective.size();
            for(std::size_t j=0; j<arg[0].derivative().get_column(); j++)
                for(std::size_t i=0; i<n; i++)
                {
                    res_derivative(j)+=objective[i]*arg[i].derivative(0, j)/arg[i].value(0)-(1-objective[i]) *arg[i].derivative(0, j)/ (1 - arg[i].value(0));
                }
            return -res_derivative/double(n);
        }

};

double binary_cross_entropy_value(const std::vector<double>& objective, const std::vector<column_vector>& arg)
{
    double res_value = 0;
    std::size_t n = objective.size();
    for(std::size_t i=0; i<n; i++)
    {
        res_value+=objective[i]*std::log(arg[i](0))+(1-objective[i]) * std::log(1 - arg[i](0));
    }
    return -res_value/double(n);
}

class categorical_cross_entropy_class
{
    public:
        auto value(const std::vector<double>& objective, const std::vector<variable<column_vector>>& arg) const
        {
            double res_value = 0;
            std::size_t n = objective.size();
            for(std::size_t i=0; i<n; i++)
            {
                res_value+= objective[i] * std::log(arg[i].value(0));
            }
            return -res_value;
        }
        auto derivative(const std::vector<double>& objective, const std::vector<variable<column_vector>>& arg) const
        {
            column_vector res_derivative(arg[0].derivative().get_column(), 0.);
            std::size_t n = objective.size();
            for(std::size_t j=0; j<arg[0].derivative().get_column(); j++)
            {
                for(std::size_t i=0; i<n; i++)
                {
                    res_derivative(j)+= objective[i] * arg[i].derivative(0, j)/arg[0].value(0);
                }
            }
            return -res_derivative;
        }
};


double categorical_cross_entropy_value(const std::vector<double>& objective, const std::vector<column_vector>& arg)
        {
            double res_value = 0;
            std::size_t n = objective.size();
            for(std::size_t i=0; i<n; i++)
            {
                res_value+= objective[i] * std::log(arg[i](0));
            }
            return -res_value;
        }


template <class E>
class MSE_class
{
    public:
        auto value(const std::vector<E>& objective, const std::vector<variable<column_vector>>& arg) const
        {
            double res_value = 0;
            std::size_t n = objective.size();
            for(std::size_t index_data=0; index_data<n; index_data++)
            {
                for(std::size_t i=0; i<objective[0].size(); i++)
                {
                    res_value+= pow(arg[index_data].value(i) -objective[index_data](i), 2);
                }
            }
            return res_value/double(n);
        }
        auto derivative(const std::vector<E>& objective, const std::vector<variable<column_vector>>& arg) const
        {
            column_vector res_derivative(arg[0].derivative().get_column(), 0.);
            std::size_t n = objective.size();
            for(std::size_t j=0; j<arg[0].derivative().get_column(); j++)
            {
                for(std::size_t index_data=0; index_data<n; index_data++)
                {
                    for(std::size_t i=0; i<objective[0].size(); i++)
                    {
                        res_derivative(j)+= 2. * (arg[index_data].value(i) - objective[index_data](i)) * arg[index_data].derivative(i, j);
                    }
                }
            }
            return res_derivative/double(n);
        }
};

template <>
class MSE_class<double>
{
    public:
        auto value(const std::vector<double>& objective, const std::vector<variable<column_vector>>& arg) const
        {
            double res_value = 0;
            std::size_t n = objective.size();
            for(std::size_t index_data=0; index_data<n; index_data++)
            {
                    res_value+= pow(arg[index_data].value(0) -objective[index_data], 2);
            }
            return res_value/double(n);
        }
        auto derivative(const std::vector<double>& objective, const std::vector<variable<column_vector>>& arg) const
        {
            column_vector res_derivative(arg[0].derivative().get_column(), 0.);
            std::size_t n = objective.size();
            for(std::size_t j=0; j<arg[0].derivative().get_column(); j++)
            {
                for(std::size_t index_data=0; index_data<n; index_data++)
                {
                        res_derivative(j)+= 2. * (arg[index_data].value(0) - objective[index_data]) * arg[index_data].derivative(0, j);
                }
            }
            return res_derivative/double(n);
        }
};

template <class E>
double MSE_value(const std::vector<E>& objective, const std::vector<column_vector>& arg) 
{
    double res_value = 0;
    std::size_t n = objective.size();
    for(std::size_t index_data=0; index_data<n; index_data++)
    {
        for(std::size_t i=0; i<objective[0].size(); i++)
        {
            res_value+= pow(arg[index_data](i) -objective[index_data](i), 2);
        }
    }
    return res_value/double(n);
}

template <>

double MSE_value<double>(const std::vector<double>& objective, const std::vector<column_vector>& arg) 
{
    double res_value = 0;
    std::size_t n = objective.size();
    for(std::size_t index_data=0; index_data<n; index_data++)
    {
            res_value+= pow(arg[index_data](0) -objective[index_data], 2);
    }
    return res_value/double(n);
}


////////////////////////////////////////////////////////////////////////////////////////
// Loss functions
////////////////////////////////////////////////////////////////////////////////////////

variable<double> binary_cross_entropy_function(const std::vector<double>& objective, const std::vector<variable<column_vector>>& arg)
{
    binary_cross_entropy_class m_f;
    return variable<double>(m_f.value(objective, arg), m_f.derivative(objective, arg));
}

variable<double> categorical_cross_entropy_function(const std::vector<double>& objective, const std::vector<variable<column_vector>>& arg)
{
    categorical_cross_entropy_class m_f;
    return variable<double>(m_f.value(objective, arg), m_f.derivative(objective, arg));
}

template <class E>
variable<double> MSE_function(const std::vector<E>& objective, const std::vector<variable<column_vector>>& arg)
{
    MSE_class<E> m_f;
    return variable<double>(m_f.value(objective, arg), m_f.derivative(objective, arg));
}


////////////////////////////////////////////////////////////////////////////////////////
// Loss dictionary
////////////////////////////////////////////////////////////////////////////////////////

enum class loss_class{binary_cross_entropy, categorical_cross_entropy, MSE};

std::ostream& operator<<(std::ostream& out, const loss_class& loss)
{
    switch(loss) {
        case loss_class::binary_cross_entropy :
            return out<<"binary_cross_entropy";
        case loss_class::categorical_cross_entropy :
            return out<<"sigmoid";
        case loss_class::MSE :
            return out<<"MSE";
        default:
            return out<<"Did you pass a loss function as an argument?";
    }
}



template <class E>
variable<double> loss_function(loss_class loss, const std::vector<E>& objective, const std::vector<variable<column_vector>>& args)
{
    if (loss==loss_class::binary_cross_entropy)
        {return binary_cross_entropy_function(objective, args);}
   else if (loss==loss_class::categorical_cross_entropy)
        {return categorical_cross_entropy_function(objective, args);}
    else if (loss==loss_class::MSE)
        {return MSE_function<E>(objective, args);}
    else return variable<double>();
}


template <class E>
double loss_function_value(loss_class loss, const std::vector<E>& objective, const std::vector<column_vector>& args)
{
    if (loss==loss_class::binary_cross_entropy)
        {return binary_cross_entropy_value(objective, args);}
   else if (loss==loss_class::categorical_cross_entropy)
            {return categorical_cross_entropy_value(objective, args);}
    else if (loss==loss_class::MSE)
        {return MSE_value<E>(objective,args);}
    else return 0.;
}




#endif