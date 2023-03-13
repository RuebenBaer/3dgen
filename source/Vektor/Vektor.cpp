#include "Vektor.h"

Vektor::Vektor(void)
{
  Koordinate = new float[3];
  Koordinate[0]=0;
  Koordinate[1]=0;
  Koordinate[2]=0;
}

Vektor::Vektor(float x, float y, float z)
{
  Koordinate = new float[3];
  Koordinate[0]=x;
  Koordinate[1]=y;
  Koordinate[2]=z;
}

Vektor::Vektor(float* xyz)
{
  Koordinate = new float[3];
  for (int i = 0; i < 3; i++){
    Koordinate[i] = xyz[i];
  }
}

Vektor::Vektor(const Vektor& v)
{
  Koordinate = new float[3];
  for (int i = 0; i < 3; i++){
    Koordinate[i] = v.GetKoordinaten(i);
  }
}

Vektor::~Vektor(void)
{
  delete[]Koordinate;
}

Vektor& Vektor::operator=(const Vektor& v)
{
  for (int i = 0; i < 3; i++){
    Koordinate[i] = v.GetKoordinaten(i);
  }
  return *this;
}

bool Vektor::operator==(const Vektor& v) const
{
    return((Koordinate[0]==v.x())&&(Koordinate[1]==v.y())&&(Koordinate[2]==v.z()));
}

void Vektor::SetKoordinaten(float x, float y, float z)
{
     Koordinate[0]=x;
     Koordinate[1]=y;
     Koordinate[2]=z;
     return;
}

void Vektor::SetKoordinaten(float* xyz)
{
     for(int i=0;i<3;i++)
     {
             Koordinate[i]=xyz[i];
     }
     return;
}

void Vektor::SetKoordinaten(int i, float tF)
{
     Koordinate[i]=tF;
     return;
}

Vektor Vektor::operator+(const Vektor& AddVektor)
{
       Vektor Rueckgabe = *this;
       for(int i = 0; i < 3; i++){
               Rueckgabe.SetKoordinaten(i,(Koordinate[i] + AddVektor.GetKoordinaten(i)));
       }
       return(Rueckgabe);
}

Vektor& Vektor::operator+=(const Vektor& AddVektor)
{
        for(int i = 0; i < 3; i++){
                Koordinate[i] += AddVektor.GetKoordinaten(i);
        }
        return *this;
}

Vektor Vektor::operator*(float Mult)
{
       Vektor Rueckgabe = *this;
       for(int i = 0; i < 3; i++){
               Rueckgabe.SetKoordinaten(i,(Koordinate[i] * Mult));
       }
       return (Rueckgabe);
}

Vektor& Vektor::operator*=(float Mult)
{
        for(int i = 0; i < 3; i++){
                Koordinate[i] *= Mult;
        }
        return *this;
}

Vektor Vektor::operator-(const Vektor& SubVektor)
{
       Vektor Rueckgabe = *this;
       for(int i = 0; i < 3; i++){
               Rueckgabe.SetKoordinaten(i,(Koordinate[i] - SubVektor.GetKoordinaten(i)));
       }
       return(Rueckgabe);
}

Vektor& Vektor::operator-=(const Vektor& SubVektor)
{
        for(int i = 0; i < 3; i++){
                Koordinate[i] -= SubVektor.GetKoordinaten(i);
        }
        return *this;
}

Vektor Vektor::operator/(float Div)
{
       Vektor Rueckgabe = *this;
       Rueckgabe /= Div;
       return(Rueckgabe);
}

#include <iostream>
Vektor& Vektor::operator/=(float Div)
{
        if(Div == 0){
               /*cerr<<"\n"<<"Division durch null!"<<flush;*/
               return *this;
        }
        for(int i = 0; i < 3;i++){
                Koordinate[i] /= Div;
        }
        return *this;
}

float Vektor::operator*(const Vektor& Mult)
{
      return(Koordinate[0]*Mult.GetKoordinaten(0)+Koordinate[1]*Mult.GetKoordinaten(1)+Koordinate[2]*Mult.GetKoordinaten(2));
}

Vektor Vektor::Kreuz(const Vektor& KrVek)
{
       Vektor Rueckgabe;
       Rueckgabe.SetKoordinaten(0,(Koordinate[1] * KrVek.GetKoordinaten(2) - Koordinate[2] * KrVek.GetKoordinaten(1)));
       Rueckgabe.SetKoordinaten(1,(Koordinate[2] * KrVek.GetKoordinaten(0) - Koordinate[0] * KrVek.GetKoordinaten(2)));
       Rueckgabe.SetKoordinaten(2,(Koordinate[0] * KrVek.GetKoordinaten(1) - Koordinate[1] * KrVek.GetKoordinaten(0)));
       return(Rueckgabe);
}

#include <cmath>
float Vektor::Laenge(void)
{
      float Laenge;
      Laenge = sqrt(Koordinate[0]*Koordinate[0]+Koordinate[1]*Koordinate[1]+Koordinate[2]*Koordinate[2]);
      return(Laenge);
}

Vektor Vektor::drehen(double alpha, double beta, double gamma)
{
    Vektor Rueckgabe;
    if(alpha){
        Rueckgabe.SetKoordinaten(0, (cos(alpha)*Koordinate[0]+sin(alpha)*Koordinate[1]));
        Rueckgabe.SetKoordinaten(1, (-sin(alpha)*Koordinate[0]+cos(alpha)*Koordinate[1]));
        Rueckgabe.SetKoordinaten(2, Koordinate[2]);
    }
    if(beta){
        Rueckgabe.SetKoordinaten(0, (cos(beta)*Koordinate[0]-sin(beta)*Koordinate[2]));
        Rueckgabe.SetKoordinaten(1, Koordinate[1]);
        Rueckgabe.SetKoordinaten(2, (sin(beta)*Koordinate[0]+cos(beta)*Koordinate[2]));
    }
    if(gamma){
        Rueckgabe.SetKoordinaten(0, Koordinate[0]);
        Rueckgabe.SetKoordinaten(1, (cos(gamma)*Koordinate[1]+sin(gamma)*Koordinate[2]));
        Rueckgabe.SetKoordinaten(2, (-sin(gamma)*Koordinate[1]+cos(gamma)*Koordinate[2]));
    }
    return Rueckgabe;
}
/*Ende Vektor*/
