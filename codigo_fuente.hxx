    /*  Autor: Jim Dios Luna, 
        Adaptado del Demo de TCP Socket de mbed simulator: https://simulator.mbed.com/#tcpsocket
        Última Modificación: 22/06/2020. 
    */

    //Declaración de las librerías a utilizar

    #include "mbed.h"
    #include "NetworkInterface.h"
    #include "TCPSocket.h"
    #include "http_request.h"

    //Declaración de componentes a utilizar

    AnalogIn pot(p15);
    BusOut led(LED1); //or DigitalOut

    // Generamos un acceso a la Interface de red por defecto

    NetworkInterface *net = NetworkInterface::get_default_instance();

    // verificación de funcionamiento de socket

    int main() {
        printf("Lectura de datos potenciometro - SDEU \n");
        net->connect();

        // Mostramos los parametros de red
        const char *ip = net->get_ip_address();
        const char *mac = net->get_mac_address();
        const char *gateway = net->get_gateway();
        printf("IP address: %s\n", ip ? ip : "None");
        printf("MAC address: %s\n", mac ? mac : "None");
        printf("Gateway: %s\n", gateway ? gateway : "None");

        // Generamos la conexion de socket TCP *SE DEBE CAMBIAR EL PUERTO AL GENERAR EL LA CONEXION TCP 

        TCPSocket socket;
        socket.open(net);
        socket.connect("2.tcp.ngrok.io", 14036);
        
        while(1) {
            // Hacemos un HttpRequest a nuestro servidor para obtener el valor del Led
            {
                // SE DEBE CAMBIAR LA RUTA DE LA API SEGUN SEA GENERADA
                HttpRequest* get_req = new HttpRequest(net, HTTP_GET, "http://3e68cbb3a2c4.ngrok.io/api/light");
        
                HttpResponse* get_res = get_req->send();
                if (!get_res) {
                    printf("HttpRequest failed (error code %d)\n", get_req->get_error());
                    return 1;
                }

                // mostramos el valor de nuestro httprequest
                printf("El estado del led es: %s \n", get_res->get_body_as_string().c_str()); 
                
                //Cambiamos de estado ON/OFF el Led del mbed
                if (get_res->get_body_as_string() == "true") {
                    led = 1;
                } else {
                    led = 0;
                }
        
                delete get_req;
            }
            
            //Declaramos un buffer de 10 caracteres y una variable tmp de 20 caracteres
            char *buffer = new char[10];
            char temp[20];

            // enviamos un vía socket con el valor leido del potenciómetro
            sprintf(temp, "%.2f", (pot.read()*10)/2);
            strcpy(buffer, temp);
            int scount = socket.send(buffer, strlen(buffer));
            
            //mostramos los datos enviados
            printf("El valor del potenciometro es:%s %.*sV\n", scount, (int)(strstr(buffer, "\r\n")-buffer), buffer);
        
        //Definimos un wait
            wait(2);
            delete[] buffer;
        }

        // Cerramos la conexion del socket 
        socket.close();
        

        // desconectamos la conexion de red
        net->disconnect();
        printf("Done\n");
    }
