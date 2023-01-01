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
        variable<double> evaluate(const variable<column_vector>& var);
    
        neuron(neuron&& n):m_weight(n.m_weight), m_actfunc(n.m_actfunc){};
        neuron(neuron& n);
        neuron(const neuron& n);
        neuron& operator=(const neuron& n);
        neuron& operator=(neuron&& rhs);
    
        column_vector weight() const{return m_weight;}
        activation_function_class act_func() const{return m_actfunc;}
        void weight_adress(){m_weight.vector_adress();}
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
        neural_layer(const std::vector<neuron>&);
        neural_layer(std::size_t i):neuron_list(i){};
    
        neural_layer(neural_layer&& n);
        neural_layer(neural_layer& n);
        neural_layer(const neural_layer& n);
        neural_layer& operator=(const neural_layer& n);
        neural_layer& operator=(neural_layer&& rhs);
        
        
        variable<column_vector> evaluate(const variable<column_vector>&);
        std::size_t size() const{return neuron_list.size();}
    private:
        std::vector<neuron> neuron_list;
};

column_vector get_weight(std::vector<neural_layer>);
class neural_network
{
    public:
        neural_network(std::vector<neural_layer>, loss);
    
        neural_network(neural_network&& n):layer_list(n.layer_list), lossfunc(n.lossfunc){};
        neural_network(neural_network& n);
        neural_network(const neural_network& n);
        neural_network& operator=(const neural_network& n);
        neural_network& operator=(neural_network&& rhs);
    
        variable<double> evaluate(data_type data);
        //void gradient_descent(double epsilon);
        std::size_t size() const {return layer_list.size();}
        std::size_t size_layer(std::size_t i){return layer_list[i].size();}
        
        
    private:
        std::vector<neural_layer> layer_list;
        loss lossfunc;
        
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////neural
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
variable<double> neuron::evaluate(const variable<column_vector>& var)
{
    return activation_function(m_actfunc, variable<double>(linear_combination(m_weight, var)));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////neural layer
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

variable<column_vector> neural_layer::evaluate(const variable<column_vector>& arg)
{
    std::size_t nbr_neurons = neuron_list.size();
    variable<column_vector> res(column_vector(nbr_neurons), matrix(nbr_neurons, arg.derivative().get_column()));
    variable<double> tmp(0., column_vector(arg.derivative().get_column()));
    for(std::size_t i=0; i< nbr_neurons; i++)
    {
        tmp = neuron_list[i].evaluate(arg);
        res.value(i) = tmp.value();
        for(std::size_t j=0; j< arg.derivative().get_column(); j++)
        {
            res.derivative(i, j) = tmp.derivative()(j);
        }
    }
    
    
    return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////neural network
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

neural_network::neural_network(std::vector<neural_layer> layer_list_arg, loss lossfunc_arg): layer_list(layer_list_arg), lossfunc(lossfunc_arg){};

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
neural_network& neural_network::operator=(neural_network&& rhs)
{
    layer_list.resize(n.size());
    for(std::size_t i=0; i<size(); i++)
    {
        layer_list[i] = n.layer_list[i];
    }
    return *this;
}





variable<double> neural_network::evaluate(const variable<column_vector>& data)
{ 
    variable<column_vector> propagated_variable(data);
    for(std::size i=0; i<layer_list.size(); i++)
    {
        propagated_variable=layer_list[i].evaluate(propagated_variable);
    }
    return propagated_variable;
}

variable<double> neural_network::evaluate_loss const(const variable<column_vector>& var)
{ 
    return lossfunc(var);
}

/*
void neural_network::gradient_descent(double epsilon, variable<column_vector> res_evaluate_loss)
{
    for(std::size_t i=0; i<size(); i++)
    {
        for (std::size_t j=0; j<size_layer(i); j++
         {
            for(std::size_t k=0; k<layer_list[i][j].size(); k++)
            {
                layer_list[i][j].weight(k) -= epsilon * res_evaluate_loss.derivative()(find_weight_in_derivative(i, j, k))
            }
         }
    }
}
*/
double neural_network::value()
{
    return final_value.value();
}

double neural_network::derivative()
{
    return final_value.derivative();
}

#endif