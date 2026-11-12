#ifndef SOCIEDAD_H
#define SOCIEDAD_H

/**
 * Estructura global del juego.
 * Contiene el estado general de la sociedad.
 */
struct juego{
    int personas_fuera;
    int dia_actual;
    int turnos_disponibles;
    int record_dias;
    int transferencias_bloqueadas;
    int consumo_pausado;
    float pobreza_global;
};

extern struct juego estado;

/**
 * nodoAVL: Nodo del arbol AVL del catalogo de libros.
 * Guarda el nombre del archivo, la altura del subarbol que cuelga de el
 * y los dos hijos. Se usa para buscar libros por nombre exacto.
 */
struct nodoAVL{
    char nombre[50];
    int altura;
    struct nodoAVL* izquierda;
    struct nodoAVL* derecha;
};

// Arbol AVL con los nombres de todos los libros del corpus
extern struct nodoAVL* catalogoLibros;

/**
 * nodoSplay: Nodo del arbol chaflaneado que hace de cache en la biblioteca
 * de cada comuna.
 * Guarda el texto que se busco, hasta 50 nombres de archivos que lo
 * contienen y los dos hijos.
 * Es una cache: si el texto ya se busco antes no se vuelven a recorrer los
 * archivos, solo se trae el nodo hasta la raiz con las rotaciones.
 */
struct nodoSplay{
    char texto[50];
    int cantidadArchivos;
    char archivos[50][50];
    struct nodoSplay* izquierda;
    struct nodoSplay* derecha;
};

//Moldes

/**
 * Estructura que representa un bien en la sociedad.
 */
struct bien {
    char nombre[50];
    int maximaCantidad;
    int actualCantidad;
    struct bien* siguiente;
};

/**
 * Estructura que representa un oficio que una persona sabe.
 * Es conocimiento, no un recurso, por eso solo guarda el nombre y el siguiente.
 */
struct servicio {
    char nombre[50];
    struct servicio* siguiente;
};

/**
 * libroLeido: Un libro que la persona ya leyo en la biblioteca.
 * Guarda el nombre del archivo y el siguiente libro leido.
 * La lista existe para no volver a contar un libro dos veces.
 */
struct libroLeido {
    char nombre[50];
    struct libroLeido* siguiente;
};

/**
 * Estructura que representa una persona en la sociedad.
 * Lleva su propia lista de servicios (los oficios que sabe).
 * Si la lista es NULL significa que no sabe hacer nada.
 * El campo librosLeidos cuenta cuantos libros ha leido en la biblioteca
 * y la lista libros guarda cuales, para no repetir ninguno.
 * El campo aprendioLeyendo vale 1 cuando ya aprendio un servicio por
 * haber leido 3 libros o mas, y 0 si todavia no le tocaba.
 */
struct persona {
    char nombre[50];
    struct servicio* servicios;
    int librosLeidos;
    int aprendioLeyendo;
    struct libroLeido* libros;
    struct persona* siguiente;
};

/**
 * Estructura que representa una comuna en la sociedad.
 * Tiene sus bienes, sus habitantes y los indices de la sociedad.
 * El indice de reciprocidad mide el balance entre regalos dados y recibidos.
 * La necesidad local es la que sale solo de sus bienes, sin mirar a las vecinas.
 * La necesidad final es la que se compara contra el limite de disolucion.
 * Los campos regalosDados y regalosRecibidos acumulan lo que ha regalado
 * y lo que ha recibido de sus comunas vecinas.
 * El campo administrada marca las 3 comunas que maneja el jugador.
 * Si administrada es 1 la comuna es del jugador, si es 0 es autonoma.
 */
struct comuna {
    char nombre[50];
    struct bien* bienes;
    struct persona* habitantes;
    float necesidad;
    float necesidadLocal;
    float satisfaccion;
    float pobreza;
    float reciprocidad;
    int regalosDados;
    int regalosRecibidos;
    int administrada;
    struct nodoSplay* cache;
    struct comuna* siguiente;
    struct comuna* anterior;
};

// Funciones
/**
 * Crea una nueva comuna con el nombre especificado.
 */
struct comuna* crearComuna(char* nombre);

/**
 * Libera la memoria de una comuna.
 */
void liberarComuna(struct comuna* comuna);

/**
 * liberarListaComunas: Libera toda la lista de comunas.
 */
void liberarListaComunas(struct comuna* lista);

/**
 * contarPersonas: Cuenta cuantas personas hay en una comuna.
 * Recorre la lista de habitantes y retorna el conteo total.
 */
int contarPersonas(struct comuna* comuna);

/**
 * contarBienes: Cuenta cuantos bienes hay en una comuna.
 * Recorre la lista de bienes y retorna el conteo total.
 */
int contarBienes(struct comuna* comuna);

/**
 * contarServicios: Cuenta cuantos oficios tienen las personas de una comuna.
 * Recorre los habitantes y suma los servicios que sabe cada uno.
 */
int contarServicios(struct comuna* comuna);

/**
 * mostrarBienes: Muestra en pantalla los bienes de una comuna.
 * Recorre la lista de bienes e imprime el nombre y las cantidades de cada uno.
 */
void mostrarBienes(struct comuna* comuna);

/**
 * mostrarServicios: Muestra en pantalla los oficios de cada persona de una comuna.
 * Recorre los habitantes e imprime los servicios que sabe cada persona.
 * Incluye un resumen con el porcentaje de personas que al menos sabe un oficio.
 */
void mostrarServicios(struct comuna* comuna);

/**
 * mostrarPersonas: Muestra en pantalla los habitantes de una comuna.
 * Recorre la lista de habitantes e imprime el nombre de cada persona.
 */
void mostrarPersonas(struct comuna* comuna);

/**
 * agregarPersonas: Agrega varias personas a una comuna desde un archivo.
 * Lee nombres del archivo y los agrega a la lista de habitantes.
 * Retorna el numero de personas agregadas.
 */
int agregarPersonas(struct comuna* comuna, char* nombreArchivo);

/**
 * agregarBienes: Agrega varios bienes a una comuna desde un archivo.
 * Lee nombres del archivo y los agrega a la lista de bienes.
 * Retorna el numero de bienes agregados.
 */
int agregarBienes(struct comuna* comuna, char* nombreArchivo);

/**
 * agregarServicioAPersona: Agrega un oficio a la lista de servicios de una persona.
 * Inserta el servicio al inicio de la lista enlazada de la persona.
 */
void agregarServicioAPersona(struct persona* persona, char* nombre);

/**
 * agregarLibroLeido: Anota un libro en la lista de libros leidos de la
 * persona. Inserta el libro al inicio de la lista y sube su contador.
 */
void agregarLibroLeido(struct persona* persona, char* nombre);

/**
 * personaYaLeyo: Revisa si la persona ya leyo ese libro antes.
 * Retorna 1 si ya lo leyo y 0 si todavia no.
 */
int personaYaLeyo(struct persona* persona, char* nombre);

/**
 * liberarLibrosLeidos: Libera la lista de libros leidos de una persona.
 * Se llama cuando se libera la comuna y sus habitantes.
 */
void liberarLibrosLeidos(struct persona* persona);

/**
 * personaTieneServicio: Revisa si la persona ya sabe ese oficio.
 * Retorna 1 si lo sabe y 0 si no.
 */
int personaTieneServicio(struct persona* persona, char* nombre);

/**
 * aprenderServicioPorLectura: La persona aprende un servicio nuevo por
 * haber leido libros en la biblioteca.
 * Mira la carpeta de servicios, junta los oficios que la persona todavia
 * no sabe y elige uno al azar entre esos.
 * Retorna 1 si aprendio algo y 0 si no quedan oficios por aprender.
 */
int aprenderServicioPorLectura(struct persona* persona);

/**
 * migrarPersona: Mueve una persona de una comuna a otra llevandose sus oficios.
 * Es una funcion auxiliar interna para la logica de disolucion de comunas.
 * Retorna 1 si la persona encontro destino, 0 si fallo.
 */
int migrarPersona(struct comuna* origen, struct comuna* destino, char* nombre);

/**
 * regalarAlAzar: Elige al azar un bien de la comuna y se lo regala a una vecina.
 * Es una funcion auxiliar interna para el turno de las comunas autonomas.
 * Retorna 1 si logro regalar, 0 si no pudo.
 */
int regalarAlAzar(struct comuna* origen);

/**
 * crearComunas: Crea varias comunas y las conecta en lista circular doble.
 * Lee nombres del archivo y crea una comuna por cada nombre.
 * Recibe la cantidad de comunas, oficios del catalogo y bienes.
 * Las 500 personas se reparten aleatoriamente entre las comunas.
 * Cada persona tiene un 30% de probabilidad de recibir un oficio al azar.
 * Las primeras 3 comunas quedan marcadas como administradas por el jugador.
 * Retorna la lista de comunas creadas.
 */
struct comuna* crearComunas(char* nombreArchivo, int cantidad, int numServicios, int numBienes);

/**
 * agregarComuna: Agrega una comuna a la lista circular doble.
 * Inserta la comuna al final de la lista y conecta los punteros.
 */
void agregarComuna(struct comuna** lista, char* nombre);

/**
 * contarComunas: Cuenta cuantas comunas hay en la lista.
 * Recorre la lista circular y retorna el conteo total.
 */
int contarComunas(struct comuna* lista);

/**
 * contarAdministradas: Cuenta las comunas que maneja el jugador.
 * Recorre la lista circular y cuenta las que tienen administrada en 1.
 * Si el jugador pierde sus 3 comunas el juego termina.
 */
int contarAdministradas(struct comuna* lista);

/**
 * inicializarSociedad: Inicializa todas las comunas con personas, bienes y servicios.
 * Carga los datos desde los archivos y los asigna a cada comuna.
 */
void inicializarSociedad(struct comuna* lista);

/**
 * calcularNecesidadLocal: Calcula la necesidad que sale solo de los bienes.
 * Recorre los bienes y calcula el promedio de (actual/maxima) de llenado.
 * La necesidad local es 100 menos ese llenado: alto = le falta recursos,
 * bajo = tiene recursos. Esta es la parte local del indice de necesidad.
 */
float calcularNecesidadLocal(struct comuna* comuna);

/**
 * calcularReciprocidad: Calcula el balance entre regalos recibidos y dados.
 * Se calcula como: (recibidos - dados) / (recibidos + dados) * 100.
 * El resultado va de -100 (solo regala) a 100 (solo recibe).
 * Si no ha habido ningun regalo el indice queda en 0 (neutral).
 */
float calcularReciprocidad(struct comuna* comuna);

/**
 * calcularNecesidad: Calcula el indice final de necesidad de una comuna.
 * Junta 5 elementos, cada uno con su peso:
 *   40% necesidad local (sus bienes)
 *   15% necesidad de las comunas circundantes
 *   15% 100 menos la satisfaccion de las comunas circundantes
 *   15% 100 menos el apoyo recibido en regalos
 *   15% presion por el indice de reciprocidad
 * Las vecinas que estan mal no pueden ayudar y suben la necesidad.
 * Recibir regalos cubre carencias y baja la necesidad.
 * Regalar sin recibir deja la comuna mas necesitada.
 * Retorna un valor entre 0 y 100.
 */
float calcularNecesidad(struct comuna* comuna);

/**
 * calcularSatisfaccion: Calcula el indice de satisfaccion de una comuna.
 * Se calcula aparte de la necesidad, solo con lo local:
 * el llenado de bienes (100 - necesidad local) y el porcentaje de personas con oficio.
 * Mas recursos y mas gente con oficio = mas satisfecha.
 * No depende de las comunas vecinas.
 */
float calcularSatisfaccion(struct comuna* comuna);

/**
 * recalcularIndices: Recalcula los indices de todas las comunas.
 * Primero la necesidad local y la reciprocidad de cada comuna.
 * Luego la satisfaccion de cada comuna con su necesidad local.
 * Al final la necesidad final usando tambien a las comunas vecinas.
 * Se hacen en ese orden para no usar valores a medias.
 */
void recalcularIndices(struct comuna* lista);

/**
 * aplicarConsumoNocturno: Reduce los bienes de todas las comunas.
 * Cada persona consume 0.1 bienes por dia (minimo 7, maximo 10).
 */
void aplicarConsumoNocturno(struct comuna* lista);

/**
 * calcularPobrezaGlobal: Calcula la pobreza global del juego.
 * Se calcula como: (personas_fuera / 500) * 100.
 */
float calcularPobrezaGlobal();

/**
 * actualizarPobreza: Actualiza la pobreza global y asigna el valor a todas las comunas.
 */
void actualizarPobreza(struct comuna* lista);

/**
 * bajarBienes: Reduce los bienes de todas las comunas en un porcentaje.
 * Se usa en las emergencias que anuncian perdida de bienes (temblor e
 * inundacion). Aunque el porcentaje redondee a cero, si la comuna tiene
 * bienes pierde al menos una unidad, para que la emergencia siempre se note.
 */
void bajarBienes(struct comuna* lista, int porcentaje);

/**
 * perderOficios: Quita un oficio a una parte de las personas de la sociedad.
 * Se usa en el temblor: la gente perdio sus herramientas y su trabajo.
 * Cada persona tiene un 20% de probabilidad de perder su primer oficio.
 * Las personas que no tienen oficio no se ven afectadas.
 */
void perderOficios(struct comuna* lista);

/**
 * verificarEmergencias: Verifica y aplica emergencias aleatorias.
 * Huelga: probabilidad base 10% + pobreza * 0.3%. Bloquea los intercambios
 *         del dia siguiente, igual que la presidencia.
 * Vandalos: probabilidad base 5% + pobreza * 0.4%. No hace nada, como dice
 *           el mensaje, porque no es dia de revision.
 * Temblor: probabilidad base 3%. Baja los bienes 25% y deja a una parte de
 *          las personas sin oficio.
 * Inundacion: probabilidad base 2%. Baja los bienes 40%.
 * Feriado: probabilidad base 4%. Deja el consumo pausado para la noche
 *          siguiente y bloquea los intercambios del dia siguiente.
 * Presidencia: probabilidad base 3%. Bloquea los intercambios del dia
 *              siguiente.
 */
void verificarEmergencias(struct comuna* lista);

/**
 * realizarTransferencia: Transfiere un bien de una comuna a otra vecina.
 * Verifica que sean vecinas, que haya suficiente cantidad y descuenta del origen.
 * El origen acumula regalos dados y el destino regalos recibidos.
 * Retorna 1 si fue exitosa, 0 si fallo.
 */
int realizarTransferencia(struct comuna* origen, struct comuna* destino, char* bien, int cantidad);

/**
 * disolverComuna: Disuelve una comuna de la sociedad.
 * Parte de sus personas migra a comunas vecinas con sus oficios.
 * La minoria se queda sin hogar y sube la pobreza global.
 * Los bienes de la comuna se pierden. La quita de la lista circular.
 * Si la comuna era del jugador se le asigna otra comuna autonoma en su lugar.
 */
void disolverComuna(struct comuna** lista, struct comuna* comuna);

/**
 * verificarDisoluciones: Verifica si alguna comuna debe disolverse.
 * Se disuelve si la necesidad es muy alta o la satisfaccion es muy baja.
 * Sus personas se reparten entre migracion y pobreza.
 * Si la lista queda vacia se vuelve NULL.
 */
void verificarDisoluciones(struct comuna** lista);

/**
 * calcularTurnosPorDia: Calcula los turnos disponibles por dia.
 * Se calcula como: totalComunas / 10, con minimo 2 y maximo 10.
 */
int calcularTurnosPorDia(int totalComunas);

/**
 * guardarRecord: Guarda el record de dias en un archivo.
 */
void guardarRecord();

/**
 * cargarRecord: Carga el record de dias desde un archivo.
 */
void cargarRecord();

/**
 * aumentarValores: Aumenta los bienes y servicios de una comuna por transferencia.
 * El aumento es proporcional a la cantidad transferida.
 * Bienes: cantidad * 0.1, Servicios: cantidad * 0.15
 */
void aumentarValores(struct comuna* comuna, int cantidad);

/**
 * mostrarResumen: Muestra un resumen de todas las comunas.
 * Muestra nombre, indices y cantidad de personas de cada comuna.
 */
void mostrarResumen(struct comuna* lista);

/**
 * mostrarMenu: Muestra el menu de opciones del jugador.
 */
void mostrarMenu();

/**
 * alturaAVL: Devuelve la altura de un nodo del arbol AVL.
 * Un nodo vacio tiene altura 0.
 */
int alturaAVL(struct nodoAVL* nodo);

/**
 * maximoEntero: Devuelve el mayor de dos numeros enteros.
 */
int maximoEntero(int primero, int segundo);

/**
 * factorBalance: Devuelve la diferencia entre la altura del hijo izquierdo
 * y la del hijo derecho. Si es menor que -1 o mayor que 1 el subarbol
 * esta desbalanceado y hay que rotarlo.
 */
int factorBalance(struct nodoAVL* nodo);

/**
 * rotarDerecha: Rota hacia la derecha un subarbol que quedo mas alto por la
 * izquierda. Es la rotacion simple del AVL.
 * Retorna la nueva raiz del subarbol.
 */
struct nodoAVL* rotarDerecha(struct nodoAVL* nodo);

/**
 * rotarIzquierda: Rota hacia la izquierda un subarbol que quedo mas alto por
 * la derecha. Es la otra rotacion simple del AVL.
 * Retorna la nueva raiz del subarbol.
 */
struct nodoAVL* rotarIzquierda(struct nodoAVL* nodo);

/**
 * insertarAVL: Inserta el nombre de un libro en el arbol AVL y lo deja
 * balanceado con las rotaciones que hagan falta.
 * Si el nombre ya estaba en el arbol no lo vuelve a insertar.
 * Retorna la nueva raiz del arbol.
 */
struct nodoAVL* insertarAVL(struct nodoAVL* raiz, char* nombre);

/**
 * buscarAVL: Busca un libro en el arbol AVL por coincidencia EXACTA del
 * nombre del archivo. Retorna el nodo si lo encuentra y NULL si no.
 */
struct nodoAVL* buscarAVL(struct nodoAVL* raiz, char* nombre);

/**
 * liberarAVL: Libera todos los nodos de un arbol AVL.
 */
void liberarAVL(struct nodoAVL* raiz);

/**
 * cargarCatalogoLibros: Recorre la carpeta del juego y mete en el arbol AVL
 * todos los archivos .txt del corpus, menos el record.txt.
 * Se llama al arrancar el programa, para tener el catalogo en memoria.
 * Retorna la cantidad de libros cargados.
 */
int cargarCatalogoLibros();

/**
 * liberarCatalogoLibros: Libera el arbol AVL del catalogo de libros.
 * Se llama al terminar el juego.
 */
void liberarCatalogoLibros();

/**
 * mostrarMenuBiblioteca: Muestra el menu de la biblioteca comunitaria.
 */
void mostrarMenuBiblioteca();

/**
 * buscarLibroPorNombre: Pide el nombre de un libro al jugador y lo busca
 * en el arbol AVL por coincidencia exacta.
 */
void buscarLibroPorNombre();

/**
 * irALaBiblioteca: Maneja la visita del jugador a la biblioteca de una comuna.
 * Elige la comuna (solo puede usar las suyas) y una persona de esa comuna
 * y despues muestra el menu de la biblioteca.
 * Retorna 1 si la entrada sigue viva y 0 si se acabo la entrada (EOF).
 */
int irALaBiblioteca(struct comuna* lista);

/**
 * pasarMinusculas: Pasa todo el texto a minusculas.
 * Sirve para que "Quijote" y "quijote" se busquen igual y compartan la
 * misma entrada en el arbol chaflaneado.
 */
void pasarMinusculas(char* texto);

/**
 * rotarDerechaSplay: Rota hacia la derecha el subarbol chaflaneado.
 * El hijo izquierdo sube a ser la raiz. Imprime la rotacion para que se
 * vea que el arbol se esta chaflaneando.
 * Retorna la nueva raiz del subarbol.
 */
struct nodoSplay* rotarDerechaSplay(struct nodoSplay* nodo);

/**
 * rotarIzquierdaSplay: Rota hacia la izquierda el subarbol chaflaneado.
 * El hijo derecho sube a ser la raiz. Imprime la rotacion para que se
 * vea que el arbol se esta chaflaneando.
 * Retorna la nueva raiz del subarbol.
 */
struct nodoSplay* rotarIzquierdaSplay(struct nodoSplay* nodo);

/**
 * chaflanear: Sube hasta la raiz el nodo con el texto buscado.
 * Si el texto ya esta en el cache solo se reacomoda el arbol, si no esta
 * el arbol queda como estaba. Usa las rotaciones zig (simple), zig-zig y
 * zig-zag, e imprime cada rotacion que hace.
 * Retorna la nueva raiz del arbol.
 */
struct nodoSplay* chaflanear(struct nodoSplay* raiz, char* texto);

/**
 * guardarEnCache: Agrega el resultado de una busqueda al arbol chaflaneado
 * y lo deja en la raiz para la proxima vez.
 * Retorna la nueva raiz del arbol.
 */
struct nodoSplay* guardarEnCache(struct nodoSplay* raiz, char* texto, char archivos[][50], int cantidad);

/**
 * buscarTextoEnArchivos: Recorre todos los .txt de la carpeta (el corpus)
 * y junta los nombres de los archivos que contienen el texto.
 * La comparacion no distingue mayusculas de minusculas y se hace linea por
 * linea. Retorna la cantidad de archivos encontrados, hasta el maximo.
 */
int buscarTextoEnArchivos(char* texto, char archivos[][50], int maximo);

/**
 * mostrarArchivos: Imprime la lista de nombres de archivos que contienen
 * el texto buscado.
 */
void mostrarArchivos(char archivos[][50], int cantidad);

/**
 * leerLinea: Lee una linea completa desde el teclado, hasta el salto de
 * linea. Sirve para buscar textos con espacios.
 * Descarta el salto de linea que dejo la lectura anterior del menu.
 * Retorna 1 si leyo algo y 0 si se acabo la entrada (EOF).
 */
int leerLinea(char* destino, int maximo);

/**
 * anotarLectura: Anota los libros que acaba de leer la persona.
 * Cada archivo que muestra la busqueda es un libro leido, pero si la
 * persona ya lo habia leido antes no se vuelve a contar.
 * Cuando la persona llega a 3 libros o mas aprende un servicio nuevo,
 * una sola vez.
 * Imprime cuantos libros lleva y si aprendio algo.
 */
void anotarLectura(struct persona* lector, char archivos[][50], int cantidad);

/**
 * buscarTextoEnLibros: Pide un texto al jugador y busca en los libros que
 * lo contienen.
 * Primero mira el arbol chaflaneado de esa biblioteca (cache): si el texto
 * ya se busco antes lo trae hasta la raiz con rotaciones y no vuelve a
 * recorrer los archivos. Si no esta, recorre los archivos, guarda el
 * resultado en el arbol y lo deja en la raiz.
 * Cada archivo que muestra lo anota como leido por la persona que hizo la
 * busqueda, y con 3 libros o mas esa persona aprende un servicio nuevo.
 */
void buscarTextoEnLibros(struct comuna* biblioteca, struct persona* lector);

/**
 * contarSplay: Cuenta cuantas busquedas estan guardadas en el arbol
 * chaflaneado. Retorna la cantidad de nodos.
 */
int contarSplay(struct nodoSplay* raiz);

/**
 * mostrarSplay: Dibuja en pantalla el arbol chaflaneado de una biblioteca.
 * Cada linea es un nodo: la raiz arriba y los hijos sangrados abajo, el
 * izquierdo con una barra / y el derecho con \, para que se vea la forma
 * que el arbol tiene ahora mismo.
 * Se llama con el arbol completo, el nivel 0 y esIzquierdo 1.
 */
void mostrarSplay(struct nodoSplay* raiz, int nivel, int esIzquierdo);

/**
 * verArbolChaflaneado: Muestra en pantalla el arbol chaflaneado de la
 * biblioteca de esa comuna, con la cantidad de busquedas guardadas.
 * Si todavia no se ha buscado nada dice que esta vacio.
 */
void verArbolChaflaneado(struct comuna* biblioteca);

/**
 * liberarSplay: Libera todos los nodos de un arbol chaflaneado.
 */
void liberarSplay(struct nodoSplay* raiz);

/**
 * turnoComunas: Maneja el turno de las comunas que no administra el jugador.
 * Cada comuna autonoma elige una accion al azar del 0 al 4.
 * La accion 1 regala un bien a una comuna vecina.
 */
void turnoComunas(struct comuna* lista);

/**
 * limpiarEntrada: Deja el teclado listo para la siguiente lectura.
 * Se llama cuando scanf no logra leer un numero: descarta todo lo que
 * quede en el buffer hasta el salto de linea.
 * Retorna 1 si quedo algo por leer, 0 si se acabo la entrada (EOF).
 */
int limpiarEntrada();

/**
 * turnoDia: Maneja el turno del dia (jugador realiza transferencias).
 * Valida cada lectura del teclado para no quedarse en un bucle infinito
 * si se escribe algo que no es un numero.
 * Retorna 1 si el dia sigue normal y 0 si se acabo la entrada.
 */
int turnoDia(struct comuna* lista);

/**
 * turnoNoche: Maneja el turno de la noche (consumo, emergencias, disoluciones).
 * Libera el bloqueo de intercambios de la noche anterior y, si hubo feriado,
 * deja el consumo pausado para esa noche.
 */
void turnoNoche(struct comuna** lista);

/**
 * Agrega un nuevo bien a la comuna.
 */
void agregarBien(struct comuna* comuna, char* nombre, int maximaCantidad, int actualCantidad);

/**
 * Agrega una nueva persona a la comuna.
 */
void agregarPersona(struct comuna* comuna, char* nombre);

/**
 * Agrega los índices a la comuna.
 */
void agregarindices(struct comuna* comuna, float necesidad, float satisfaccion, float pobreza);

/**
 * Muestra la información de la comuna.
 */
void mostrarComuna(struct comuna* comuna);
/**
 * Muestra los bienes de la comuna.
 */
void mostrarBienes(struct comuna* comuna);

/**
 * Muestra los servicios de la comuna.
 */
void mostrarServicios(struct comuna* comuna);

/**
 * Muestra los habitantes de la comuna.
 */
void mostrarHabitantes(struct comuna* comuna);

/**
 * Muestra los índices de la comuna.
 */
void mostrarIndices(struct comuna* comuna);
















#endif 
