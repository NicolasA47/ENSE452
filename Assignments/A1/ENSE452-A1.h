#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class Complex {
public:
    Complex();
    Complex(string a, string b);

    double getImaginary() const;
    double getReal() const;
    void setImaginary(double i);
    void setReal(double r);

private:
    double real;
    double imaginary;
};

class ComplexCalculator {
public:
    ComplexCalculator();
    void startup();

private:
    Complex parameterOne;
    Complex parameterTwo;
    Complex result;
    string operation;
    string errorCode;
    string errorMessage;

    Complex divide(Complex a, Complex b);
    Complex multiply(Complex a, Complex b);
    Complex add(Complex a, Complex b);
    Complex subtract(Complex a, Complex b);

    void errorCheck(string input);
    void runCalculation();
    void printError();
    void printResult();
    string trim(string s);
    vector<string> split(const string& s, char delimiter);
};
