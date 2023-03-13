#ifndef __Vektor_
#define __Vektor_

class Vektor{
  private:
    float* Koordinate;
  public:
    /*Konstruktoren*/
    Vektor();
    Vektor(const Vektor&);
    Vektor(float, float, float);
    Vektor(float*);
    /*Destruktor*/
    ~Vektor();
    /*Koordinaten fuellen*/
    void SetKoordinaten(float, float, float);
    void SetKoordinaten(float*);
    void SetKoordinaten(int, float);/*Index, Wert*/
    /*Koordinaten auslesen*/
    float x(void)const{return(Koordinate[0]);};
    float y(void)const{return(Koordinate[1]);};
    float z(void)const{return(Koordinate[2]);};
    float GetKoordinaten(int i)const{return(Koordinate[i]);} ;
    /*Vektorrechnungen*/
    Vektor operator+(const Vektor&);
    Vektor& operator+=(const Vektor&);
    Vektor operator-(const Vektor&);
    Vektor& operator-=(const Vektor&);
    Vektor operator*(float);
    Vektor& operator*=(float);
    Vektor operator/(float);
    Vektor& operator/=(float);
    float operator*(const Vektor&);
    float operator*=(const Vektor&);
    Vektor& operator=(const Vektor&);
    bool operator==(const Vektor&) const;
    Vektor Kreuz(const Vektor&);
    float Laenge(void);
    Vektor drehen(double alpha, double beta=0, double gamma=0);
  };

#endif
