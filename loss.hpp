#ifndef LOSS_HPP
#define LOSS_HPP

#include "variable.hpp"
////////////////////////////////////////////////////////////////////////////////////////
// Loss classes
////////////////////////////////////////////////////////////////////////////////////////

class binary_cross_entropy_class
{
    public:
        auto value(const column_vector& objective, const variable<column_vector>& arg) const
        {
            double res_value = 0;
            std::size_t n = objective.size();
            for(std::size_t i=0; i<n; i++)
            {
                res_value+=objective(i)*std::log(arg.value(i))+(1-objective(i)) * std::log(1 - arg.value(i));
            }
            return -res_value/double(n);
        }
    
        auto derivative(const column_vector& objective, const variable<column_vector>& arg) const
        {
            column_vector res_derivative(arg.derivative().get_column(), 0.);
            std::size_t n = objective.size();
            for(std::size_t j=0; j<arg.derivative().get_column(); j++)
                for(std::size_t i=0; i<n; i++)
                {
                    res_derivative(j)+=objective(i)*arg.derivative(i, j)/arg.value(i)-(1-objective(i)) *arg.derivative(i, j)/ (1 - arg.value(i));
                }
            return -res_derivative/double(n);
        }

};


class categorical_cross_entropy_class
{
    public:
        auto value(const column_vector& objective, const variable<column_vector>& arg) const
        {
            double res_value = 0;
            std::size_t n = objective.size();
            for(std::size_t i=0; i<n; i++)
            {
                res_value+= objective(i) * std::log(arg.value(i));
            }
            return -res_value;
        }
        auto derivative(const column_vector& objective, const variable<column_vector>& arg) const
        {
            column_vector res_derivative(arg.derivative().get_column(), 0.);
            std::size_t n = objective.size();
            for(std::size_t j=0; j<arg.derivative().get_column(); j++)
            {
                for(std::size_t i=0; i<n; i++)
                {
                    res_derivative(j)+= objective(i) * arg.derivative(i, j)/arg.value(i);
                }
            }
            return -res_derivative;
        }
};





class MSE_class
{
    public:
        auto value(const column_vector& objective, const variable<column_vector>& arg) const
        {
            double res_value = 0;
            std::size_t n = objective.size();
            for(std::size_t i=0; i<n; i++)
            {
                res_value+= pow(arg.value(i) -objective(i), 2);
            }
            return res_value/double(n);
        }
        auto derivative(const column_vector& objective, const variable<column_vector>& arg) const
        {
            column_vector res_derivative(arg.derivative().get_column(), 0.);
            std::size_t n = objective.size();
            for(std::size_t j=0; j<arg.derivative().get_column(); j++)
            {
                for(std::size_t i=0; i<n; i++)
                {
                    res_derivative+= 2. * (arg.value(i) - objective(i)) * arg.derivative(i, j);
                }
            }
            return res_derivative/double(n);
        }
    

};


////////////////////////////////////////////////////////////////////////////////////////
// Loss functions
////////////////////////////////////////////////////////////////////////////////////////

external_op<binary_cross_entropy_class, column_vector, variable<column_vector>> binary_cross_entropy_function(const column_vector& objective, const variable<column_vector>& arg)
{
    return external_op<binary_cross_entropy_class, column_vector, variable<column_vector>>(objective, arg);
}

external_op<categorical_cross_entropy_class, column_vector, variable<column_vector>> categorical_cross_entropy_function(const column_vector& objective, const variable<column_vector>& arg)
{
    return external_op<categorical_cross_entropy_class, column_vector, variable<column_vector>>(objective, arg);
}

external_op<MSE_class, column_vector, variable<column_vector>> MSE_function(const column_vector& objective, const variable<column_vector>& arg)
{
    return external_op<MSE_class, column_vector, variable<column_vector>>(objective, arg);
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



template <class T>
variable<double> loss_function(loss_class loss, const column_vector& objective, T args)
{
    if (loss==loss_class::binary_cross_entropy)
        {return binary_cross_entropy_function(objective, variable<column_vector>(args));}
   else if (loss==loss_class::categorical_cross_entropy)
        {return categorical_cross_entropy_function(objective, variable<column_vector>(args));}
    else if (loss==loss_class::MSE)
        {return categorical_cross_entropy_function(objective, variable<column_vector>(args));}
    else return variable<double>();
}




#endif