#include<iostream>
#include<string>
using namespace std;
class Fraction{
    private:
    int numerator;
    int denominator;
    int gcd(int a , int b)//caculate the greatest common divisor
    {
        if(a % b == 0) return b;
        else return gcd(b , a % b);
    }
    public:
    Fraction()//default constructor 
    {
        this->numerator = 0;
        this->denominator = 1;
    }
    Fraction(int numer , int deno)//construtor takes 2 integers
    {
        this->numerator = numer;
        this->denominator = deno;
    }
    Fraction(const Fraction &f)//copy constructor
    {
        this->numerator = f.numerator;
        this->denominator = f.denominator;
    }
    Fraction(string decimal)//conversion from a finite decimal string
    {
        int i = decimal.length()-1;
        int weight = 1;
        int sum = 0;
        while(i >= 0)
        {
            if(decimal[i] == '.')
            {
                i--;
            }
            else
            {
                sum += weight * (decimal[i] - '0');
                weight *= 10;
                i--;
            }
        }
        weight /= 10;
        this->numerator = sum;
        this->denominator = weight;
        this->Reduction();
    }
    void Print()//print the information
    {
        cout << numerator << "/" << denominator << endl;
        return ;
    }
    void Reduction()//reduct the farction
    {
        int r;
        if(this->numerator == 0) {this-> denominator = 1; return; }//if the fraction is 0, reduct to 0/1
        if(this->denominator > this->numerator) r = gcd(this->denominator , this->numerator);
        else r = gcd(this->numerator , this->denominator);
        this->denominator /= r;
        this->numerator /= r;
        return;
    }
    double Cast_to_Double() const//type cast to double
    {
        double d = (double)numerator / denominator;
        return d; 
    }
    string Cast_to_String()//type cast to string
    {
        string s = to_string(this->numerator) + "/" + to_string(this->denominator);
        return s;
    }
    Fraction operator+(const Fraction &b)//overload add
    {
        Fraction res;
        res.denominator = this->denominator * b.denominator;
        res.numerator = this->numerator * b.denominator + b.numerator * this->denominator;
        res.Reduction();
        return res;
    }
    Fraction operator-(const Fraction &b)//overload sub
    {
        Fraction res;
        res.denominator = this->denominator * b.denominator;
        res.numerator = this->numerator * b.denominator - b.numerator * this->denominator;
        res.Reduction();
        return res;
    }
    Fraction operator*(const Fraction &b)//overload mul
    {
        Fraction res;
        res.denominator = this->denominator * b.denominator;
        res.numerator = this->numerator * b.numerator;
        res.Reduction();
        return res;
    }
    Fraction operator/(const Fraction &b)//overload div
    {
        Fraction res;
        res.denominator = this->denominator * b.numerator;
        res.numerator = this->numerator * b.denominator;
        res.Reduction();
        return res;
    }
    bool operator<(const Fraction &b)//overload <
    {
        return this->Cast_to_Double() < b.Cast_to_Double();
    }
    bool operator<=(const Fraction &b)//overload <=
    {
        return this->Cast_to_Double() <= b.Cast_to_Double();
    }
    bool operator==(const Fraction &b)//overload ==
    {
        return this->Cast_to_Double() == b.Cast_to_Double();
    }
    bool operator!=(const Fraction &b)//overload !=
    {
        return this->Cast_to_Double() != b.Cast_to_Double();
    }
    bool operator>=(const Fraction &b)//overload >=
    {
        return this->Cast_to_Double() >= b.Cast_to_Double();
    }
    bool operator>(const Fraction &b)//overload >
    {
        return this->Cast_to_Double() > b.Cast_to_Double();
    }

};

int main()
{
    Fraction a(2,3);
    a.Print();
    cout << "double : "<< a.Cast_to_Double() << endl;
    Fraction b(1,9);
    Fraction c;
    Fraction x("1.414");
    x.Print();
    cout << "string : " << a.Cast_to_String() << endl;
    c = a + b;
    c.Print();
    c = a - b;
    c.Print();
    c = a * b;
    c.Print();
    c = a / b;
    c.Print();
    cout << "'<'result: " << (a<b) << endl;
    cout << "'<='result: "<< (a<=b) << endl;
    cout << "'=='result: "<< (a==b) << endl;
    cout << "'!='result: "<< (a!=b) << endl;
    cout << "'>='result: "<< (a>=b) << endl;
    cout << "'>'result: "<< (a>b) << endl;
    return 0;

}
