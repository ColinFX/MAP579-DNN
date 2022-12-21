#ifndef LOSS_HPP
#define LOSS_HPP

#include "variable.hpp"
#include "miscellaneous.hpp"

template <class T>
class loss
{
    
};
template <class T>
class MpE: public loss<T>
{
    public:

        const std::vector<T>& get_objective() const
        {
            return m_objective;
        }
        void print_objective() const
        {
            std::cout<<'{';
            for(std::size_t i=0; i<m_objective.size(); i++)
            {
                std::cout<<m_data[i];
                if (i<m_data.size()-1) std::cout<<", ";
            }
            std::cout<<'}'<<std::endl;
        }
        
        T value(std::vector<variable> estimator) const
        {
            T Mp_error=0;
            std::size_t n_data = estimator.size();
            assert n_data = m_objective.size();
            for(std::size_t i=0; i<n_data; i++)
            {
                Mp_error+=distp_value(m_objective, estimator, p);
            }
        return Mp_error/n_data;
        }
        
        T derivative(std::vector<variable<T>> estimator) const
        {
            T Mp_error_derivative=0;
            std::size_t n_data = estimator.size();
            assert n_data = m_objective.size();
            for(std::size_t i=0; i<n_data; i++)
            {
                Mp_error_derivative+=distp_derivative(m_objective, estimator, p);
            }
        return Mp_error_derivative/n_data;
        }
        
    private:
        const std::vector<T>& m_objective; //original goal
        const double p;
}

template <class T>
    
class MAE: public MpE<T>
{
    private:
        const double p=2;
};

template <class T>
class M1E: public MpE<T>
{
    private:
        const double p=1;
};
#endif