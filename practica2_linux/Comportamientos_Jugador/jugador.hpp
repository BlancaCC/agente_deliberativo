#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      hayplan=false;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      hayplan=false;

    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    int fil, col, brujula;
    estado actual, destino;
    list<Action> plan;
  bool hayplan = false; //modificamos este valor para implementar la búsqueda

  //añado mis variables
  bool bikini = false;
  bool zapatillas = false; 

    // Métodos privados de la clase
  bool pathFinding(int level, const estado &origen, const estado &destino, list<Action> &plan, Sensores sensores);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);

    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);


  // ------- MÉTODOS DE BÚSQUEDA -------
  // EN ANCHURA
  bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);

  // COSTO UNIFORME 
  bool pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan, Sensores sensores);

  // variables auxiliares
  int gastoBateria( char tipoCasilla);
  bool bikiniEquipado(){return bikini;};
  bool zapatillasEquipadas(){ return zapatillas;}
  
  
};

#endif
