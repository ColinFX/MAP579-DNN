#ifndef MATRIX_HPP
#define MATRIX_HPP
#include<vector>
#include<iostream>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// column_vector
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class column_vector
{
    public:
        column_vector(const std::vector<double>&);
        column_vector(const std::initializer_list<double>&);
        column_vector(std::size_t);
        column_vector():column_vector(0){};
        column_vector(std::size_t, double value);
        column_vector(column_vector&&);
        column_vector(const column_vector&);
        column_vector& operator=(const column_vector&);
        column_vector& operator=(column_vector&& rhs);
    
        double& operator()(std::size_t i);
        double operator()(std::size_t i) const {return m_data[i];}
    
        std::size_t size() const{return n_row;}
        column_vector& operator-();
        column_vector& operator+=(const column_vector&);
        column_vector& operator*=(double);
        column_vector& operator/=(double);
        
        void resize(std::size_t new_n_row){m_data.resize(new_n_row);n_row=new_n_row;}
        void vector_adress(){std::cout<<&m_data<<std::endl;}
    private:
        std::vector<double> m_data;
        std::size_t n_row;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// matrix
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class matrix
{
    public:
        matrix(const std::vector<double>&, std::size_t, std::size_t);
        matrix(const std::initializer_list<std::initializer_list<double>>& double_list);
        
    
        matrix(std::size_t);
        matrix(std::size_t, std::size_t);
        matrix():matrix(0, 0){};
        matrix(std::size_t, std::size_t, double value);
        matrix(matrix&&);
        matrix(matrix&);
        matrix(const matrix&);
        matrix& operator=(const matrix&);
        matrix& operator=(matrix&& rhs);
        
        double& operator()(std::size_t i, std::size_t j);
        double operator()(std::size_t i, std::size_t j) const{return m_data[i*n_column+j];}
        column_vector operator()(std::size_t i) const;
        column_vector* operator()(std::size_t i);
        
        std::size_t get_row() const {return n_row;}
        std::size_t get_column() const {return n_column;}
        
        matrix& operator+=(const matrix&);
        matrix& operator*=(double rhs);
        matrix& operator/=(double rhs);
        matrix& operator-();
        //matrix& operator*=(const matrix&);
        //matrix& operator*=(const column_vector&);
        
    private:
        std::vector<double> m_data;
        std::size_t n_row;
        std::size_t n_column;
};
std::ostream& operator<<(std::ostream& out, const matrix& m);
matrix operator+(const matrix&, const matrix&);
void print(const matrix& m){std::cout<< m<<std::endl;}

std::ostream& operator<<(std::ostream& out, const column_vector& m);
column_vector operator+(const column_vector&, const column_vector&);
void print(const column_vector& m){std::cout<< m<<std::endl;}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// matrix
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
matrix::matrix(const std::vector<double>& data, std::size_t i, std::size_t j):m_data(data), n_row(i), n_column(j){};
matrix::matrix(const std::initializer_list<std::initializer_list<double>>& double_list)
        {
            n_row = double_list.size();
            if (n_row==0){n_column=0; m_data = std::vector<double>(0); return;}
            n_column = double_list.begin()[0].size();
            for(std::size_t i=0; i<n_row; i++)
            {
                for(std::size_t j=0; j<n_row; j++)
                {
                    m_data[i*n_column+n_row] = (double_list.begin()[i]).begin()[j];
                }
            }
        };

matrix::matrix(std::size_t i, std::size_t j):m_data(i*j, 0),n_row(i), n_column(j){};
matrix::matrix(std::size_t i, std::size_t j, double value):m_data(i*j, value),n_row(i), n_column(j){};

double& matrix::operator()(std::size_t i, std::size_t j) 
{
    return m_data[i*n_column+j];
}


matrix::matrix(const matrix& m) :n_row(m.get_row()), n_column(m.get_column())
{
    m_data.resize(n_row*n_column);
    for(std::size_t i=0; i<n_row; i++)
    {
        for(std::size_t j=0; j<n_column; j++)
        {
            m_data[i *n_column+ j] = m(i, j);
        }
    }
}
matrix::matrix(matrix&& m) : n_row(m.get_row()), n_column(m.get_column())
{
    m_data.resize(n_row*n_column);
    for(std::size_t i=0; i<n_row; i++)
    {
        for(std::size_t j=0; j<n_column; j++)
        {
            m_data[i *n_column+ j] = m(i, j);
        }
    }
}


matrix& matrix::operator=(const matrix& m)
{
    n_row = m.get_row();
    n_column = m.get_column();
    for(std::size_t i=0; i<n_row; i++)
    {
        for(std::size_t j=0; j<n_row; j++)
        {
            m_data[i*n_column+j] = m(i, j);
        }
     }
    return *this;
}

matrix& matrix::operator=(matrix&& rhs)
{
    using std::swap;
    swap(m_data, rhs.m_data);
    n_column = rhs.get_column();
    n_row = rhs.get_row();
    return *this;
}


matrix& matrix::operator+=(const matrix& rhs)
{
    for(std::size_t i=0; i<n_row; i++)
    {
        for(std::size_t j=0; j<n_column; j++)
        {
            m_data[n_column*i+j] = m_data[i *n_column+ j] + rhs(i, j);
        }
    }
    return *this;
}

matrix& matrix::operator*=(double d)
{
    for(std::size_t i=0; i<n_row; i++)
    {
        for(std::size_t j=0; j<n_column; j++)
        {
            m_data[n_column*i+j] = d*m_data[i *n_column+ j];
        }
    }
    return *this;
}
matrix& matrix::operator/=(double d)
{
    for(std::size_t i=0; i<n_row; i++)
    {
        for(std::size_t j=0; j<n_column; j++)
        {
            m_data[n_column*i+j] = m_data[i *n_column+ j]/d;
        }
    }
    return *this;
}

matrix operator+(const matrix& lhs, const matrix& rhs) 
{
    matrix res(lhs);
    res+=rhs;
    return res;
}
matrix operator *(const matrix& m, double d)
{
    matrix res(m);
    res*=d;
    return res;
}
matrix operator /(const matrix& m, double d)
{
    matrix res(m);
    res/=d;
    return res;
}

matrix& matrix::operator-()
{
    for(std::size_t i=0; i<m_data.size(); i++)
    {
            m_data[i] = -m_data[i];
    }
    return *this;
}


std::ostream& operator<<(std::ostream& out, const matrix& m)
{
    for(std::size_t i=0; i<m.get_row()-1; i++)
    {
        for(std::size_t j=0; j<m.get_column(); j++)
        {
            out<<m(i, j)<<' ';
        }
        out<<std::endl;
    }
    for(std::size_t j=0; j<m.get_column(); j++)
        {
            out<<m(m.get_row()-1, j)<<' ';
        }
   return out;
}
 
column_vector  matrix::operator()(std::size_t i) const
{
    column_vector res_column(n_column);
    for(std::size_t j=0;  j<n_row; j++)
    {
        res_column(j) = m_data[i*n_column+j];
    }
    return res_column;
}

column_vector*  matrix::operator()(std::size_t i) 
{
    column_vector* res_column;
    res_column = new column_vector(n_column);
    for(std::size_t j=0;  j<n_row; j++)
    {
        (*res_column)(j) = m_data[i*n_column+j];
    }
    return res_column;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// column_vector
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


double& column_vector::operator()(std::size_t i) 
{
    return m_data[i];
}
column_vector::column_vector(const std::initializer_list<double>& v):m_data(v), n_row(v.size()){};

column_vector::column_vector(const std::vector<double>& v):m_data(v), n_row(v.size()){};
column_vector::column_vector(std::size_t i):m_data(i, 0),n_row(i){};
column_vector::column_vector(std::size_t i, double value):m_data(i, value),n_row(i){};



column_vector::column_vector(const column_vector& m) :n_row(m.size())
{
    m_data.resize(n_row);
    for(std::size_t i=0; i<n_row; i++)
    {
        m_data[i] = m(i);
    }
}
column_vector::column_vector(column_vector&& m) : n_row(m.size())
{
    m_data.resize(n_row);
    for(std::size_t i=0; i<n_row; i++)
    {
            m_data[i] = m(i);
    }
}


column_vector& column_vector::operator=(const column_vector& m)
{
    n_row = m.size();
    for(std::size_t i=0; i<n_row; i++)
    {
        m_data[i] = m(i);
     }
    return *this;
}

column_vector& column_vector::operator=(column_vector&& rhs)
{
    using std::swap;
    swap(m_data, rhs.m_data);
    n_row = rhs.size();
    return *this;
}


column_vector& column_vector::operator+=(const column_vector& rhs)
{
    for(std::size_t i=0; i<n_row; i++)
    {
            m_data[i] = m_data[i] + rhs(i);
    }
    return *this;
}

column_vector& column_vector ::operator*=(double d)
{
    for(std::size_t i=0; i<n_row; i++)
    {
        m_data[i] = d*m_data[i];
    }
    return *this;
}

column_vector& column_vector ::operator/=(double d)
{
    for(std::size_t i=0; i<n_row; i++)
    {
        m_data[i] = m_data[i]/d;
    }
    return *this;
}

column_vector operator+(const column_vector& lhs, const column_vector& rhs)
{
    column_vector res(lhs);
    res+=rhs;
    return res;
}
column_vector operator *(const column_vector& m, double d)
{
    column_vector res(m);
    res*=d;
    return res;
}

column_vector operator *(double d, const column_vector& m)
{
    column_vector res(m);
    res*=d;
    return res;
}
column_vector operator /(const column_vector& m, double d)
{
    column_vector res(m);
    res/=d;
    return res;
}

column_vector& column_vector::operator-()
{
    for(std::size_t i=0; i<n_row; i++)
    {
        m_data[i] = -m_data[i];
    }
    return *this;
}

std::ostream& operator<<(std::ostream& out, const column_vector& m)
{
    for(std::size_t i=0; i<m.size()-1; i++)
    {
        out<<m(i);
        out<<std::endl;
    }
    out<< m(m.size()-1);
   return out;
}



#endif