#define _GNU_SOURCE
/***********************************************
*
* @Proposit: Fitxer principal del funcionament de Bowman.
* @Autor/s: Alberto Marquillas Marsà i Alba Falón Roy
* @Data creacio: 10/10/2024
* @Data ultima modificacio:
*
************************************************/
//Include el .h associat a les funcions de Fleck
#include "Fleck_funct/fleck_funct.h"
//Include del .h associat a les comunicacions de Fleck
#include "Fleck_comms/fleck_comms.h" 

//Definicions
#define USR_INIT "user initialized\n" //Missatge de benvinguda

//Crem una instancia de la estructura FleckConfig
FleckConfig fleck_config_file;
char* command = NULL; //Cadena de caràcters per guardar la comanda




void freeMemory(){
    if(fleck_config_file.user_name != NULL){
        free(fleck_config_file.user_name);
        fleck_config_file.user_name = NULL;
    }
    if(fleck_config_file.folder_name != NULL){
        free(fleck_config_file.folder_name);
        fleck_config_file.folder_name = NULL;
    }
    if(fleck_config_file.gotham_ip != NULL){
        free(fleck_config_file.gotham_ip);
        fleck_config_file.gotham_ip = NULL;
    }
    if(fleck_config_file.gotham_port != NULL){
        free(fleck_config_file.gotham_port);
        fleck_config_file.gotham_port = NULL;
    }
    if(command != NULL){
        free(command);
        command = NULL;
    }
    
}


/***********************************************
 * @Nom: Fleck_signalHandler
 * @Definicio: Funció que gestiona la senyal SIGINT
 * @Arg: int signum: Senyal que es rep
 * @Ret: ----
 * ***********************************************/
void Fleck_signalHandler(int signum) {

    switch (signum) {
        case SIGINT:
            printF("L'usuari ha forçat la desconnexio\n");
            freeMemory();        
            break;

        case SIGUSR1:
            FleckComms_EsperaFinalizacioThreads();
            //Funcio que espera que acabi el thread
            freeMemory();
            break;
        default:
            break;
  
    }

    exit(0);
}




//Funcio principal del programa
int main(int argc, char *argv[]) {

    //Comprovem que el nombre d'arguments sigui correcte
    if (argc != 2) { 
        perror(ERROR_ARGS);
        exit(1);
    }
    
    signal(SIGINT, (void *) Fleck_signalHandler); //Gestiona la senyal SIGINT
    signal(SIGUSR1, (void *) Fleck_signalHandler); //Gestiona la senyal SIGUSR1
    //Llegim el fitxer de configuració
    fleck_config_file = FleckFunct_readConfigFleck(argv[1], fleck_config_file);

    fleck_config_file.user_name[strlen(fleck_config_file.user_name)] = '\0';
    //Printem la benvinguda (user initialized)
    Utils_printarBenvinguda(USR_INIT, fleck_config_file.user_name);

    int logout = 0;
    //pasar int por parametro a la funcion
           
    signal(SIGPIPE, SIG_IGN); //Ignorem la senyal SIGPIPE
        
    //Esperem a que l'usuari entri una comanda
    while (1) {
        //Printem el prompt
        printF("\n$ ");
        command = strdup(FleckFunct_llegirComanda()); //Llegim la comanda
        command[strlen(command)] = '\0';
        //command = FleckFunct_llegirComanda(command); 
        //Llegim la comanda
        // //Si la comanda es correcta, la executem
        logout = FleckComms_ExecuteCommandFleck(command, logout, fleck_config_file.folder_name, fleck_config_file.gotham_port, fleck_config_file.gotham_ip, fleck_config_file.user_name);

        command = NULL;
    }
    freeMemory();

    return 0;

} 
