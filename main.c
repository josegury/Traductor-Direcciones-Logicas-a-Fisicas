/* 
 * File:   main.c
 * Authors: José Ángel Pardo Cerdán, Daniel García Navarro, Pablo Martínez Mondéjar 
 *
 * Created on 12 de noviembre de 2013, 16:30
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//------------------------------------------
//------------Variables predefinidas -------
//------------------------------------------

#define maxPag 15       //número máximo de filas de la tabla paginación
#define maxSeg 15       //número máximo de filas de la tabla segmentación
#define maxProce 6      //número máximo de procesos

//------------------------------------------
//--------Estructuras Necesarias------------
//------------------------------------------

struct particionamiento	//Datos necesarios para las direcciones de de particionamiento
 {
    
    int dir_logica;	//Direccion lógica
    int dir_fisica;	//Direccion física
    int regBase;	//Registro base
    int regLimite;	//Registro límite
};

struct segmentacion		//Datos necesarios para las direcciones de segmentación
{
    int nFilas;				//numero de filas de la tabla
    int base[maxSeg]; 		// Columna registro base de la tabla de segmentacion
    int longitud[maxSeg];	// Columna registro Longitud de la tabla de segmentación
    int segmento[maxSeg]; 	// Columna registro segmento de la tabla de segmentación
    int RBTS; 				// Registro base tabla de segmentación
    int RLTS; 				// Registro límite tabla de segmentación 
};
 
struct paginacion 		//Datos necesarios para las direcciones de paginación
{
    int nFilas;
    int pagina[maxPag];	//Columna página de la tabla paginación
    int marco[maxPag];  //Columna marco de pagina de la tabla paginación
    int RBTP; 			//Registro base tabla paginación
    int RLTP; 			//Registro límite tabla paginación              
    int tam_pagina;		//tama?o de pagina
};

struct proceso 			//Estructura donde se almacenarán los datos de particionamiento, segmentación y paginación pertenecientes a un proceso
{
    struct particionamiento p;
    struct segmentacion seg;
    struct paginacion pag;
};

int Nprocesos;			//Variable global: número de procesos existenetes
int Ndirecciones;		//Variable global: número de direcciones a traducir

void LeerFichDirecciones(char ruta[],char **DireccionesLog);
//------------------------------------------
//-------------Función Principal------------
//------------------------------------------
int main(int argc, char** argv) {
        
    
    struct proceso proces[maxProce];//Array de procesos donde almacenaremos los datos de los dierentes procesos existentes
    int i=0,j=0,k=0;   					//Variables auxiliares
    char ruta[250]; 				//Cadena donde almacenaremos la ruta del fichero que contiene los datos de los procesos
    char ruta2[250]; 				//Cadena donde almacenaremos la ruta del fichero que contiene las direcciones logicas a traducir
    int *** resultados;				//Array tridimensional donde almacenaremos los resultados de las posibles direcciones físicas 
    								//con la forma:  resultados[número de direccion][número de proceso][3]
    								//donde la ultima dimension "[3]" contiene: en la pos 0: dir física de particionamiento
    																		  //en la pos 1: dir física de paginación
    																		  //en la pos 2: dir física de segmentación
    char ** direccionesLog;			//Array de cadenas donde almacenaremos las direcciones lógicas a traducir
    // Las variables resultados y direccionesLog se crean como punteros de su tipo puesto que según sea el caso tendrán tama?os 
    // diferentes, de esta manera conseguimos darles el tama?o deseado segun se de el caso.
    switch(argc)
    {
    	//Caso en el que se le pasa un parametro
        case 2:
            strcpy(ruta,argv[1]);								//copio el primer parámetro en la variable ruta
            Nprocesos = cargarProcesos(proces,ruta);			//cargo los procesos que se encuentran en el fichero 1
            Ndirecciones=1;
            // -- Proceso de creación de arrays -------------------------------------------------------------------------------------------------------------------
            resultados = (int***) malloc(1*sizeof(int**));				//Aquí es donde designamos el tama?o deseado para las variables resultado y direccionesLog
			resultados[0] = (int**)malloc(Nprocesos*sizeof(int*));		//En este caso puesto que el número de parametros introducido es 1 se pedirá por pantalla
				for(i=0;i<Nprocesos;i++) 								//una dirección a traducir y puesto que solo tendremos que traducir 1 dirección para N procesos
					resultados[0][i] = (int*)malloc(3*sizeof(int)); 	//Creamos el array resultados de la forma: resultados[1][Nprocesos][3]
				
            direccionesLog = (char**) malloc(1*sizeof(char*));			//Creamos el array direccionesLog de la forma: [1][100]
			direccionesLog[0] = (char*)malloc(50*sizeof(char));
           //------------------------------------------------------------------------------------------------------------------------------------------------------
            printf(" Introduzca una direccion lógica para convertirla a física:  \n >> "); 	//Se pide por pantalla la dirección a traducir 
            scanf("%s",direccionesLog[0]);													//Se almacena 
            resolver(1,direccionesLog,proces,resultados);									//Se resuelve la direcccion introducida
            
            guardar(resultados, direccionesLog,1);//almacena el resultado en el fichero correspondiente
            
            break;
       
       	//Caso en el que se le pasan dos parámetros
	    case 3:
            strcpy(ruta,argv[1]);						//Copio el primer parámetro en la variable ruta
            Nprocesos = cargarProcesos(proces,ruta);	//Cargo los procesos que se encuentran en el fichero 1 y almaceno en la variable global Nprocesos el número de procesos encontrado en el fichero
			strcpy(ruta2,argv[2]);						//Copio el segundo parametro en la variable ruta2
            Ndirecciones=NumDireccionesLogicas(ruta2);	//Almaceno en la variable global Ndirecciones el número de direcciones encontradas en el fichero2 
           
			 // -- Proceso de creación de arrays -----------------------------------------------------------------------------------------------------	
            resultados = (int***) malloc(Ndirecciones*sizeof(int**));			//En este caso creamos el array de resultados de la forma:
			direccionesLog = (char**) malloc(Ndirecciones*sizeof(char*));		//	resultados [Ndirecciones][Nprocesos][3]
            for(i=0;i<Ndirecciones;i++){										//Y direccionesLog de la forma: 
            	direccionesLog[i] = (char*)malloc(50*sizeof(char));				//	direccionesLog[Ndirecciones][100]
				resultados[i] = (int**)malloc(Nprocesos*sizeof(int*));
					for(j=0;j<Nprocesos;j++) {
						resultados[i][j] = (int*)malloc(3*sizeof(int));
					}
			}
             //------------------------------------------------------------------------------------------------------------------------------------------
			LeerFichDirecciones(ruta2,direccionesLog);				//leemos las direcciones lógicas a convertir del fichero2 y almacenamos en la variable direccionesLog
			resolver(Ndirecciones,direccionesLog,proces,resultados);//Convertimos las direcciones logicas a físicas y almacenamos en "resultados"
            	
                         guardar(resultados, direccionesLog,Ndirecciones);//almacena los resultados en los ficheros correspondientes
            break;
        default: 
            printf(" Error sintaxis invalida!\n");
            exit(1);//termina el programa
            break;
    }

    
    return (EXIT_SUCCESS);
}


int cargarProcesos(struct proceso proces[],char ruta[])
{
    FILE* fichero;
    char linea[50];    
    int si_tabla=0;//interruptor para saber con que tabla trabajar
    int nProcInfo=0;
    int nProcPag=-1;//se declara a -1 porque cuando entra a la opcion de pagina se pasa a 0
    int j=0;
    
    fichero = fopen(ruta, "rt");//abro el fichero en modo lectura y de tipo texto
    
    if (fichero == NULL)//compruebo si existe el fichero
    {
      printf(" No existe el fichero!\n");
      exit(1);//termina el programa
    }
    
    fscanf(fichero,"%s",&linea);//leo la primera linea del documento
    
    while (! feof(fichero)) 
    {
        switch(linea[0])
         {
             case 't':
                 fscanf(fichero,"%d",&proces[0].pag.tam_pagina);
                 break;
             case 'i':
                  si_tabla=3; //entra en la tabla de informacion
                 break;            
            case 'p':
                  si_tabla=1;//se modificara la tabla de paginacion
                  nProcPag++;//como ha encontrado una tabla de paginacion se suma el puntero para acceder a ese proceso en concreto
                 
                  break;
            case 's':
                  si_tabla=2;//se modificara la tabla de segmentacion
                 break;
            case '#':
                  si_tabla=0;//termina la tabla
                 break;
            default://entra a leer las tablas
                 switch(si_tabla)
                 { 
                     case 1:
                     	 proces[nProcPag].pag.pagina[proces[nProcPag].pag.nFilas] = atoi(linea);//comvierto la cadena leida en entero y lo asigno a la pagina
                         fscanf(fichero,"%d",&proces[nProcPag].pag.marco[proces[nProcPag].pag.nFilas]);//agrego el margo
                         proces[nProcPag].pag.nFilas++;//sumo el numero de filas de la tabla                         
                         break;
                         
                     case 2:
                         proces[0].seg.segmento[proces[0].seg.nFilas] = atoi(linea);
                         fscanf(fichero,"%d",&proces[0].seg.base[proces[0].seg.nFilas]);//agrego el margo
                         fscanf(fichero,"%d",&proces[0].seg.longitud[proces[0].seg.nFilas]);//agrego el margo
                         proces[0].seg.nFilas++;//sumo el numero de filas de la tabla                     	
                         break;
                         
                     case 3:
                         //inicializo los indices de las tablas de paginacion y segmentacion
                         proces[nProcInfo].pag.nFilas=0;
                         proces[nProcInfo].seg.nFilas=0;
                         
                         proces[nProcInfo].p.regBase = atoi(linea);//recojo el registro base
                         fscanf(fichero,"%d",&proces[nProcInfo].p.regLimite);//recojo el registro limite
                         fscanf(fichero,"%d",&proces[nProcInfo].seg.RBTS);//recojo RBTS
                         fscanf(fichero,"%d",&proces[nProcInfo].seg.RLTS);//recojo RLTS
                         fscanf(fichero,"%d",&proces[nProcInfo].pag.RBTP);//recojo RBTP
                         fscanf(fichero,"%d",&proces[nProcInfo].pag.RLTP);//recojo RLTP
                         
                         
                         nProcInfo++;//sumo el numero del proceso
                         break;
                 }
                 break;
        }
        fscanf(fichero,"%s",&linea);
    }
    
    fclose(fichero);//cierro el fichero
    
    
    if(nProcPag==0)//las paginas son comunes para todos
    {
        for(j=1;j<nProcInfo;j++)
        {
            proces[j].pag = proces[0].pag;
        }
    }
    
    for(j=1;j<nProcInfo;j++)//a?ado a los demas procesos la taba segmentacion y el tama?o de pagina
    {
        proces[j].seg=proces[0].seg;
        proces[j].pag.tam_pagina = proces[0].pag.tam_pagina;
    }
    return nProcInfo;
}



//-----Funcion encargada de la lectura del fichero 2 donde se encuentran las direcciones lógicas a convertir 
//-----Dichas direcciones seran almacenadas en la matriz de caracteres DireccionesLog

void LeerFichDirecciones(char ruta[],char **DireccionesLog)
{
    FILE* fichero;  
    int i=0;
    fichero = fopen(ruta, "rt");//abro el fichero en modo lectura y de tipo texto
    fscanf(fichero,"%s",DireccionesLog[i]);//leo la primera linea del documento 
    i++;
    while (! feof(fichero)) 
    {
        fscanf(fichero,"%s",DireccionesLog[i]);//leo la primera linea del documento
		i++;    
	}
    fclose(fichero);
}

//Funcion que cuenta el numero de direcciones logicas contenidas en el fichero 2
int NumDireccionesLogicas(char ruta []){
	int fila[100],Ndirs=0;
	FILE* fichero;  
	fichero = fopen(ruta, "rt");//abro el fichero en modo lectura y de tipo texto
	
	if (fichero == NULL)//compruebo si existe el fichero
    {
      printf(" No existe el fichero!\n");
      fclose(fichero);
      exit(1);//termina el programa
    }
	while(fscanf(fichero,"%s",fila)!=EOF){
   		Ndirs++;
   }
   fclose(fichero);
   return Ndirs;
}

//---------Funcion encargada de resolver las direcciones logicas leidas a direcciones físicas-----------
/*
* Par??metros de entrada:
* la función almacena en la matriz tridimensional  resultados[direccionLogica][Proceso][3] donde la ultima dimension ([3]) es 3, una por cada tipo de esquema
* en la matriz de resultados se almacena , un -1 indica ERROR , un -2 indica INCOMPATIBLE y -3 querrá decir que el formato de la direccion introducida es errónea
*/
void resolver(int Ndirecciones,char **direccionesLog, struct proceso p[],int ***resultados){
    int d=0,i=0,j=0; 	// Variables indice donde d=indice de DireccionLogica, i=indice de proceso y j es auxiliar 
    int marco;		 	// Variable donde se almacena el numero de marco
    int direccion;	  	// Variable donde se almacena el numero de direccion de la tabla de segmentacion
    int base, longitud; // Variables donde se almacenan el base y longitud de la tabla de segmentación
    int dirlogica[2]; 	// Array auxiliar de tipo entero utilizado para operar con las direcciones cuyo formato es (pagina/segmento,desplazamiento)
    					// 		dirlogica[0]=pagina/segmento
    					// 		dirlogica[1]=esplazamiento
    
    for(d=0;d<Ndirecciones;d++)
    {
	    switch(formato(direccionesLog[d]))//dependiendo del formato de la direccion logica se realiza una funcion u otra
	    {
	        case 1://direccion logica simple
	                direccion = atoi(direccionesLog[d]);//combierte la direccion a entero
	                for(i=0;i<Nprocesos;i++)//para cada proceso...
	                {
	                   //Registro base + limite
	                   if(direccion <= p[i].p.regLimite)
	                   {//se calcula:
	                        resultados[d][i][0]= direccion + p[i].p.regBase;// .particionamiento.regBase;
	                   }
	                   else resultados[d][i][0] = -1;//si no la direccion es erronea
	                   //Paginacion
	                   dirlogica[0] = direccion/p[i].pag.tam_pagina;//se pasa la direcion logica de formato simple al otro formato
	                   dirlogica[1] = direccion%p[i].pag.tam_pagina;
	                   for(j=0;j<p[i].pag.nFilas;j++)//recorrer la tabla de paginacion
	                   {
	                       if(dirlogica[0]==p[i].pag.pagina[j]);//buscar la pagina correspondiente a la direcion logica de la tabla de paginacion
	                       {
	                           marco = p[i].pag.marco[j]; //se almacena el marco asociado a la pagina
	                       }  
						}
	                   resultados[d][i][1] = marco*(p[i].pag.tam_pagina + dirlogica[1]);//calcula el resultado
	                   //Segmentacion
	                   resultados[d][i][2] = -2;//el formato es incompatible
	                }
	                break;
	           case 2://direccion logica en formato: (pagina/segmento, desplazamiento)
	                dirtoa(direccionesLog[d], dirlogica);
	         		//Registro base + limite
	         		for(i=0;i<Nprocesos;i++){//para cada uno de los procesos...
	         			resultados[d][i][0] = -2;//el formato para el particionamiento es incompatible
	         		
					//Paginacion
		         		if(dirlogica[0]<=p[i].pag.RLTP){//compruebla la primera parte de la direccion logica con el RLTP
		         			for(j=0;j<p[i].pag.nFilas;j++){//Se busca el marco en la tabla de segmentacion
		          				if(dirlogica[0]==p[i].pag.pagina[j]){
		                			marco = p[i].pag.marco[j];
		          				}
		          			}     
		         			resultados[d][i][1] = marco*(p[i].pag.tam_pagina+dirlogica[1]);  //Se calcula              
		         		}
		         		else resultados[d][i][1] = -1;//direccion erronea
	     			
			 		//Segmentacion
		        		 if(dirlogica[0]<=p[i].seg.RLTS){
		        		 	if((p[i].seg.RBTS+dirlogica[0])<p[i].seg.nFilas)
		        		 	{
			               		longitud = p[i].seg.longitud[(p[i].seg.RBTS+dirlogica[0])];
			               		if(dirlogica[1]<=longitud){
			                     	base =  p[i].seg.base[(p[i].seg.RBTS+dirlogica[0])];
			                    	resultados[d][i][2] = base + dirlogica[0];
			               		}
			               		else resultados[d][i][2] = -1;
			               	}
	               			else resultados[d][i][2] = -1;
	         			}
	         			else resultados[d][i][2] = -1;
	         		}
                        break;
	           case 0://Si el formato es erroneo almacenamos un -3 en los resultados para dar a entender que no se a podido resolver la direccion dada
	           		i=0;
	           		for(i=0;i<Nprocesos;i++){
		                resultados[d][i][0] = -3;
		                resultados[d][i][1] = -3;
		                resultados[d][i][2] = -3;
		            }
	           break;    
	    }
    }
}


//Esta funcion devuelve 1 si es un formato de direccion simple, 2 si es del tipo (pagina/segmento, desplazamiento) 
//y 0 si el formato es erróneo

int formato(char dir[])//Se le pasa por parametro la dirección lógica leida
{
    int i=0;
    
    if(dir[0]>='0' && dir[0]<='9')//Comprobamos si el primer caracter es un numero
    {
        for(i=1;i<strlen(dir);i++)//Como el primer caracter es un número, el resto debe serlo también.
        {//bucle hasta el final de la cadena.
                if(dir[0]<'0'|| dir[0]>'9')//Si el caaracter en la posicion i no es un numero retornamos error de formato (0)
                {
                        return 0;          
                }   
        }
         
        return 1;
    }
    else //Si el primer caracter no es un número
    {
        if(dir[0] == '(' ) // El otro formato posible es el del tipo (pagina/segmento,desplazamiento) por lo que el primer caracter debe ser un "("
        {//Si el primer caracter es un "abrir paréntesis" 
             i=1;// igualamos el indice a 1 para empezar a comprobar los siguientes caracteres desde el segundo caracter .
             while(dir[i]>='0'&& dir[i]<='9') //Mientras que los siguientes caracteres sean números aumentamos indice hasta llegar a uno distinto a un numero
             {
                    i++; 
             }
            	
             if(dir[i]!=',' || i==1 || i==strlen(dir)-2) //Si el caracter encontrado distinto a un numero es distinto a una coma, o lo és pero 
			  {			
			  							 //esta en la segunda o penultima posicion en adelante retornamos error de formato
                   return 0;     
              }
              
             else //Si no se ha retornado fallo es porque el formato de la direccion dada es (*,?  donde * es un número indeterminado de números y ? todavia no se sabe lo que es
             {
             	
                  i++; //aumentamos indice para pasar a comprobar la siguiente posicion despues de la coma, que debe ser uno o varios números para ser correcto el formato
                  while(dir[i]>='0'&& dir[i]<='9') //mientras que lo siguiente a la coma sea un número aumentamos el indice
                  {
                    i++; 
                  }//cuando se encuentre algo diferente a un número, lo siguiente en la direcion para que sea correcta debe ser un parentesis como último caracter
                  if(dir[i] == ')' && i==strlen(dir)-1)return 2; //Retornamos un 2 que simboliza que el formato es del tipo (pagina/segmento,desplazamiento)
                  else{ return 0;}//Si no se da la condicion retornamos error de formato.
             }
         }
         else{return 0;} // Si el primer carácter no es ni un paréntesis ni un número retornamos error de formato
    }
         
}
//Funcion auxiliar dirtoa (direccion a array), para construir un array de dos posiciones a partir de una direccion del tipo (pagina/segmento, desplazamiento)
void dirtoa(char dir[], int *dirlogica){
    int i=1;
    char aux1[20];
    char aux2[20];
    int j=0;
    while(dir[i] != ','){
                       aux1[j]=dir[i];
                       i++;
                       j++;
    }
    i++;
    j=0;
    while(dir[i] != ')'){
                       aux2[j] = dir[i];
                       i++;
                       j++;
    }
    dirlogica[0]=atoi(aux1);
    dirlogica[1]=atoi(aux2);
}

void guardar(int *** resultados,char **direccionesLog,int Ndirecciones)
{
    int j=0,k=0,i=0;
    char ficherog[50] = "./Proceso";
    char num[10];
    FILE* fichero;  
    //ficherog="Proceso";
    
    
   for (j=0;j<Nprocesos;j++)
   {
       //-----creo el nombre del fichero
       strcpy(ficherog,"./Proceso");
       sprintf(num, "%d", j);
       strcat(ficherog,num);
       
       fichero = fopen(ficherog, "wt");//abro el fichero en modo lectura y de tipo texto
       fprintf(fichero,">> PROCESO: P%d\n\n",j);
       for (i=0;i<Ndirecciones;i++)
       {
           
            fprintf(fichero,">> DIRECCION: %s\n",direccionesLog[i]);//leo la primera linea del documento 
            if(resultados[i][0][0]==-3)
            {
                    fprintf(fichero,"   (Formato erroneo)\n ");
            }
            else
            {
                for(k=0;k<3;k++)
                {
                        if(k==0)
                        {
                                fprintf(fichero,"        - Particionamiento: ");
                        }
                        if(k==1)
                        {
                                 fprintf(fichero,"        - Paginacion: ");
                        }
                        if(k==2)
                        {
                                 fprintf(fichero,"        - Segmentacion: ");
                        }

                        if(resultados[i][j][k]==-1)
                        {
                                fprintf(fichero,"Error\n");
                        }
                        else //escribo los resultados
                        {
                                if(resultados[i][j][k]==-2)
                                {
                                        fprintf(fichero,"Incompatible\n");
                                }
                                else
                                {
                                        fprintf(fichero,"%d\n",resultados[i][j][k]);
                                }
                        }
                }
            }
            fprintf(fichero,"-----------------------------------------------------\n");
       }
    fclose(fichero);
       
   }
   
    
    
    
   
}

