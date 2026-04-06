import re

# Nombre del fichero a parchear
FILENAME = "srvwww_svc.c"

# Fragmento que se debe incluir después del último `#include`
# donde se declara la variable global `documentroot`
global_patch = r"""

//Include añadido para utilizar la función existe_carpeta
#include "util.h"

/***** VARIABLE GLOBAL AÑADIDA   ******/
char *documentroot;
"""

# Fragmento que se debe insertar dentro de `main()`, al principio,
# antes de `svcudp_create` o `svctcp_create`
main_patch = r"""
    //Añadido al código generado por rpcgen
    /****************************************************************************** */
       if (argc<2)
       {
         fprintf(stderr,"Forma de uso: %s ruta_absoluta_document_root \n",argv[0]);
         exit(1);
       }
       documentroot=strdup(argv[1]);
       if (documentroot==NULL)
       {
          fprintf(stderr,"Error: No se pudo reservar memoria para almacenar el nombre de la carpeta document root\n");
          exit(12);
       }
       if (documentroot[strlen(documentroot)-1]=='/') documentroot[strlen(documentroot)-1]=0;
       if (!existe_carpeta(documentroot))
       {
          fprintf(stderr,"Error: La carpeta %s no existe o no es una carpeta\n",documentroot);
          exit(13);
       }
	   fprintf(stderr,"Servidor: La raíz de documentos es: %s\n",documentroot);
    /******************************************************************************/
"""

# Leer el archivo original
with open(FILENAME, "r") as f:
    lines = f.readlines()

# Insertar `global_patch` después del último `#include`
insert_index = 0
for i, line in enumerate(lines):
    if line.startswith("#include"):
        insert_index = i + 1  # Justo después del último #include

# Evitar insertar varias veces
if global_patch not in "".join(lines):
    lines.insert(insert_index, global_patch + "\n")
else:
    print(
        f"El archivo {FILENAME} ya ha sido modificado anteriormente. No es necesario aplicar cambios"
    )
    exit()

# Insertar `main_patch` dentro de `main()`,
# antes de la primera llamada a `svcudp_create` o `svctcp_create`
inside_main = False
for i, line in enumerate(lines):
    if re.search(r"main *\(", line):  # Detectar inicio de main()
        inside_main = True

    if inside_main and re.search(
        r"\bsvc(udp|tcp)_create\b", line
    ):  # Buscar la primera aparición
        lines.insert(i, main_patch + "\n")
        break  # Solo insertar una vez

# Guardar el archivo modificado
with open(FILENAME, "w") as f:
    f.writelines(lines)

print(f"Modificaciones aplicadas a {FILENAME}")
