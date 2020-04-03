#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue> // std::priority_queue Para el costo uniforme 


// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {
  Action accion = actIDLE;
  // Estoy en el nivel 1

  actual.fila        = sensores.posF;
  actual.columna     = sensores.posC;
  actual.orientacion = sensores.sentido;

  cout << "Fila: " << actual.fila << endl;
  cout << "Col : " << actual.columna << endl;
  cout << "Ori : " << actual.orientacion << endl;

  destino.fila       = sensores.destinoF;
  destino.columna    = sensores.destinoC;

  if (sensores.nivel != 4){
    bool hay_plan = pathFinding (sensores.nivel, actual, destino, plan, sensores);
  }
  else {
    // Estoy en el nivel 2
    cout << "Aún no implementado el nivel 2" << endl;
  }

  return accion;
}


// Llama al algoritmo de busqueda que se usará en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const estado &destino, list<Action> &plan, Sensores sensores){
  switch (level){
  case 1: cout << "Busqueda en profundad\n";
    return pathFinding_Profundidad(origen,destino,plan);
    break;
  case 2: cout << "Busqueda en Anchura\n";
    // Incluir aqui la llamada al busqueda en anchura
    return  pathFinding_Anchura(origen,destino,plan);
    break;
  case 3: cout << "Busqueda Costo Uniforme\n";
    // Incluir aqui la llamada al busqueda de costo uniforme
    
    return pathFinding_CostoUniforme(origen, destino, plan, sensores);
    break;
  case 4: cout << "Busqueda para el reto\n";
    // Incluir aqui la llamada al algoritmo de búsqueda usado en el nivel 2
    break;
  }
  cout << "Comportamiento sin implementar\n";
  return false;
}


//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el código en carácter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
  if (casilla=='P' or casilla=='M')
    return true;
  else
    return false;
}


// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st){
  int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
  switch (st.orientacion) {
  case 0: fil--; break;
  case 1: col++; break;
  case 2: fil++; break;
  case 3: col--; break;
  }

  // Compruebo que no me salgo fuera del rango del mapa
  if (fil<0 or fil>=mapaResultado.size()) return true;
  if (col<0 or col>=mapaResultado[0].size()) return true;

  // Miro si en esa casilla hay un obstaculo infranqueable
  if (!EsObstaculo(mapaResultado[fil][col])){
    // No hay obstaculo, actualizo el parámetro st poniendo la casilla de delante.
    st.fila = fil;
    st.columna = col;
    return false;
  }
  else{
    return true;
  }
}




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


// Implementación de la búsqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
  //Borro la lista
  cout << "Calculando plan\n";
  plan.clear();
  set<estado,ComparaEstados> generados; // Lista de Cerrados
  stack<nodo> pila;		        // Lista de Abiertos

  nodo current;
  current.st = origen;
  current.secuencia.empty();

  pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

    pila.pop();
    generados.insert(current.st);

    // Generar descendiente de girar a la derecha
    nodo hijoTurnR = current;
    hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
    if (generados.find(hijoTurnR.st) == generados.end()){
      hijoTurnR.secuencia.push_back(actTURN_R);
      pila.push(hijoTurnR);

    }

    // Generar descendiente de girar a la izquierda
    nodo hijoTurnL = current;
    hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
    if (generados.find(hijoTurnL.st) == generados.end()){
      hijoTurnL.secuencia.push_back(actTURN_L);
      pila.push(hijoTurnL);
    }

    // Generar descendiente de avanzar
    nodo hijoForward = current;
    if (!HayObstaculoDelante(hijoForward.st)){
      if (generados.find(hijoForward.st) == generados.end()){
	hijoForward.secuencia.push_back(actFORWARD);
	pila.push(hijoForward);
      }
    }

    // Tomo el siguiente valor de la pila
    if (!pila.empty()){
      current = pila.top();
    }
  }

  cout << "Terminada la busqueda\n";

  if (current.st.fila == destino.fila and current.st.columna == destino.columna){
    cout << "Cargando el plan\n";
    plan = current.secuencia;
    cout << "Longitud del plan: " << plan.size() << endl;
    PintaPlan(plan);
    // ver el plan en el mapa
    VisualizaPlan(origen, plan);
    return true;
  }
  else {
    cout << "No encontrado plan\n";
  }


  return false;
}







// Sacar por la términal la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
  auto it = plan.begin();
  while (it!=plan.end()){
    if (*it == actFORWARD){
      cout << "A ";
    }
    else if (*it == actTURN_R){
      cout << "D ";
    }
    else if (*it == actTURN_L){
      cout << "I ";
    }
    else {
      cout << "- ";
    }
    it++;
  }
  cout << endl;
}



void AnularMatriz(vector<vector<unsigned char> > &m){
  for (int i=0; i<m[0].size(); i++){
    for (int j=0; j<m.size(); j++){
      m[i][j]=0;
    }
  }
}


// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
  estado cst = st;

  auto it = plan.begin();
  while (it!=plan.end()){
    if (*it == actFORWARD){
      switch (cst.orientacion) {
      case 0: cst.fila--; break;
      case 1: cst.columna++; break;
      case 2: cst.fila++; break;
      case 3: cst.columna--; break;
      }
      mapaConPlan[cst.fila][cst.columna]=1;
    }
    else if (*it == actTURN_R){
      cst.orientacion = (cst.orientacion+1)%4;
    }
    else {
      cst.orientacion = (cst.orientacion+3)%4;
    }
    it++;
  }
}



int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}


// MÉTODOS DE BÚSQUEDA DE CAMINOS

// Implementación de la búsqueda en anchura.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan) {
  //Borro la lista
  cout << "Calculando plan\n";
  plan.clear();
  set<estado,ComparaEstados> generados; // Lista de Cerrados
  stack<nodo> pila_nivel,pila_siguiente_generacion;		        // Lista de Abiertos

  nodo current;
  current.st = origen;
  current.secuencia.empty();

  pila_siguiente_generacion.push(current);

  while (!pila_siguiente_generacion.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)) {
    
    pila_nivel = pila_siguiente_generacion; //exploramos un nivel más profundo
    
    pila_siguiente_generacion = stack<nodo>(); // tener cuidado de que esta no borre pila de nivel y que esta orden exista

    // nuestro padre no es pero añadimos a posibles exporaciones sus hijos
    while (!pila_nivel.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

      pila_nivel.pop();
      generados.insert(current.st);

      // Generar descendiente de girar a la derecha
      nodo hijoTurnR = current;
      hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
      if (generados.find(hijoTurnR.st) == generados.end()){
	hijoTurnR.secuencia.push_back(actTURN_R);
	pila_siguiente_generacion.push(hijoTurnR);

      }

      // Generar descendiente de girar a la izquierda
      nodo hijoTurnL = current;
      hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
      if (generados.find(hijoTurnL.st) == generados.end()){
	hijoTurnL.secuencia.push_back(actTURN_L);
	pila_siguiente_generacion.push(hijoTurnL);
      }

      // Generar descendiente de avanzar
      nodo hijoForward = current;
      if (!HayObstaculoDelante(hijoForward.st)){
	if (generados.find(hijoForward.st) == generados.end()){
	  hijoForward.secuencia.push_back(actFORWARD);
	  pila_siguiente_generacion.push(hijoForward);
	}
      }

      // Tomo el siguiente valor de la pila
      if (!pila_nivel.empty()){
	current = pila_nivel.top();
      }
    }
  }

  cout << "Terminada la busqueda\n";

  if (current.st.fila == destino.fila and current.st.columna == destino.columna){
    cout << "Cargando el plan\n";
    plan = current.secuencia;
    cout << "Longitud del plan: " << plan.size() << endl;
    PintaPlan(plan);
    // ver el plan en el mapa
    VisualizaPlan(origen, plan);
    return true;
  }
  else {
    cout << "No encontrado plan\n";
  }


  return false;
}


//____________  BÚSQUED DE COSTO UNIFORME ___________-

struct nodoUniforme {
  nodo n;
  int bateriaGastada;
 
  //tendrá preferencia en la cola el que menos batería lleve gastada
  
  bool operator<(const nodoUniforme & otroNodo) const {
        return bateriaGastada > otroNodo.bateriaGastada;
    }
  
};



// función coste  (cuando mayor sea peor es)

//sensores.terreno[]; 
int  ComportamientoJugador::gastoBateria( char tipoCasilla) {

  int gasto = 1; 
  switch( tipoCasilla ) {
  case 'A':
    if (bikiniEquipado()) gasto = 10;
    else gasto =100;
    break;

  case 'B':
    if (zapatillasEquipadas()) gasto = 50;
    else gasto =5;
    break;
    
  case 'T':
    gasto = 2;
    break; 
    
  
    return gasto;
  }
			  
}


// Implementación del algoritmo de búsqueda de costo uniforme

bool ComportamientoJugador::pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan, Sensores sensores) {
  //Borro la lista
  cout << "Calculando plan\n";
  plan.clear();
  set<estado,ComparaEstados> generados; // Lista de Cerrados
  priority_queue<nodoUniforme> cola_prioridad;		        // Lista de Abiertos

  nodoUniforme current;
  current.n.st = origen;
  current.n.secuencia.empty();
  current.bateriaGastada = 0;


  cola_prioridad.push(current);


 
  while (!cola_prioridad.empty() and (current.n.st.fila!=destino.fila or current.n.st.columna != destino.columna)){

    cola_prioridad.pop();
   
    generados.insert(current.n.st);

    // Generar descendiente de girar a la derecha
    nodoUniforme  hijoTurnR = current;    
    hijoTurnR.n.st.orientacion = (hijoTurnR.n.st.orientacion+1)%4;
    if (generados.find(hijoTurnR.n.st) == generados.end()){
      hijoTurnR.bateriaGastada += gastoBateria( sensores.terreno[0]);
     
      hijoTurnR.n.secuencia.push_back(actTURN_R);
      cola_prioridad.push(hijoTurnR);

    }

    // Generar descendiente de girar a la izquierda
    nodoUniforme  hijoTurnL = current;
    hijoTurnL.n.st.orientacion = (hijoTurnL.n.st.orientacion+3)%4;
    if (generados.find(hijoTurnL.n.st) == generados.end()){
      hijoTurnL.bateriaGastada += gastoBateria( sensores.terreno[0]);
      
      hijoTurnL.n.secuencia.push_back(actTURN_L);
      cola_prioridad.push(hijoTurnL);
    }

    // Generar descendiente de avanzar
    nodoUniforme  hijoForward = current;
    hijoForward.bateriaGastada += gastoBateria( sensores.terreno[0]); //(debemos contabilizarlo antes de que avance)
    if (!HayObstaculoDelante(hijoForward.n.st)){
      if (generados.find(hijoForward.n.st) == generados.end()){
	hijoForward.n.secuencia.push_back(actFORWARD);
	cola_prioridad.push(hijoForward);
      }
    }

    // Tomo el siguiente valor de la cola_prioridad
    if (!cola_prioridad.empty()){
      current = cola_prioridad.top();
      
    }
  }
 
  cout << "Terminada la busqueda\n";

  if (current.n.st.fila == destino.fila and current.n.st.columna == destino.columna){
    cout << "Cargando el plan\n";
    plan = current.n.secuencia;
    cout << "Longitud del plan: " << plan.size() << endl;
    PintaPlan(plan);
    // ver el plan en el mapa
    VisualizaPlan(origen, plan);
    return true;
  }
  else {
    cout << "No encontrado plan\n";
  }


  return false;
}
