// Declaramos la libreria WFi
#include <WiFi.h>

// Escribimos el nombre y contraseña de la red
const char* ssid     = "BUAP_Estudiantes";       // Nombre de la red
const char* password = "f85ac21de4";     // Contraseña de la red
  
// Configuramos el servidor web en el puerto 80
WiFiServer server(80);

// Variable para almacenar el encabezado de la solicitud HTTP
String header;

// Creamos variables auxiliares para almacenar el estado actual de las salidas
String output2State = "off";    // El estado inicial del estado es apagado

// Asignamos un GPIO  cada una de la salidas
const int output2 = 4;

void setup() {
  // Iniciamos la comunicacion serial a 115200 baudios
  Serial.begin(115200);
  // Definimos los GPIO como salidas
  pinMode(output2, OUTPUT);
  // Iniciamos en bajo nuestras salidas
  digitalWrite(output2, LOW);

  // En esta parte se conecta a la red wifi usando la ssd y el password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Nos da como resultado una direccion IP para utilizar
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Con esta linea el ESP32 siempre esta escuchando clientes entrantes

  if (client) {                             // Cuando se recibe la solicitud de un nuevo cliente
    Serial.println("New Client.");          // guardara los datos entrantes,
    String currentLine = "";                // el cicli while se ejecutara mientras el cliente permanesca conectado.
    while (client.connected()) {            // No se debe cambiar esta parte del codigo
      if (client.available()) {             // a menos que sepas lo que esatsa haciendo
        char c = client.read();              
        Serial.write(c);                     
        header += c;
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            // Los encabezados HTTP siempre empiezan con un codigo de respuesta (e.g. HTTP/1.1 200 OK)
            // Y un tipo de contenido para que el cliente sepa lo que viene, y una linea en blanco:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // En esta parte del codigo las sentencias if y else verifica si se preiono el boton en la pagina web
            if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("GPIO 2 on");
              output2State = "on";
              digitalWrite(output2, HIGH);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("GPIO 2 off");
              output2State = "off";
              digitalWrite(output2, LOW);
            } 
            
            // En esta parte del codigo diseñamos la pagina HTML
            client.println("<!DOCTYPE html><html>");                                                              // Le indicamos que realizaremos una pagina HTML
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");     // ESta linea hace que la pagina se pueda abrir en cualquier navegador
            client.println("<link rel=\"icon\" href=\"data:,\">");                                                // Esta linea evita solicitudes en el favicon
            // Usaremos texto CSS para diseñar los botones 
            // Diseñamos los botones, color, letras , relleno y tamaño
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");                               // Aqui se encuantra el diseño para el boton cuando se encuentra en off ( es el mismo diseño solo con diferente color)                       
            
            // Encabezado de la pagina web
            client.println("<body><h1>ESP32 Web Server Control De Focos Equipo 5 Marco Antonio Chong Guerrero </h1>");
            
            // Aqui se muestra el boton 1 y su estado 
            client.println("<p>GPIO 2 - State " + output2State + "</p>");
            // Si el output26State esta off, el boton mostrara ON        
            if (output2State=="off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            client.println("</body></html>");
            
            // La respuesta termina con una linea en blanco
            client.println();
            // Terminamos el ciclo while
            break;
          } else { // Si tienes una nueva linea borra currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // Si se quiere agregar algo mas
          currentLine += c;      // agreguelo terminando esta linea
        }
      }
    }
    // Cerramos el encabezado
    header = "";
    // Cerramos la conexion
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
