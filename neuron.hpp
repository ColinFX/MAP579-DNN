/*A neuron is an expression which is differentiable
+ a list of neurons preceding it with associated weights from which we deduce an expression
First implement it for type double then extend it with temlates
There is also an activation function
*/
#include "variable.hpp"


class neuron
{
    public:
        neuron(std::vector<neuron> neuron_list, std::vector<variable> weight, activatefunction actfunc):m_actfunc(actfunc), m_neuron_list(neuron_list), m_weight(weight), m_expr(linear_combine_neurons(m_weight, m_neuron_list));
    expression linear_combine_neurons()
    {
        
    }
    private:
        std::vector<neuron> m_neuron_list;
        std::vector<variable> m_weight;
        expression<double> m_expr;
        activatefunction m_actfunc;
};