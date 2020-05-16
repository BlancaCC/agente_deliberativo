#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue> // std::queue para búsqueda en anchura y std::priority_queue Para el costo uniforme 

// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {
  Action accion = actIDLE;

  // Estoy en el nivel 1
  if (sensores.nivel != 4){
    if( !hayplan) {
  
      actual.fila        = sensores.posF;
      actual.columna     = sensores.posC;
      actual.orientacion = sensores.sentido;

      destino.fila       = sensores.destinoF;
      destino.columna    = sensores.destinoC;

      cout << "Fila: " << actual.fila << endl;
      cout << "Col : " << actual.columna << endl;
      cout << "Ori : " << actual.orientacion << endl;


      hayplan = pathFinding (sensores.nivel, actual, destino, plan);
    }

    if( hayplan and plan.size() > 0) {
      accion = plan.front();
      plan.erase(plan.begin());
    }
    else {

      // aquí entraría si no se ha encontrado un comportamiento o está mal implementado
      cout << "Plan mal implementado o no se ha encontrado " << endl;

    }
       
  }   
      
  else {
    // Estoy en el nivel 2

    // RECALUCULAMOS TODO  
    actual.fila        = sensores.posF;
    actual.columna     = sensores.posC;
    actual.orientacion = sensores.sentido;

    destino.fila       = sensores.destinoF;
    destino.columna    = sensores.destinoC;

    if(girosInicio <4) {

	if(girosInicio == 0){
	  limiteMapaf = mapaResultado.size();
	  limiteMapac = mapaResultado[0].size();
	   actualizaMapa(0,0, sensores);
	   
	}
	
	actualizaMapa(1,1, sensores);
	actualizaMapa(2,4, sensores);
	actualizaMapa(3,9, sensores);
	
      accion = actTURN_R;
      girosInicio++; 
      }
   
    else if(! (hayplan and destinoF_ant == destino.fila and destinoC_ant == destino.columna )) {
      destinoF_ant = destino.fila;
      destinoC_ant = destino.columna; 

      cout << "Fila: " << actual.fila << endl;
      cout << "Col : " << actual.columna << endl;
      cout << "Ori : " << actual.orientacion << endl;

      hayplan = pathFinding_Nivel2 (actual,destino,plan, sensores);
    }
    if( hayplan and plan.size() > 0) {

      //actualizamos mapa con la accion pasada
      switch( ultima_accion ) {
      case actIDLE:
	
	break;
      case actFORWARD:
	actualizaMapa(3,9, sensores);
	
	break;
      default:

	
	actualizaMapa(1,1, sensores);
	actualizaMapa(2,4, sensores);
	actualizaMapa(3,9, sensores);
	break;
      }
      accion = plan.front();
      ultima_accion = accion; 
      plan.erase(plan.begin());

      if (plan.empty()) {	
	hayplan = false;
      }
    }
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
  queue<nodo> cola;		        // Lista de Abiertos

  nodo current;
  current.st = origen;
  current.secuencia.empty();

  cola.push(current);

  while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

    cola.pop();
    generados.insert(current.st);

    // Generar descendiente de girar a la derecha
    nodo hijoTurnR = current;
    hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
    if (generados.find(hijoTurnR.st) == generados.end()){
      hijoTurnR.secuencia.push_back(actTURN_R);
      cola.push(hijoTurnR);

    }

    // Generar descendiente de girar a la izquierda
    nodo hijoTurnL = current;
    hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
    if (generados.find(hijoTurnL.st) == generados.end()){
      hijoTurnL.secuencia.push_back(actTURN_L);
      cola.push(hijoTurnL);
    }

    // Generar descendiente de avanzar
    nodo hijoForward = current;
    if (!HayObstaculoDelante(hijoForward.st)){
      if (generados.find(hijoForward.st) == generados.end()){
	hijoForward.secuencia.push_back(actFORWARD);
	cola.push(hijoForward);
      }
    }

    // Tomo el siguiente valor de la cola
    if (!cola.empty()){
      current = cola.front();
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

  
  int gasto = 1;  // gasto por defecto
  
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
  }

  if (gasto == 10 ) cout << "SE HA COGIDO BIKINI" << endl; 
  // cout << "Para casilla " << tipoCasilla << " se devuelve " << gasto << endl; 
  return gasto;			  
}


// Implementación del algoritmo de búsqueda de costo uniforme

bool ComportamientoJugador::pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan, Sensores sensores) {
  //Borro la lista
  cout << "Calculando plan\n";
  plan.clear();
  set<estado,ComparaEstados> generados;          // Lista de Cerrados
  priority_queue<nodoUniforme> cola_prioridad;	 // Lista de Abiertos

  nodoUniforme current;
  current.n.st = origen;
  current.n.secuencia.empty();
  current.bateriaGastada = 0;


  cola_prioridad.push(current);


 
  while (!cola_prioridad.empty() and (current.n.st.fila!=destino.fila or current.n.st.columna != destino.columna)){

    cola_prioridad.pop();
   
    generados.insert(current.n.st);
    //cout << "--- Se ha seleccionado nodo con gasto " << current.bateriaGastada << "---" <<  endl; 

    int gasto_bateria = gastoBateria( mapaResultado[current.n.st.fila][current.n.st.columna]);

    
    // Generar descendiente de girar a la derecha
    nodoUniforme  hijoTurnR = current;    
    hijoTurnR.n.st.orientacion = (hijoTurnR.n.st.orientacion+1)%4;
    if (generados.find(hijoTurnR.n.st) == generados.end()){
      hijoTurnR.bateriaGastada += gasto_bateria;
      //cout << "Se crea hijo derecho con coste de bateria " << hijoTurnR.bateriaGastada << endl; 
      hijoTurnR.n.secuencia.push_back(actTURN_R);
      cola_prioridad.push(hijoTurnR);

    }

    // Generar descendiente de girar a la izquierda
    nodoUniforme  hijoTurnL = current;
    hijoTurnL.n.st.orientacion = (hijoTurnL.n.st.orientacion+3)%4;
    if (generados.find(hijoTurnL.n.st) == generados.end()){
      hijoTurnL.bateriaGastada += gasto_bateria; 
      // cout << "Se crea hijo izquierdo con coste de bateria " << hijoTurnL.bateriaGastada << endl; 
      hijoTurnL.n.secuencia.push_back(actTURN_L);
      cola_prioridad.push(hijoTurnL);
    }

    // Generar descendiente de avanzar
    nodoUniforme  hijoForward = current;

    if (! HayObstaculoDelante(hijoForward.n.st)){
      if (generados.find(hijoForward.n.st) == generados.end()){
	hijoForward.bateriaGastada += gasto_bateria;
	//	cout << "S añade hijo de avance  con coste de batería  " << hijoForward.bateriaGastada << endl; 
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




// ==================== NIVEL 2 ==========================


int distanciaManhattan( int destinoF, int destinoC, int posF, int posC) {
  return  abs(destinoF - posF) + abs(destinoC - posC); 
}


struct nodoNivel2 {
  nodo n;
  int bateria, distancia; 
  float prioridad;

  void calculaDistancia ( int destinoF, int destinoC){
    distancia = distanciaManhattan( destinoF, destinoC,posF, posC, n.st.fila, n.st.columna) 
  }

  void calculaPrioridad(){
    prioridad = distancia + bateria;
  }
  //tendrá preferencia en la cola el que menos prioridad tenga
  bool operator<(const nodoNivel2 & otroNodo) const {
    return prioridad > otroNodo.prioridad || (prioridad == otroNodo.prioridad && bateria > otroNodo.bateria);
    }
  
};

float calculaPrioridad ( nodoNivel2 & n, int destinoF, int destinoC) {

  int dist = distanciaManhattan(destinoF, destinoC, n.n.st.fila, n.n.st.columna)*2;
  return dist;
  
}

bool ComportamientoJugador::pathFinding_Nivel2(const estado &origen, const estado &destino, list<Action> &plan, Sensores & sensores) {
  //Borro la lista
  cout << "Calculando plan\n";
  plan.clear();
  set<estado,ComparaEstados> generados;          // Lista de Cerrados
  priority_queue<nodoNivel2> cola_prioridad;	 // Lista de Abiertos

  nodoNivel2 current;
  current.n.st = origen;
  current.n.secuencia.empty();
  current.prioridad = 0;

  vector<Action> opcionesPosibles = {actFORWARD, actTURN_R, actTURN_L, actIDLE};

  cola_prioridad.push(current);


  int cnt = 0;
  
  while (!cola_prioridad.empty() and (current.n.st.fila!=destino.fila or current.n.st.columna != destino.columna or cnt > VENTANA)){

    cola_prioridad.pop();
   
    generados.insert(current.n.st);
    /*
    //generamos hijos
    for(auto d : opcionesPosibles)
      generaHijo(current, d, generados, cola_prioridad); 
    */
    // Tomo el siguiente valor de la cola_prioridad
    if (!cola_prioridad.empty()){
      current = cola_prioridad.top();
      
    }
    cnt++; 
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


bool ComportamientoJugador::actualizaMapa ( int bloque , int inicio, Sensores & sensores ){

  bool salida = true;
  
  // nos colocamos en el principio del tablero a actualizar
  int f = actual.fila;
  int c = actual.columna; // columna icial de la que se parte
  int addf = 0,addc = 0;

  
  //en función de la orientación el índice de las casillas a actualizar
  //partiendo de la posición inicial se mueven de manera distinta
  switch(actual.orientacion) {
  case norte:
    f -= bloque;
    c -= bloque;
    addc = 1;

    break;

  case sur:
    f += bloque;
    c += bloque;
    addc = -1;

    break;

  case oeste:
    f += bloque; 
    c -= bloque;
    addf = -1;

    break;

  case este:
    f -= bloque;
    c += bloque;
    addf = 1;

    break;


  }

  
  for( int i=0; i< bloque*2+1; i++) {
    if(f >= 0 and c >=0 and f < limiteMapaf and c < limiteMapac) {
      mapaResultado[f][c] = sensores.terreno[inicio++];
     
    }   
    else {
      salida = false;
     
    }
    cout << "  f= " << f <<  "  c = " << c << endl ; 
    f+=addf;
    c+=addc;
  }
  return salida; 
  
}


bool generaHijo(nodoNivel2 nodo, Action direccion,  set<estado,ComparaEstados> & generados, priority_queue<nodoNivel2>& cola_prioridad;) {
  
  if( direccion == actFORWARD && HayObstaculoDelante(nodo.n.st)) {
      
      return false; // no se puede crear el hijo
    }
  
  int add_orientacion = 0; 
  switch(direccion){
  case actTURN_R:
      add_orientacion = 1;
      break;
  case actTURN_L:
      add_orientacion = 3;
      break;
  default:
    break;
  }

  
  if(add_orientacion != 0)
    nodo.n.st.orientacion = (nodo.n.st.orientacion + add_orientacion)%4;
  
  if (generados.find(nodo.n.st) == generados.end()){
    nodo.prioridad += distanciaManhattan(destino.fila, destino.columna,
					      nodo.n.st.fila, nodo.n.st.columna); 
    // cout << "Se crea hijo izquierdo con coste de bateria " << nodo.bateriaGastada << endl; 
    nodo.n.secuencia.push_back(direccion);
    cola_prioridad.push(nodo);

    return true; //se envía el nodo modoficado
  
}
