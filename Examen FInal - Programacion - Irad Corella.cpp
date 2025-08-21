
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdio>   // sscanf
#include <cctype>   // tolower
#include <stdexcept>

using namespace std;

static string a_minusculas(const string& s) {
    string r = s;
    for (size_t i = 0; i < r.size(); ++i) r[i] = (char)tolower((unsigned char)r[i]);
    return r;
}

static bool parsear_fecha(const string& s, int& y, int& m, int& d) {
    return std::sscanf(s.c_str(), "%d-%d-%d", &y, &m, &d) == 3;
}

static string fecha_hoy() {
    time_t t = time(NULL);
    struct tm* lt = localtime(&t);
    char buf[16];
    if (lt) { strftime(buf, sizeof(buf), "%Y-%m-%d", lt); return string(buf); }
    return string("1970-01-01");
}

static int calcular_edad(int y, int m, int d) {
    time_t t = time(NULL);
    struct tm* lt = localtime(&t);
    if (!lt) return 0;
    int anio = lt->tm_year + 1900;
    int mes  = lt->tm_mon + 1;
    int dia  = lt->tm_mday;
    int edad = anio - y;
    if (mes < m || (mes == m && dia < d)) edad--;
    return edad;
}


enum Categoria {
    CATEGORIA_ADMINISTRADOR = 0,
    CATEGORIA_OPERARIO      = 1,
    CATEGORIA_PEON          = 2
};

static string categoria_a_texto(Categoria c) {
    switch (c) {
        case CATEGORIA_ADMINISTRADOR: return "Administrador";
        case CATEGORIA_OPERARIO:      return "Operario";
        case CATEGORIA_PEON:          return "Peon";
        default:                      return "Desconocida";
    }
}

static bool texto_a_categoria(const string& texto, Categoria& out) {
    string l = a_minusculas(texto);
    if (l == "administrador") { out = CATEGORIA_ADMINISTRADOR; return true; }
    if (l == "operario")      { out = CATEGORIA_OPERARIO;      return true; }
    if (l == "peon")          { out = CATEGORIA_PEON;          return true; }
    return false;
}


class Empleado {
private:
    string numero_carnet_;
    string nombre_;
    string fecha_nacimiento_; // "YYYY-MM-DD"
    Categoria categoria_;
    double salario_;
    string direccion_;
    string telefono_;
    string correo_;

    // simulacion de unicidad de correo
    static vector<string> correos_usados_; // en minusculas

    static bool correo_existe(const string& low) {
        for (size_t i = 0; i < correos_usados_.size(); ++i)
            if (correos_usados_[i] == low) return true;
        return false;
    }
    static void registrar_correo(const string& low) { correos_usados_.push_back(low); }
    static void eliminar_correo(const string& low) {
        for (size_t i = 0; i < correos_usados_.size(); ++i) {
            if (correos_usados_[i] == low) {
                correos_usados_.erase(correos_usados_.begin() + (int)i);
                return;
            }
        }
    }

    void validar_y_setear_fecha_nacimiento(const string& f) {
        int y, m, d;
        if (!parsear_fecha(f, y, m, d))
            throw runtime_error("Fecha de nacimiento invalida (use YYYY-MM-DD).");
        if (calcular_edad(y, m, d) < 18)
            throw runtime_error("No se pueden contratar menores de edad.");
        fecha_nacimiento_ = f;
    }
    void validar_y_setear_categoria(const string& cat_texto) {
        Categoria c;
        if (!texto_a_categoria(cat_texto, c))
            throw runtime_error("Categoria invalida. Use: Administrador, Operario o Peon.");
        categoria_ = c;
    }
    void validar_y_setear_salario(double s) {
        if (s < 250000.0 || s > 500000.0)
            throw runtime_error("El salario debe estar entre 250000 y 500000.");
        salario_ = s;
    }
    void validar_y_setear_correo(const string& c) {
        string low = a_minusculas(c);
        if (low.empty())
            throw runtime_error("El correo no puede estar vacio.");
        if (!correo_.empty()) {
            string anterior_low = a_minusculas(correo_);
            if (anterior_low != low) eliminar_correo(anterior_low);
            else return; // mismo correo
        }
        if (correo_existe(low))
            throw runtime_error("El correo ya esta registrado.");
        registrar_correo(low);
        correo_ = c;
    }

public:
    // constructor sin salario: asigna 250000 por defecto
    Empleado(const string& carnet,
             const string& nombre,
             const string& fecha_nacimiento,
             const string& categoria_texto,
             const string& direccion,
             const string& telefono,
             const string& correo)
        : numero_carnet_(carnet), nombre_(nombre), fecha_nacimiento_(""),
          categoria_(CATEGORIA_OPERARIO), salario_(250000.0),
          direccion_(direccion), telefono_(telefono), correo_("")
    {
        if (direccion_.empty()) direccion_ = "San Jose";
        validar_y_setear_fecha_nacimiento(fecha_nacimiento);
        validar_y_setear_categoria(categoria_texto);
        validar_y_setear_correo(correo);
    }

    // constructor con salario
    Empleado(const string& carnet,
             const string& nombre,
             const string& fecha_nacimiento,
             const string& categoria_texto,
             double salario,
             const string& direccion,
             const string& telefono,
             const string& correo)
        : numero_carnet_(carnet), nombre_(nombre), fecha_nacimiento_(""),
          categoria_(CATEGORIA_OPERARIO), salario_(250000.0),
          direccion_(direccion), telefono_(telefono), correo_("")
    {
        if (direccion_.empty()) direccion_ = "San Jose";
        validar_y_setear_fecha_nacimiento(fecha_nacimiento);
        validar_y_setear_categoria(categoria_texto);
        validar_y_setear_salario(salario);
        validar_y_setear_correo(correo);
    }

    // getters
    const string& getCarnet() const { return numero_carnet_; }
    const string& getNombre() const { return nombre_; }
    const string& getFechaNacimiento() const { return fecha_nacimiento_; }
    Categoria getCategoria() const { return categoria_; }
    double getSalario() const { return salario_; }
    const string& getDireccion() const { return direccion_; }
    const string& getTelefono() const { return telefono_; }
    const string& getCorreo() const { return correo_; }

    // setters
    void setNombre(const string& n) { nombre_ = n; }
    void setFechaNacimiento(const string& f) { validar_y_setear_fecha_nacimiento(f); }
    void setCategoria(const string& c) { validar_y_setear_categoria(c); }
    void setSalario(double s) { validar_y_setear_salario(s); }
    void setDireccion(const string& d) { direccion_ = d.empty() ? "San Jose" : d; }
    void setTelefono(const string& t) { telefono_ = t; }
    void setCorreo(const string& c) { validar_y_setear_correo(c); }

    // mostrar info completa
    void mostrar(ostream& os) const {
        os << "Carnet: " << numero_carnet_ << "\n";
        os << "Nombre: " << nombre_ << "\n";
        os << "Fecha de nacimiento: " << fecha_nacimiento_ << " (edad aprox: ";
        int y, m, d;
        if (parsear_fecha(fecha_nacimiento_, y, m, d)) os << calcular_edad(y, m, d);
        else os << "?";
        os << ")\n";
        os << "Categoria: " << categoria_a_texto(categoria_) << "\n";
        os << "Salario: " << salario_ << "\n";
        os << "Direccion: " << direccion_ << "\n";
        os << "Telefono: " << telefono_ << "\n";
        os << "Correo: " << correo_ << "\n";
    }
};

vector<string> Empleado::correos_usados_;


class Proyecto {
private:
    string codigo_;
    string nombre_;
    string fecha_inicio_;
    string fecha_finalizacion_;

    // nombres unicos (simulacion)
    static vector<string> nombres_usados_; // en minusculas

    static bool nombre_existe(const string& low) {
        for (size_t i = 0; i < nombres_usados_.size(); ++i)
            if (nombres_usados_[i] == low) return true;
        return false;
    }
    static void registrar_nombre(const string& low) { nombres_usados_.push_back(low); }
    static void eliminar_nombre(const string& low) {
        for (size_t i = 0; i < nombres_usados_.size(); ++i) {
            if (nombres_usados_[i] == low) {
                nombres_usados_.erase(nombres_usados_.begin() + (int)i);
                return;
            }
        }
    }
    void validar_y_setear_nombre(const string& n) {
        string low = a_minusculas(n);
        if (low.empty())
            throw runtime_error("El nombre del proyecto no puede estar vacio.");
        if (!nombre_.empty()) {
            string anterior_low = a_minusculas(nombre_);
            if (anterior_low != low) eliminar_nombre(anterior_low);
            else return;
        }
        if (nombre_existe(low))
            throw runtime_error("El nombre del proyecto ya existe.");
        registrar_nombre(low);
        nombre_ = n;
    }

public:
    Proyecto(const string& codigo, const string& nombre,
             const string& fecha_inicio, const string& fecha_fin)
        : codigo_(codigo), nombre_(""),
          fecha_inicio_(fecha_inicio), fecha_finalizacion_(fecha_fin)
    { validar_y_setear_nombre(nombre); }

    // getters
    const string& getCodigo() const { return codigo_; }
    const string& getNombre() const { return nombre_; }
    const string& getFechaInicio() const { return fecha_inicio_; }
    const string& getFechaFinalizacion() const { return fecha_finalizacion_; }

    // setters
    void setNombre(const string& n) { validar_y_setear_nombre(n); }
    void setFechaInicio(const string& f) { fecha_inicio_ = f; }
    void setFechaFinalizacion(const string& f) { fecha_finalizacion_ = f; }

    // mostrar
    void mostrar(ostream& os) const {
        os << "Codigo: " << codigo_ << "\n";
        os << "Nombre: " << nombre_ << "\n";
        os << "Fecha de inicio: " << fecha_inicio_ << "\n";
        os << "Fecha de finalizacion: " << fecha_finalizacion_ << "\n";
    }
};

vector<string> Proyecto::nombres_usados_;


class GestorSistema {
private:
    vector<Empleado> empleados_;
    vector<Proyecto> proyectos_;

    struct Asignacion {
        string carnet_empleado;
        string codigo_proyecto;
        string fecha_asignacion; // "YYYY-MM-DD"
    };
    vector<Asignacion> asignaciones_;

    // helpers de busqueda lineal
    int buscarEmpleadoPorCarnet(const string& carnet) const {
        for (size_t i = 0; i < empleados_.size(); ++i)
            if (empleados_[i].getCarnet() == carnet) return (int)i;
        return -1;
    }
    int buscarProyectoPorCodigo(const string& codigo) const {
        for (size_t i = 0; i < proyectos_.size(); ++i)
            if (proyectos_[i].getCodigo() == codigo) return (int)i;
        return -1;
    }
    bool asignacionExiste(const string& carnet, const string& codigo) const {
        for (size_t i = 0; i < asignaciones_.size(); ++i)
            if (asignaciones_[i].carnet_empleado == carnet &&
                asignaciones_[i].codigo_proyecto == codigo) return true;
        return false;
    }

public:
    // crear empleado sin salario (250000 por defecto)
    bool crearEmpleado(const string& carnet, const string& nombre,
                       const string& fecha_nac, const string& categoria,
                       const string& direccion, const string& telefono,
                       const string& correo)
    {
        if (buscarEmpleadoPorCarnet(carnet) != -1) {
            cout << "Aviso: ya existe un empleado con ese carnet.\n";
            return false;
        }
        try {
            Empleado e(carnet, nombre, fecha_nac, categoria, direccion, telefono, correo);
            empleados_.push_back(e);
            return true;
        } catch (const std::exception& ex) {
            cout << "Error al crear empleado: " << ex.what() << "\n";
            return false;
        }
    }

    // crear empleado con salario
    bool crearEmpleado(const string& carnet, const string& nombre,
                       const string& fecha_nac, const string& categoria,
                       double salario, const string& direccion,
                       const string& telefono, const string& correo)
    {
        if (buscarEmpleadoPorCarnet(carnet) != -1) {
            cout << "Aviso: ya existe un empleado con ese carnet.\n";
            return false;
        }
        try {
            Empleado e(carnet, nombre, fecha_nac, categoria, salario, direccion, telefono, correo);
            empleados_.push_back(e);
            return true;
        } catch (const std::exception& ex) {
            cout << "Error al crear empleado: " << ex.what() << "\n";
            return false;
        }
    }

    // crear proyecto
    bool crearProyecto(const string& codigo, const string& nombre,
                       const string& fecha_inicio, const string& fecha_fin)
    {
        if (buscarProyectoPorCodigo(codigo) != -1) {
            cout << "Aviso: ya existe un proyecto con ese codigo.\n";
            return false;
        }
        try {
            Proyecto p(codigo, nombre, fecha_inicio, fecha_fin);
            proyectos_.push_back(p);
            return true;
        } catch (const std::exception& ex) {
            cout << "Error al crear proyecto: " << ex.what() << "\n";
            return false;
        }
    }

    // listar empleados
    void listarEmpleados(ostream& os) const {
        os << "--- LISTA DE EMPLEADOS ---\n";
        for (size_t i = 0; i < empleados_.size(); ++i) {
            os << "--------------------------\n";
            empleados_[i].mostrar(os);
        }
        os << "==========================\n";
    }

    // listar proyectos
    void listarProyectos(ostream& os) const {
        os << "--- LISTA DE PROYECTOS ---\n";
        for (size_t i = 0; i < proyectos_.size(); ++i) {
            os << "--------------------------\n";
            proyectos_[i].mostrar(os);
        }
        os << "==========================\n";
    }

    // asignar empleado a proyecto (fecha actual), sin duplicados
    bool asignarEmpleadoAProyecto(const string& carnet, const string& codigo) {
        int idxE = buscarEmpleadoPorCarnet(carnet);
        if (idxE == -1) { cout << "No existe el empleado.\n"; return false; }

        int idxP = buscarProyectoPorCodigo(codigo);
        if (idxP == -1) { cout << "No existe el proyecto.\n"; return false; }

        if (asignacionExiste(carnet, codigo)) {
            cout << "El empleado ya esta asignado a ese proyecto.\n";
            return false;
        }

        Asignacion a;
        a.carnet_empleado = carnet;
        a.codigo_proyecto = codigo;
        a.fecha_asignacion = fecha_hoy();
        asignaciones_.push_back(a);
        return true;
    }

    // listar empleados asignados a un proyecto
    void listarEmpleadosDeProyecto(const string& codigo, ostream& os) const {
        int idxP = buscarProyectoPorCodigo(codigo);
        if (idxP == -1) { os << "Proyecto no encontrado.\n"; return; }

        os << "--- EMPLEADOS EN PROYECTO [" << codigo << "] ---\n";
        for (size_t i = 0; i < asignaciones_.size(); ++i) {
            if (asignaciones_[i].codigo_proyecto == codigo) {
                int idxE = buscarEmpleadoPorCarnet(asignaciones_[i].carnet_empleado);
                if (idxE != -1) {
                    os << "- " << empleados_[idxE].getCarnet()
                       << " | " << empleados_[idxE].getNombre()
                       << " | Categoria: " << categoria_a_texto(empleados_[idxE].getCategoria())
                       << " | Asignado el: " << asignaciones_[i].fecha_asignacion
                       << "\n";
                }
            }
        }
        os << "=============================================\n";
    }

    // listar proyectos en los que trabaja un empleado
    void listarProyectosDeEmpleado(const string& carnet, ostream& os) const {
        int idxE = buscarEmpleadoPorCarnet(carnet);
        if (idxE == -1) { os << "Empleado no encontrado.\n"; return; }

        os << "--- PROYECTOS DEL EMPLEADO [" << carnet << "] ---\n";
        for (size_t i = 0; i < asignaciones_.size(); ++i) {
            if (asignaciones_[i].carnet_empleado == carnet) {
                int idxP = buscarProyectoPorCodigo(asignaciones_[i].codigo_proyecto);
                if (idxP != -1) {
                    os << "- " << proyectos_[idxP].getCodigo()
                       << " | " << proyectos_[idxP].getNombre()
                       << " | Asignado el: " << asignaciones_[i].fecha_asignacion
                       << "\n";
                }
            }
        }
        os << "=============================================\n";
    }
};


static void pausar() {
    cout << "Presione ENTER para continuar...";
    cin.ignore(10000, '\n');
}

static string leer_linea(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

static double leer_double(const string& prompt) {
    while (true) {
        cout << prompt;
        string s; getline(cin, s);
        double val = 0.0;
        if (std::sscanf(s.c_str(), "%lf", &val) == 1) return val;
        cout << "Entrada invalida, intente de nuevo.\n";
    }
}

static int leer_opcion() {
    string s; getline(cin, s);
    int op = -1;
    std::sscanf(s.c_str(), "%d", &op);
    return op;
}


static void imprimir_menu() {
    cout << "\n--- MENU SISTEMA CONSTRUCTORES AVANCE ---\n";
    cout << "1) Crear empleado (salario por defecto 250000)\n";
    cout << "2) Crear empleado (con salario)\n";
    cout << "3) Listar empleados\n";
    cout << "4) Crear proyecto\n";
    cout << "5) Listar proyectos\n";
    cout << "6) Asignar empleado a proyecto\n";
    cout << "7) Listar empleados de un proyecto\n";
    cout << "8) Listar proyectos de un empleado\n";
    cout << "0) Salir\n";
    cout << "Seleccione opcion: ";
}

int main() {
    GestorSistema gs;

    while (true) {
        imprimir_menu();
        int op = leer_opcion();

        if (op == 0) {
            cout << "Saliendo...\n";
            break;
        }

        if (op == 1) {
            cout << "\n-- Crear empleado (salario por defecto) --\n";
            string carnet = leer_linea("Carnet: ");
            string nombre = leer_linea("Nombre: ");
            string fnac   = leer_linea("Fecha de nacimiento (YYYY-MM-DD): ");
            string cat    = leer_linea("Categoria (Administrador/Operario/Peon): ");
            string dir    = leer_linea("Direccion (vacio -> San Jose): ");
            string tel    = leer_linea("Telefono: ");
            string corr   = leer_linea("Correo: ");

            bool ok = gs.crearEmpleado(carnet, nombre, fnac, cat, dir, tel, corr);
            if (ok) cout << "Empleado creado.\n";
            else    cout << "No se creo el empleado.\n";
        }
        else if (op == 2) {
            cout << "\n-- Crear empleado (con salario) --\n";
            string carnet = leer_linea("Carnet: ");
            string nombre = leer_linea("Nombre: ");
            string fnac   = leer_linea("Fecha de nacimiento (YYYY-MM-DD): ");
            string cat    = leer_linea("Categoria (Administrador/Operario/Peon): ");
            double sal    = leer_double("Salario (250000..500000): ");
            string dir    = leer_linea("Direccion (vacio -> San Jose): ");
            string tel    = leer_linea("Telefono: ");
            string corr   = leer_linea("Correo: ");

            bool ok = gs.crearEmpleado(carnet, nombre, fnac, cat, sal, dir, tel, corr);
            if (ok) cout << "Empleado creado.\n";
            else    cout << "No se creo el empleado.\n";
        }
        else if (op == 3) {
            cout << "\n-- Lista de empleados --\n";
            gs.listarEmpleados(std::cout);
        }
        else if (op == 4) {
            cout << "\n-- Crear proyecto --\n";
            string cod  = leer_linea("Codigo del proyecto: ");
            string nom  = leer_linea("Nombre del proyecto (unico): ");
            string ini  = leer_linea("Fecha de inicio (YYYY-MM-DD): ");
            string fin  = leer_linea("Fecha de finalizacion (YYYY-MM-DD): ");

            bool ok = gs.crearProyecto(cod, nom, ini, fin);
            if (ok) cout << "Proyecto creado.\n";
            else    cout << "No se creo el proyecto.\n";
        }
        else if (op == 5) {
            cout << "\n-- Lista de proyectos --\n";
            gs.listarProyectos(std::cout);
        }
        else if (op == 6) {
            cout << "\n-- Asignar empleado a proyecto --\n";
            string carnet = leer_linea("Carnet del empleado: ");
            string codigo = leer_linea("Codigo del proyecto: ");
            bool ok = gs.asignarEmpleadoAProyecto(carnet, codigo);
            if (ok) cout << "Asignacion realizada.\n";
            else    cout << "No se realizo la asignacion.\n";
        }
        else if (op == 7) {
            cout << "\n-- Empleados de un proyecto --\n";
            string codigo = leer_linea("Codigo del proyecto: ");
            gs.listarEmpleadosDeProyecto(codigo, std::cout);
        }
        else if (op == 8) {
            cout << "\n-- Proyectos de un empleado --\n";
            string carnet = leer_linea("Carnet del empleado: ");
            gs.listarProyectosDeEmpleado(carnet, std::cout);
        }
        else {
            cout << "Opcion invalida.\n";
        }
    }

    return 0;
}

