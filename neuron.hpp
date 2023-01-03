#ifndef NEURON_HPP
#define NEURON_HPP
#include "activationfunction.hpp"
#include "loss.hpp"

class neuron
{
    public:
        neuron(column_vector weight, activation_function_class actfunc);
        neuron(std::size_t i, activation_function_class act_func_arg):m_weight(column_vector(i)), m_actfunc(act_func_arg){}; 
        neuron():neuron(1, activation_function_class::ReLu){};
        
    
        neuron(neuron&& n):m_weight(n.m_weight), m_actfunc(n.m_actfunc){};
        neuron(neuron& n);
        neuron(const neuron& n);
        neuron& operator=(const neuron& n);
        neuron& operator=(neuron&& rhs);
    
        variable<double> evaluate(const variable<column_vector>& var) const;
        double value(const column_vector& data) const;
    
        column_vector weight() const{return m_weight;}
        double& weight(std::size_t i){return m_weight(i);}
        double weight(std::size_t i) const{return m_weight(i);}
        activation_function_class act_func() const{return m_actfunc;}
        void weight_adress(){m_weight.vector_adress();}
    
        std::size_t nbr_weight() const {return m_weight.size();}
    private:
        column_vector m_weight;
        activation_function_class m_actfunc;
};


std::ostream& operator<<(std::ostream& out, const neuron& n)
{
   return out<<"weight:"<<std::endl<<n.weight()<<std::endl<<"activation function:"<<std::endl<< n.act_func()<<std::endl;
}

class neural_layer
{
    public:
        std::vector<neuron> neuron_list;
    
    public:
        neural_layer(std::size_t n_neurons, std::size_t n_weight, const std::initializer_list<activation_function_class>& list_act_func);
        neural_layer(std::size_t n_neurons, std::size_t n_weight, activation_function_class act_func);
        neural_layer(std::size_t n_neurons, std::size_t n_weight);

        
    
        neural_layer(const std::vector<neuron>&);
        neural_layer(std::size_t i):neuron_list(i){};
        neural_layer():neural_layer(0){};
    
        neural_layer(neural_layer&& n);
        neural_layer(neural_layer& n);
        neural_layer(const neural_layer& n);
        neural_layer& operator=(const neural_layer& n);
        neural_layer& operator=(neural_layer&& rhs);
        
        
        variable<column_vector> evaluate(const variable<column_vector>&) const;
        column_vector value(const column_vector& data) const;
        
        std::size_t size() const{return neuron_list.size();}
        
};

column_vector get_weight(std::vector<neural_layer>);
class neural_network
{
    public:
        neural_network(std::size_t size_data, const std::initializer_list<std::size_t>& c, loss_class loss_func_arg, const std::initializer_list<std::initializer_list<activation_function_class>>& list_act_func);
    
        neural_network(std::size_t size_data, const std::initializer_list<std::size_t>& nbr_neurons_per_layer_list, loss_class loss_func_arg, activation_function_class act_func_arg);
    
        neural_network(std::size_t size_data, const std::initializer_list<std::size_t>& c, loss_class loss_func_arg);
    
    
        neural_network(std::vector<neural_layer>, loss_class);
    
        neural_network(neural_network&& n):layer_list(n.layer_list), lossfunc(n.lossfunc){};
        neural_network(neural_network& n);
        neural_network(const neural_network& n);
        neural_network& operator=(const neural_network& n);
        neural_network& operator=(neural_network&& rhs);
    
        variable<column_vector> evaluate(const column_vector& data) const;
        template <class T>
        variable<double> evaluate_loss(const std::vector<T>& objective, const std::vector<variable<column_vector>>& res_evaluate)const;
    
        column_vector value(const column_vector& data) const;
        template <class T>
        double value_loss(const std::vector<T>& objective, const std::vector<column_vector>& res_value)const;
        
        std::size_t size() const {return layer_list.size();}
        std::size_t size_layer(std::size_t i) const{return layer_list[i].size();}
    
        void gradient_descent(double epsilon, variable<double> res_evaluate_loss);
        template <class E>
        void gradient_descent(std::size_t n_iterations, double epsilon, const std::vector<E>& objective, const std::vector<column_vector>& data);
        
    private:
        std::vector<neural_layer> layer_list;
        loss_class lossfunc;
        
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////neuron
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
neuron::neuron(column_vector weight, activation_function_class actfunc):m_weight(weight), m_actfunc(actfunc){};

neuron::neuron(const neuron& n)
{
    m_weight = n.m_weight;
    m_actfunc=n.m_actfunc;
}
neuron& neuron::operator=(const neuron& n)
{
    m_weight = n.m_weight;
    m_actfunc=n.m_actfunc;
    return *this;
}
neuron& neuron::operator=(neuron&& rhs)
{
    using std::swap;
    swap(m_weight, rhs.m_weight);
    m_actfunc=rhs.m_actfunc;
    return *this;
}
variable<double> neuron::evaluate(const variable<column_vector>& var) const
{
    return activation_function(m_actfunc, variable<double>(linear_combination_for_neurons(m_weight, var)));
}


double neuron::value(const column_vector& data) const
{
    double arg_activation = 0.;
    for(std::size_t i=0; i< data.size(); i++)
    {
        arg_activation+=m_weight(i)*data(i);
    }
    return activation_function_value(m_actfunc, arg_activation);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////neural layer
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
neural_layer::neural_layer(std::size_t n_neurons, std::size_t n_weight, const std::initializer_list<activation_function_class>& list_act_func)
{
    neuron_list.resize(n_neurons);

    for(std::size_t i=0; i<n_neurons; i++)
    {
        neuron_list[i] = neuron(n_weight, list_act_func.begin()[i]);
    }
    
}


neural_layer::neural_layer(std::size_t n_neurons, std::size_t n_weight, activation_function_class act_func)
{
    neuron_list.resize(n_neurons);
    
    for(std::size_t i=0; i<n_neurons; i++)
    {
        neuron_list[i] = neuron(n_weight, act_func);
    }
    
}
neural_layer::neural_layer(std::size_t n_neurons, std::size_t n_weight):neural_layer(n_neurons, n_weight, activation_function_class::ReLu)
{};


neural_layer::neural_layer(const std::vector<neuron>& arg)
{
    neuron_list.resize(arg.size());
    for(std::size_t i=0; i<arg.size(); i++)
        {
            neuron_list[i] = arg[i];
        }
}

neural_layer::neural_layer(neural_layer&& n)
         {
            neuron_list.resize(n.neuron_list.size());
            for(std::size_t i=0; i<neuron_list.size(); i++)
            {
                neuron_list[i] = n.neuron_list[i];
            }
         }
neural_layer::neural_layer(neural_layer& n)
{
    neuron_list.resize(n.neuron_list.size());
    for(std::size_t i=0; i<neuron_list.size(); i++)
    {
        neuron_list[i] = n.neuron_list[i];
    }
 }
neural_layer::neural_layer(const neural_layer& n)
{
    neuron_list.resize(n.neuron_list.size());
    for(std::size_t i=0; i<neuron_list.size(); i++)
    {
        neuron_list[i] = n.neuron_list[i];
    }
 }
neural_layer& neural_layer::operator=(const neural_layer& n)
{
    neuron_list.resize(n.neuron_list.size());
    for(std::size_t i=0; i<neuron_list.size(); i++)
    {
        neuron_list[i] = n.neuron_list[i];
    }
    return *this;
 }
neural_layer& neural_layer::operator=(neural_layer&& rhs)
{
    using std::swap;
    swap(neuron_list, rhs.neuron_list);
    return *this;
}

variable<column_vector> neural_layer::evaluate(const variable<column_vector>& arg) const
{
    
    std::size_t nbr_neurons = neuron_list.size();
    
    variable<double> tmp(0., column_vector(arg.derivative().get_column()));
    
    std::size_t nbr_total_weight(0);
    for(std::size_t i=0; i< nbr_neurons; i++)
    {
        nbr_total_weight+=neuron_list[i].nbr_weight();
    }
    variable<column_vector> res(column_vector(nbr_neurons), matrix(nbr_neurons, arg.derivative().get_column()+nbr_total_weight));
    //print(arg);
    
    std::size_t counter_weight_derivative=0;
    for(std::size_t i=0; i< nbr_neurons; i++)
    {
    
        if (neuron_list[i].act_func()==activation_function_class::softmax)
        {
            res = activation_function(activation_function_class::softmax, arg);
        }
        else
        {
            tmp = neuron_list[i].evaluate(arg);
            res.value(i) = tmp.value();
            for(std::size_t j=0; j< arg.derivative().get_column(); j++)
            {
                res.derivative(i, j) = tmp.derivative()(j);
            }
            
            for(std::size_t j=0; j<neuron_list[i].nbr_weight(); j++)
            {
                res.derivative(i, counter_weight_derivative+j) = tmp.derivative()(j);
            }
            counter_weight_derivative+= neuron_list[i].nbr_weight();
        }
    }
    print(res.value());
    return res;
}

column_vector neural_layer::value(const column_vector& data) const
{
    std::size_t nbr_neurons = neuron_list.size();
    column_vector res(nbr_neurons);
    
    for(std::size_t i=0; i< nbr_neurons; i++)
    {
        if (neuron_list[i].act_func()==activation_function_class::softmax)
        {
            double denom=0;
            for(std::size_t j=0; j< data.size(); j++)
            {
                denom+= std::exp(data(i));
            }
            for(std::size_t j=0; j< data.size(); j++)
                res(j) = std::exp(data(i));
            return res/denom;
        }
        else
        {
            res(i) = neuron_list[i].value(data);
        }
    }
    return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////neural network
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

neural_network::neural_network(std::size_t size_data, const std::initializer_list<std::size_t>& nbr_neurons_per_layer_list, loss_class loss_func_arg, const std::initializer_list<std::initializer_list<activation_function_class>>& list_act_func):lossfunc(loss_func_arg)
{
    layer_list.resize(nbr_neurons_per_layer_list.size());
    
    layer_list[0] = neural_layer(nbr_neurons_per_layer_list.begin()[0], size_data, list_act_func.begin()[0]);
    
    for(std::size_t i=1; i<nbr_neurons_per_layer_list.size();i++)
    {
        layer_list[i] = neural_layer(nbr_neurons_per_layer_list.begin()[i], nbr_neurons_per_layer_list.begin()[i-1], list_act_func.begin()[i]);
    }
}


neural_network::neural_network(std::size_t size_data, const std::initializer_list<std::size_t>& nbr_neurons_per_layer_list, loss_class loss_func_arg, activation_function_class act_func_arg):lossfunc(loss_func_arg)
{
    layer_list.resize(nbr_neurons_per_layer_list.size());
    
    layer_list[0] = neural_layer(nbr_neurons_per_layer_list.begin()[0], size_data, act_func_arg);
    
    for(std::size_t i=1; i<nbr_neurons_per_layer_list.size();i++)
    {
        layer_list[i] = neural_layer(nbr_neurons_per_layer_list.begin()[i], nbr_neurons_per_layer_list.begin()[i-1], act_func_arg);
    }
    
}
    
neural_network::neural_network(std::size_t size_data, const std::initializer_list<std::size_t>& c,loss_class loss_func_arg ):neural_network(size_data, c, loss_func_arg, activation_function_class::ReLu)
{};

neural_network::neural_network(std::vector<neural_layer> layer_list_arg, loss_class lossfunc_arg): layer_list(layer_list_arg), lossfunc(lossfunc_arg)
{};

neural_network::neural_network(neural_network& n)
{
    layer_list.resize(n.size());
    for(std::size_t i=0; i<size(); i++)
    {
        layer_list[i] = n.layer_list[i];
    }
}
neural_network::neural_network(const neural_network& n)
{
    layer_list.resize(n.size());
    for(std::size_t i=0; i<size(); i++)
    {
        layer_list[i] = n.layer_list[i];
    }
}
neural_network& neural_network::operator=(const neural_network& n)
{
    layer_list.resize(n.size());
    for(std::size_t i=0; i<size(); i++)
    {
        layer_list[i] = n.layer_list[i];
    }
    return *this;
}
neural_network& neural_network::operator=(neural_network&& n)
{
    layer_list.resize(n.size());
    for(std::size_t i=0; i<size(); i++)
    {
        layer_list[i] = n.layer_list[i];
    }
    return *this;
}





variable<column_vector> neural_network::evaluate(const column_vector& data) const
{ 

    variable<column_vector> propagated_variable(data, matrix(0, 0, 0.));
    for(std::size_t i=0; i<layer_list.size(); i++)
    {
    
        propagated_variable=layer_list[i].evaluate(propagated_variable);
    }
    return propagated_variable;
}

template <class T>
variable<double> neural_network::evaluate_loss(const std::vector<T>& objective, const std::vector<variable<column_vector>>& res_evaluate)const
{ 
    return loss_function(lossfunc, objective, res_evaluate);
}

    
column_vector neural_network::value(const column_vector& data) const
{
    column_vector propagated_value(data);
    for(std::size_t i=0; i<layer_list.size(); i++)
    {
        propagated_value=layer_list[i].value(propagated_value);
    }
    return propagated_value;
}

template <class T>
double neural_network::value_loss(const std::vector<T>& objective, const std::vector<column_vector>& res_value) const
{
    return loss_function_value<T>(lossfunc, objective, res_value);
}


void neural_network::gradient_descent(double epsilon, variable<double> res_evaluate_loss)
{
    std::size_t current_derivative_index = 0;
    for(std::size_t i=0; i<size(); i++)
    {
        for (std::size_t j=0; j<size_layer(i); j++)
         {
            if (layer_list[i].neuron_list[j].act_func()!=activation_function_class::softmax)
            {
                for(std::size_t k=0; k<layer_list[i].neuron_list[j].nbr_weight(); k++)
                {
                    layer_list[i].neuron_list[j].weight(k) -= epsilon * res_evaluate_loss.derivative()(current_derivative_index);
                    current_derivative_index+=1;
                }
            }
         }
    }
}

template <class T>
void neural_network::gradient_descent(std::size_t n_iterations, double epsilon, const std::vector<T>& objective, const std::vector<column_vector>& data)
{
    std::size_t n_data = data.size();
    std::vector<variable<column_vector>> estimator_data(n_data);
    for(std::size_t i=0; i<n_iterations; i++)
    {
        for(std::size_t j=0; j<n_data; j++)
        {
           estimator_data[i] = evaluate(data[i]);
        }
        gradient_descent(epsilon, evaluate_loss<T>(objective, estimator_data));
    }
}



#endif