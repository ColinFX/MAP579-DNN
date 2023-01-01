#include<vector>
#include<iostream>

class uvector
{
    public:
        uvector(const std::vector<double>& x):m_data(x){};
        double& operator[](std::size_t i){return m_data[i];}
    private:
        std::vector<double> m_data;

};

double a=5;
enum class suit {a};