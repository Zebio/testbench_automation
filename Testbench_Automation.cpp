#include <iostream>
#include <fstream>
using namespace std;

struct estimulos {
    //INPUTS
    signed   int a : 4;
    signed   int b : 4;
    unsigned int opr : 1;

    //REGISTERS
    signed   int b2 : 4;

    //OUTPUTS
    signed   int soma : 4;
    unsigned int neg : 1;
    unsigned int of : 1;
    unsigned int zero : 1;
};

class testbench {
private:
    ofstream Arquivo;
    string NomeDoModulo;
    bool ack_in;
    estimulos Estimulos;

public:

    testbench(string nomedomodulo) 
    {
        NomeDoModulo = nomedomodulo;
        Arquivo.open(NomeDoModulo+"_tb.v", ios::out);
        Arquivo << "Iniciando.." << endl;
     }

    void setInputs(int a, int b,int opr)
    {
        Estimulos.a = a;
        Estimulos.b= b;
        Estimulos.opr = opr;
    }
    void printInputs()
    {
        cout << "A: " << Estimulos.a << endl;
        cout << "B: " << Estimulos.b << endl;
        cout << "opr: " << Estimulos.opr << endl;
    }

    void calculate()
    {
        if (Estimulos.opr == 1) //OPR==1(Subtraction); Invert B
        {
            Estimulos.b2 = Estimulos.b * -1;
        }
        else //OPR==0(Sum) ; Maintain B
        {
            Estimulos.b2 = Estimulos.b;
        }
        Estimulos.soma = Estimulos.a + Estimulos.b2;
        Estimulos.neg = (Estimulos.soma<0);
        Estimulos.zero = (Estimulos.soma == 0);

        //OVERFLOW DETECTOR:
        //(A(signal) XOR B(signal)) AND (A(signal) XNOR SOMA(signal))
        Estimulos.of = (Estimulos.a < 0 == Estimulos.b2 < 0) && (Estimulos.a < 0 != Estimulos.soma < 0);
    }

    void printOutputs()
    {
        cout << "Soma: " << Estimulos.soma << ", Zero: " << Estimulos.zero << ", Neg: " << Estimulos.neg << ", OF: " << Estimulos.of << endl;
    }


    string dualRailConverter(unsigned int SingleRailNumber,int size)
    {
        bool DualRailNumber[8];
        string auxiliar="";
        for (int i = size-1; i >= 0; i--)
        {
            DualRailNumber[i + 1] = (SingleRailNumber >> i) / 1;
            DualRailNumber[i] = !DualRailNumber[i + 1];
            auxiliar=auxiliar+DualRailNumber[i + 1]+ DualRailNumber[i];
        }
        return auxiliar;
    }

    void nullStateInputs()
    {
        Arquivo << "\n\n\t//NULL STATE" << endl;
        Arquivo << "\ta = 8'b00000000;" << endl;
        Arquivo << "\tb = 8'b00000000;" << endl;
        Arquivo << "\topr = 2'b00;" << endl;
        Arquivo << "\tack_in = 1;"  << endl;
        Arquivo << "\t  #1;" << endl;

    }

    void inputEstimulos()
    {
        Arquivo << "\n\n\t//Estímulo: a="<<Estimulos.a<<", b="<<Estimulos.b <<", opr="<<Estimulos.opr << endl;
        Arquivo << "\t//Saída desejada: Soma=" << Estimulos.soma << ", Neg=" << Estimulos.neg;
        Arquivo << ", Zero=" << Estimulos.zero << ", OF=" << Estimulos.of << endl;
        Arquivo << "\ta = 8'b" << dualRailConverter(a, 4) << "; " << endl;
        Arquivo << "\tb = 8'b" << dualRailConverter(b, 4) << "; " << endl;
        Arquivo << "\topr = 2'b" << dualRailConverter(opr, 1) << "; " << endl;
        Arquivo << "\tack_in = 1;" << endl;
        Arquivo << "\t  #1;" << endl;
    }

    ~testbench()
    {
        Arquivo.close();
        cout << "Object being destructed";
    }
};




int main()
{
    testbench Testbench("ALU_Assincrona");

    Testbench.nullStateInputs();

    Testbench.setInputs(-7, -7, 0);

    Testbench.calculate();

    Testbench.inputEstimulos();

    Testbench.nullStateInputs();

    return 0;
}
