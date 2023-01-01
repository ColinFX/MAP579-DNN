#include "neuron.hpp"

//#include "test.cpp"
int main()
{
    
    column_vector x({1, 2, 3});
    matrix m({1, 2, 3, 4, 5, 6}, 3, 2);
    column_vector weight({1, 2, 3});
    variable<column_vector> var(x, m);
    variable<double> res(linear_combination(weight, var));
    
    var.value(1) = 32;
    var.derivative(1, 1) = 56;
    
    std::vector<neuron> n_vec(4);
    
    
    for(std::size_t i=0; i<4; i++)
    {
        n_vec[i] = neuron(weight, activation_function_class::sigmoid);
    }
    
    neural_layer nlayer(n_vec);
    variable<column_vector> var2 = nlayer.update(var);
    print(var2);
}