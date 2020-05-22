#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <set>
#include <stack>
#include <queue> // std::queue para búsqueda en anchura y std::priority_queue Para el costo uniforme
#include <vector>

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

      if (plan.empty()) {	
	hayplan = false;
      }
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
	  
    // }
    //cout << "Aún no implementado el nivel 2" << endl;
    else if(! (hayplan and destinoF_ant == destino.fila and destinoC_ant == destino.columna )) {
      destinoF_ant = destino.fila;
      destinoC_ant = destino.columna; 

      cout << "Fila: " << actual.fila << endl;
      cout << "Col : " << actual.columna << endl;
      cout << "Ori : " << actual.orientacion << endl;

      hayplan = pathFinding_Nivel2 (actual,destino,plan, sensores);
    }
    // si lo hay, el plan siempre va a ser mayor que 0 por cómo está diseñado
    if( hayplan  and plan.size() > 0) {

      CONTADOR_PASOS++; 
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

      if(accion == actFORWARD && sensores.superficie[2] =='a') //si se va a chocar con un aldeano se para
	accion = actIDLE;
      else
	plan.erase(plan.begin());
      
      ultima_accion = accion; 


      if (plan.empty()) {	
	hayplan = false;
      }
    }
  }

return accion;
}

// Llama al algoritmo de busqueda que se usará en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const estado &destino, list<Action> &plan){
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
    
    return pathFinding_CostoUniforme(origen, destino, plan);
    break;
  case 4: cout << "Busqueda para el reto\n";
    // Incluir aqui la llamada al algoritmo de búsqueda usado en el nivel 2
    //se llama en el think
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





/** esto me lo he llevado al .hhp por mayor modularidad 
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
*/

// Implementación de la búsqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	stack<nodo> pila;											// Lista de Abiertos

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




// ============================== MI BÚSQUEDA ============================


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
  bool bikini_on, zapatillas_on; 
 
  //tendrá preferencia en la cola el que menos batería lleve gastada
  
  bool operator<(const nodoUniforme & otroNodo) const {
        return bateriaGastada > otroNodo.bateriaGastada;
    }
  
};



// función coste  (cuando mayor sea peor es)

//sensores.terreno[];

int  ComportamientoJugador::gastoBateriaCostoUniforme( char tipoCasilla, bool &bikini_on, bool &zapatillas_on) {

  
  int gasto = 1;  // gasto por defecto
  
  switch( tipoCasilla ) {
    
    //activamos los objetos
  case 'K':
    bikini_on = true;
  break;
  case 'D':
    zapatillas_on = true;
    break;

    //casillas 
  case 'A':
    if (bikini_on) gasto = 10;
    else gasto =100;
    break;

  case 'B':
    if (zapatillas_on) gasto = 5;
    else gasto =50;
    break;
    
  case 'T':
    gasto = 2;
    break;

  default:
    gasto = 1;
    break;

  }

  return gasto;
}

float  ComportamientoJugador::gastoBateriaNV2( char tipoCasilla, bool &bikini_on, bool &zapatillas_on, float bateria_actual) {

  
  int gasto = 1;  // gasto por defecto
  
  switch( tipoCasilla ) {

    //actualizamos si la casilla encontrada ES DE OBEHTO
  case 'K':
    if (!bikini_on) {
      bikini_on = true;
      gasto = -20; //aliciente para que lo coja, porque es mejor tener que no tener
    }
    
    break;
  case 'D':
    if(!zapatillas_on) {
      zapatillas_on = true;
      gasto = -20;
    }
    break;
    

    //recompensa por recargar
  case 'X':
    gasto -= 10;
    break;
    
    //ahora ya caso normal , comento los tiempo reales,los estimados son pa que recorte si va sobrao de batería
  case 'A':
    if (bikini_on) gasto = 5 + 5*(BATERIA_MAX -bateria_actual)/ BATERIA_MAX;//10;
    else gasto =100;
    break;


  case 'B':
    if (zapatillas_on) gasto = 3+2*(BATERIA_MAX -bateria_actual)/ BATERIA_MAX;//5;
    else gasto =50;
    break;
    
  case 'T':
    gasto = 1+(BATERIA_MAX -bateria_actual)/ BATERIA_MAX;//2;
    break;

    //ESTIMACIÓN 
  case '?':
    gasto = 1;
    break;

  default:
    gasto = 1;
    break;

  }

  
  // cout << "Para casilla " << tipoCasilla << " se devuelve " << gasto << endl; 
  return gasto;			  
}


// Implementación del algoritmo de búsqueda de costo uniforme

bool ComportamientoJugador::pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan) {
  //Borro la lista
  cout << "Calculando plan\n";
  plan.clear();
  set<estado,ComparaEstados> generados;          // Lista de Cerrados
  priority_queue<nodoUniforme> cola_prioridad;	 // Lista de Abiertos

  nodoUniforme current;
  current.n.st = origen;
  current.n.secuencia.empty();
  current.bateriaGastada = 0;
  
  current.bikini_on = bikiniEquipado();
  current.zapatillas_on = zapatillasEquipadas(); 


  cola_prioridad.push(current);


 
  while (!cola_prioridad.empty() and (current.n.st.fila!=destino.fila or current.n.st.columna != destino.columna)){

    cola_prioridad.pop();
   
    generados.insert(current.n.st);
    //cout << "--- Se ha seleccionado nodo con gasto " << current.bateriaGastada << "---" <<  endl; 

    int gasto_bateria = gastoBateriaCostoUniforme( mapaResultado[current.n.st.fila][current.n.st.columna], current.bikini_on, current.zapatillas_on);

    
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

bool ComportamientoJugador::pathFinding_Nivel2(const estado &origen, const estado &destino, list<Action> &plan, Sensores & sensores) {
  //Borro la lista
  cout << "Calculando plan\n";
  plan.clear();
  set<estado,ComparaEstados> generados;          // Lista de Cerrados
  priority_queue<nodoNivel2> cola_prioridad;	 // Lista de Abiertos

  //PARAMETROS A AJUSTAR
  RECARGA_HASTA = 3000 - CONTADOR_PASOS/2;
  NIVEL_RECARGA = 200;

  nodoNivel2 current;
  current.n.st = origen;
  current.n.secuencia.empty();
  current.bikini_on = bikiniEquipado();
  current.zapatillas_on = zapatillasEquipadas();
  current.bateria = 0;
  current.bateria_actual = sensores.bateria;
  current.pseudo_destino = true;

  current.tipo_padre = mapaResultado[current.n.st.fila][current.n.st.columna];
  current.mi_tipo = current.tipo_padre; 




  //fijamos un objetivo
  bool modoRecarga = false;
  if(current.bateria_actual < NIVEL_RECARGA && !casillasBateria.empty()) {
    //cout << "MODO VE A POR LA BATERÍA " << endl;
    modoRecarga = true;
    set<punto> baterias;
 
    // metemos todos los puntos 
    for( auto i: casillasBateria) {
      punto p;
      p.f = i[0];
      p.c = i[1];
      p.distancia = current.distanciaManhattanNodo(p.f,p.c);
      baterias.insert(p);
    }

    //sacamos el mínimo
    set<punto>::iterator primero= baterias.begin();
    current.destinoF = primero->f;
    current.destinoC = primero ->c;
  }
  else {
    current.destinoF = destino.fila;
    current.destinoC = destino.columna;
  }


  //para que no piense demasiado

  current.calculaDistancia();
  if(current.distancia > 79) { //reducimos a punto medio de la distancia y punto inicial
    current.destinoF = (current.destinoF + current.n.st.fila)/2;
    current.destinoC = (current.destinoC + current.n.st.columna)/2;
    modoRecarga = false;
    current.pseudo_destino = false; 
  }
  else if(current.distancia > 35) {
    current.destinoF = (3*current.destinoF + current.n.st.fila)/4;
    current.destinoC = (3*current.destinoC + current.n.st.columna)/4;
    modoRecarga = false;
    current.pseudo_destino = false; 
  }

  current.calculaPrioridad();

  vector<Action> opcionesPosibles = {actFORWARD, actTURN_R, actTURN_L};// actIDLE};

  cola_prioridad.push(current);

  
  while (!cola_prioridad.empty() and (!current.enDestino())){

    cola_prioridad.pop();
   
    generados.insert(current.n.st);

    //generamos hijos
    for(auto d : opcionesPosibles)
      generaHijo(current, d, generados, cola_prioridad);

    
    
    // Tomo el siguiente valor de la cola_prioridad
    if (!cola_prioridad.empty()){
      current = cola_prioridad.top();

	
    }
  }
 cout << "Terminada la busqueda\n";

 if (current.enDestino()){
    cout << "Cargando el plan\n";
    if(modoRecarga == true and current.pseudo_destino) {
      cout << "__ VOY A RECARGAR PORQUE ESTO EN MODO RECARGA " << endl; 
      while(false){//(current.bateria_actual < RECARGA_HASTA - sensores.tiempo) {
	    current.n.secuencia.push_back(actIDLE);
	    current.bateria_actual += 10;
	    //cout << "Me he quedado recargando" << current.bateria_actual<< endl;
	  }
      current.n.secuencia.push_back(actIDLE);
    }

    //si se han cogido las zapatillas se marca como tal 
      bikini = current.bikini_on;
      zapatillas =current.zapatillas_on;
    
    plan = current.n.secuencia;

   
    // cout << "La distancia era " << current.distanciaManhattan(destino.fila, destino.columna, sensores.posF, sensores.posC) << endl; 
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
      //almacenamos si la bateria está aquí 
      if( mapaResultado[f][c] == 'X'){
	casillasBateria.insert( {f,c});
      }
    }   
    else {
      salida = false;
     
    }
    //cout << "  f= " << f <<  "  c = " << c << endl ; 
    f+=addf;
    c+=addc;
  }
  return salida; 
  
}


bool ComportamientoJugador::generaHijo(nodoNivel2 nodo, Action direccion,  set<estado,ComparaEstados> & generados, priority_queue<nodoNivel2>& cola_prioridad) {

  
  //  if( tipo_casilla == '?')
  //return false;
 
  if( direccion == actFORWARD && HayObstaculoDelante(nodo.n.st)) {
      
      return false; // no se puede crear el hijo
    }

  
  char tipo_casilla = mapaResultado[nodo.n.st.fila][nodo.n.st.columna] ;
  int add_orientacion = 0; 
  switch(direccion){
  case actTURN_R:
      add_orientacion = 1;
      break;
  case actTURN_L:
      add_orientacion = 3;
      break;
  }

  
  if(add_orientacion != 0)
    nodo.n.st.orientacion = (nodo.n.st.orientacion + add_orientacion)%4;


  if (generados.find(nodo.n.st) == generados.end()){

    if(tipo_casilla != '?') {
      
	if (nodo.mi_tipo == 'X') {

	  //ajuste bateria
	  cout << "VOY A RECARGAR INTENSAMENTE " << endl; 
	  while(nodo.bateria_actual < RECARGA_HASTA ) { //máximo de batería 3000, recarga de 5 en 5
	    nodo.bateria -= 4; // ESTIMACIÓN BONIFICACIÓN
	    nodo.n.secuencia.push_back(actIDLE);
	    nodo.bateria_actual += 10;
	    
	  }

	  //actualizamos tipo
	  nodo.tipo_padre = nodo.mi_tipo;
	  nodo.mi_tipo = tipo_casilla;
      
	}

	//añadir que co
	nodo.n.secuencia.push_back(direccion);
    }
    else {
      //estimación de la distancia que le qued
      //cout << "Padre: " << nodo.tipo_padre << " mi tipo " << nodo.mi_tipo << " el siguiente tipo casilla "
      //   <<  tipo_casilla << endl; 
      nodo.bateria += nodo.distancia* gastoBateriaNV2(nodo.tipo_padre, nodo.bikini_on, nodo.zapatillas_on, nodo.bateria_actual);
      
      nodo.n.st.fila = nodo.destinoF;
      nodo.n.st.columna = nodo.destinoC;
      nodo.pseudo_destino = false;

      
    }

    

    //fianlmente como paso común añadimos el nodo a la cola de analizarse
    nodo.actualizaInfo( gastoBateriaNV2(nodo.mi_tipo, nodo.bikini_on, nodo.zapatillas_on, nodo.bateria_actual));
    
    cola_prioridad.push(nodo);
      
    return true; //se envía el nodo modoficado
  }
  return false;
}

