#include "neuron.hpp"

//#include "test.cpp"
int main()
{
    neural_network n(5, {3, 4, 1}, loss_class::MSE, 
        {
            {activation_function_class::ReLu, activation_function_class::ReLu, activation_function_class::ReLu},
            {activation_function_class::ReLu, activation_function_class::sigmoid, activation_function_class::ReLu, activation_function_class::ReLu},
            {activation_function_class::sigmoid}
        }
    );
    
    std::vector<column_vector> data(6);
    std::vector<variable<column_vector>> extrapolate(6);
    std::vector<double> objective(6);
    for(std::size_t i=0; i<6;i++)
    {
        data[i] = column_vector({double(i), double(i+1), double(i+2), double(i+3), double(i+4)});
        
        extrapolate[i] = n.evaluate(data[i]);
        std::cout<<"value of extrapolate["<<i<<"]: "<<std::endl; 
        print(extrapolate[i]);

        objective[i]  = i-3>0?i-3:3-i;
    }
    
    
    variable<double> res_evaluate_loss = n.evaluate_loss(objective, extrapolate);
    print(res_evaluate_loss.value());
    
    
}