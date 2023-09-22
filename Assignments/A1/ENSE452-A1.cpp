#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "ENSE452-A1.h"

using namespace std;

ComplexCalculator::ComplexCalculator() {
    parameterOne = Complex();
    parameterTwo = Complex();
    result = Complex();
    operation = "";
    errorCode = "";
}
Complex::Complex() {
    real = 0.0;
    imaginary = 0.0;
}
Complex :: Complex(string a, string b) {
        try {
            real = stod(a);
            imaginary = stod(b);
        }
        catch (const invalid_argument& e) {

            real = 0.0;
            imaginary = 0.0;
        }
}

double Complex :: getImaginary() const {
    return imaginary;
}

double Complex::getReal() const {
    return real;
}

void Complex::setImaginary(double i) {
    imaginary = i;
}

void Complex::setReal(double r) {
    real = r;
}
    

void ComplexCalculator::startup() {
    cerr << "Complex calculator\n\n\tType a letter to specify the arithmetic operator (A, S, M, D)\n\tfollowed by two complex numbers expressed as pairs of doubles.\n\tType Q to quit.\n";
    runCalculation();
    return;
}

string ComplexCalculator :: trim(string s) {
    auto start = s.begin();
    auto end = s.end();

    // trim leading whitespaces
    while (start != end && isspace(*start)) {
        start++;
    }

    // trim trailing whitespaces
    while (start != end && isspace(*(end - 1))) {
        end--;
    }

    return string(start, end);
}

vector<string> ComplexCalculator :: split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);

    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

void ComplexCalculator :: errorCheck(string input) {
    errorCode = "0";
    input = trim(input);
    if (input == "") {
        errorCode = 1;
        errorMessage = "illegal command";
        return;
    }
    vector<string> parts = split(input, ' ');

    for (long long unsigned int i = 0; i < parts.size(); i++) {
        parts[i] = trim(parts[i]);
        cerr << parts[i] << "\n";
    }
    
    if (parts[0] == "q" || parts[0] == "Q") {
        errorCode = "6";
        return;
    }

    if (parts.size() < 5) {
        errorCode = "2";
        errorMessage = "missing arguments";
        return;
    }
    else if (parts.size() > 5) {
        errorCode = "3";
        errorMessage = "extra arguments";
        return;
    }

    // Check the operation type
    if (parts[0].length() != 1 || string("asmdASMD").find(parts[0][0]) == string::npos) {
        errorCode = "1";
        errorMessage = "illegal command";
        return;
    }

    // Check if next 4 parts are numbers
    for (int i = 1; i <= 4; i++) {
        try {
            stod(parts[i]);
        }
        catch (const invalid_argument& a) {
            errorCode = "5";
            errorMessage = "illegal arguments";
            return;
        }
    }
    operation = parts[0];
    parameterOne = Complex(parts[1], parts[2]);
    parameterTwo = Complex(parts[3], parts[4]);
    return;
}

void ComplexCalculator:: runCalculation() {
    string input;
    while (true) {
        getline(cin, input);  // Use getline to get the entire line     
        errorCheck(input);
        if (errorCode == "0") {
            if (operation == "A" || operation == "a") {
                result = add(parameterOne, parameterTwo);
            }
            else if (operation == "S" || operation == "s") {
                result = subtract(parameterOne, parameterTwo);
            }
            else if (operation == "M" || operation == "m") {
                result = multiply(parameterOne, parameterTwo);
            }
            else if (operation == "D" || operation == "d") {
                result = divide(parameterOne, parameterTwo);
            }
            else if (errorCode == "6") {
                break;
            }
        }   
        if (errorCode == "0") {
            printResult();
        }
        else {
            if (errorCode == "6") {
                break;
            }
            printError();
        }
    }   
}

void ComplexCalculator::printResult() {
    string sign = result.getImaginary() < 0 ? "-" : "+";
    if (sign == "-"){
        result.setImaginary(abs(result.getImaginary()));
    }

    cout << result.getReal() << " " << sign <<" j " << result.getImaginary();
}

void ComplexCalculator :: printError() {
    cout << "error code: " << errorCode << ": " << errorMessage;
}

Complex ComplexCalculator :: add(Complex a, Complex b) {
    Complex result = Complex();
    result.setReal(a.getReal() + b.getReal());
    result.setImaginary(a.getImaginary() + b.getImaginary());
    return result;
}

Complex ComplexCalculator :: subtract(Complex a, Complex b) {
    Complex result = Complex();
    result.setReal(a.getReal() - b.getReal());
    result.setImaginary(a.getImaginary() - b.getImaginary());
    cerr << result.getImaginary() << " " << result.getReal();
    return result;
}
Complex ComplexCalculator :: multiply(Complex a, Complex b) {
    Complex result = Complex();
    result.setReal(a.getReal() * b.getReal() - a.getImaginary() * b.getImaginary());
    result.setImaginary(a.getReal() * b.getImaginary() + a.getImaginary() * b.getReal());
    return result;
}

Complex ComplexCalculator :: divide(Complex a, Complex b) {
    Complex result = Complex();
    double denominator = b.getReal() * b.getReal() + b.getImaginary() * b.getImaginary();
    const double epsilon = 1e-9; // or another suitably small value
    if (std::fabs(denominator) < epsilon) {
        // Handle the case where denominator is considered as zero
        errorCode = "4";
        errorMessage = "divide by zero";
        return result;
    }
    result.setReal((a.getReal() * b.getReal() + a.getImaginary() * b.getImaginary()) / denominator);
    result.setImaginary((a.getImaginary() * b.getReal() - a.getReal() * b.getImaginary()) / denominator);
    return result;
}


int main()
{
    ComplexCalculator calc = ComplexCalculator();
    calc.startup();
}

