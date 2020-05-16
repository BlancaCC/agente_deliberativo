#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <cmath>
#include <list>
#include <stack>
#include <queue> // std::queue para búsqueda en anchura y std::priority_queue Para el costo uniforme
#include <vector>
#include<set>

struct estado {
  int fila;
  int columna;
  int orientacion;
};

struct nodo{
	estado st;
	list<Action> secuencia;
};

struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};
// mis estructura



struct nodoNivel2 {
  nodo n;
  int bateria, distancia; 
  float prioridad;
  int destinoF,destinoC;
  bool bikini_on, zapatillas_on; 

 
  
  int distanciaManhattan( int destinoF, int destinoC, int posF, int posC) {
    return  abs(destinoF - posF) + abs(destinoC - posC); 
  }
  

  int distanciaManhattanNodo(int destinoF, int destinoC){
    return distanciaManhattan( destinoF, destinoC, n.st.fila, n.st.columna);
  }
  
  void actualizaInfo(int gasto) {
    calculaDistancia();
    actualizaBateria(gasto);
    calculaPrioridad();
  }

  void calculaDistancia (){
    distancia =  distanciaManhattanNodo( destinoF, destinoC);//, n.st.fila, n.st.columna);
  }

  void calculaPrioridad(){
    prioridad = distancia;
  }

  void actualizaBateria(int gasto) {
    bateria +=  gasto;
  }
  //tendrá preferencia en la cola el que menos prioridad tenga
  bool operator<(const nodoNivel2 & otroNodo) const {
    return prioridad > otroNodo.prioridad || (prioridad == otroNodo.prioridad && bateria > otroNodo.bateria);
    }
  
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
    bool hayplan;

  //AÑADO MIS VARIABLES 
  bool bikini = false;
  bool zapatillas = false; 


    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);

    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);


  
  // ------- MÉTODOS DE BÚSQUEDA -------
  // _________ EN ANCHURA________
  bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);

  // COSTO UNIFORME 
  bool pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan);

  // variables auxiliares
  int gastoBateria( char tipoCasilla, bool & bikini_on, bool & zapatillas_on);
  bool bikiniEquipado(){return bikini;};
  bool zapatillasEquipadas(){ return zapatillas;}

  
  // ===============  NIVEL 2 ============
  bool pathFinding_Nivel2(const estado &origen, const estado &destino, list<Action> &plan, Sensores & sensores);
  int girosInicio = 0; //si estamos al inicio del juego
  Action ultima_accion = actIDLE; 
  int limiteMapaf, limiteMapac;
  int destinoF_ant, destinoC_ant; 
  bool actualizaMapa( int bloque, int inicio, Sensores & sensores);


  /**
     Intenta generar un hijo si es posible
     true si lo ha generado
   */
  bool generaHijo(nodoNivel2 nodo, Action direccion,  set<estado,ComparaEstados> & generados, priority_queue<nodoNivel2>& cola_prioridad);

  // almacenamos puntos interesantes
  set<vector<int>>casillasBateria; // {fila, columna}

  struct punto {
    int f,c;
    int distancia;
    
    bool operator<(const punto & otro) const {
      return distancia > otro.distancia || (distancia > otro.distancia && (f != otro.f || c != otro.c));
    }
    
    
  }; 
  
  // ---- variables para evitar el bloqueo
  int psudo_destino_activado = false; 
  int pseudo_destinoC;
  int pseudo_destinoF;


};

#endif
