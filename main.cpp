#include "activationfunction.hpp"
//#include "loss.hpp"
/*begin by checking that activatefunction are correct*/

int main()
{

    
    //softmax<double> softmax_d = softmax<double>();
    variable<double> x(5, 3);
    variable<double> y(7, 5);
    auto z = softmax(x, y);
    std::cout<<typeid(z).name()<<std::endl;
    std::cout<< z.value()<<std::endl;
    //std::cout<< z.derivative()<<std::endl;
}