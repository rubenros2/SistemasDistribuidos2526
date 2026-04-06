const LENTROZO   = 64;
const LENMSG     = 256;
const LENNOMFILE = 128;

typedef opaque chunk<LENTROZO>;
typedef string cadenaL<LENMSG>;
typedef string cadenaS<LENNOMFILE>;

struct param1 {
   cadenaS filename;
   long offset;
   int size;
};

struct listatrozos {
   chunk trozo;
   listatrozos * sgte;
};


union Resultado switch (int caso){
   case 0: bool b;
   case 1: long val;
   case 2: listatrozos *lista;
   case 3: cadenaL err;
};

program SRVWWW{
   version PRIMERA{
     Resultado existfile(cadenaS)=1;
     Resultado getsize(cadenaS)=2;
     Resultado getchunk(param1)=3;
   }=1;
}=0x2026f001;

