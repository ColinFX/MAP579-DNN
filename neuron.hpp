/*A neuron is an expression which is differentiable
+ a list of neurons preceding it with associated weights from which we deduce an expression
First implement it for type double then extend it with temlates
There is also an activation function
*/
#include "variable.hpp"


class neuron
{
    public:
        neuron(std::vector<double> weight, activatefunction actfunc);
        variable<double> update(variable<std::vector<double>> var, std::vector<std::size_t> neuron_in_graph);
    private:
        std::vector<double> m_weight;
        activatefunction m_actfunc;
};



class neural_layer
{
    public:
        neural_network(const std::vector<neuron>&);
        variable<double> update(variable<double>);
        std::size_t n_neurons(){return neuron_list.size();}
    private:
        std::vector<neuron> neuron_list;
};

std::matrix<double> get_weight(std::vector<neural_layer>);
class neural_network
{
    public:
        neural_network(std::vector<neural_layer>, activatefunction, loss);
        variable<double> evaluate(data_type data);
        variable<double> evaluate_loss(data_type data);
        void gradient_descent(double epsilon);
        double value();
        double derivative();
    private:
        std::vector<neural_layer> layer_list;
        std::vector<double> weight_list;
        activatefunction actfunc;
        loss lossfunc;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////neural
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
neuron::neuron(std::vector<variable> weight, activatefunction actfunc):m_weight(weight), m_actfunc(actfunc){};
variable<double> neuron::update(variable<double> var)
{
    return m_actfunc(linear_combination(m_weight, var));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////neural layer
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
neural_layer::neural_layer(const std::vector<neuron>& arg): neuron_list(arg){};

variable<double> neural_layer::update(variable<double> arg)
{
    std::size_t nbr_neurons = neuron_list.size();
    variable<double> res(nbr_neurons);
    for(std::size_t i=0; i< nbr_neurons; i)
    {
        res[i] = neuron_list[i].update(arg);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////neural network
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

neural_network::neural_network(data_type arg, std::vector<neural_layer> layer_list_arg, activatefunction actfunc_arg, loss lossfunc_arg): layer_list(layer_list_arg), actfunc(actfunc_arg), lossfunc(lossfunc_arg), weight_matrix(get_weight(layer_list))
{};

void neural_network::evaluate()
{ 
    variable<double> propagated_variable(data);
    for(std::size i=0; i<layer_list.size(); i++)
    {
        propagated_variable=layer_list[i].update(propagated_variable);
    }
    return lossfunc(propagated_variable);
}

void neural_network::evaluate_loss()
{ 
    return lossfunc(final_value);
}


void neural_network::update(double epsilon)
{
    weight_matrix -= epsilon * (this <- derivative());
}

double neural_network::value()
{
    return final_value.value();
}

double neural_network::derivative()
{
    return final_value.derivative();
}