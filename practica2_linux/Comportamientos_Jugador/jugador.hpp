#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;
};

/* Actualiza el mapa actual, 
bloque es la tanda de sensor terreno a añadir 0  [0], 1[1,3], [4,8],[9,15] {0,1,2,3}
inicio es la casilla primera  a analizar {0,1,4,9}ç

EL O NO LO CALCULA
 */


int distanciaManhattan( int destinoF, int destinoC, int posF, int posC);

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
  bool pathFinding(int level, const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);

    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);


  // ------- MÉTODOS DE BÚSQUEDA -------
  // EN ANCHURA
  bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);

  // COSTO UNIFORME 
  bool pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan);

  // variables auxiliares
  int gastoBateria( char tipoCasilla);
  bool bikiniEquipado(){return bikini;};
  bool zapatillasEquipadas(){ return zapatillas;}

  // ===============  NIVEL 2 ============
  bool pathFinding_Nivel2(const estado &origen, const estado &destino, list<Action> &plan, Sensores & sensores);
  int girosInicio = 0; //si estamos al inicio del juego
  Action ultima_accion = actIDLE; 
  int limiteMapaf, limiteMapac;
  int destinoF_ant, destinoC_ant; 
  bool actualizaMapa( int bloque, int inicio, Sensores & sensores);

  //variables para estimación del predominio terreno
  int cnt_pasos = 0; //contador de pasos dados
  //contadores de tipos de casilla:
  int cnt_s =0;
  int cnt_t =0; //tierra gasto 2
  int cnt_b =0; //bosque gasto 50
  int cnt_a =0; //aguas gasto 100

  /* vector de prioridades, estas son: (cuanto menor sea el número mayor será la prioridad:
     - ahorro_bateria
     - destino final
     - coger bañador
     - coger zapatillas
    
   */
  int pesosPrioridad[]= {1,1,2,2};

  enum tiposPrioridades {BATERIA, DESTINO, BIKINI, ZAPATILLAS};
  void reajustaPrioridadBasadoContadores();
  void reajustaPriorudadBasadoMapa();
  /**
     Intenta generar un hijo si es posible
     true si lo ha generado
   */
  int VENTANA = 5; 

#endif
