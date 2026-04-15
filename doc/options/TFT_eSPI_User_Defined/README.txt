You need to modify the User_Setup.h file according to your board and copy it into the TFT_eSPI library folder.
In the program's config.h file, select CYD_USER_DEFINED as the hardware and modify the CYD_USER_DEFINED.h file according to your board.



Tiene que modificar el archivo User_Setup.h de acuerdo a su placa y copiarlo en la carpeta de la biblioteca TFT_eSPI.
En el archivo config.h del programa seleccionar CYD_USER_DEFINED como hardware y modifique el archivo CYD_USER_DEFINED.h de acuerdo a su placa.



Heu de modificar el fitxer User_Setup.h segons la vostra placa i copiar-lo a la carpeta de la biblioteca TFT_eSPI.
Al fitxer config.h del programa, seleccioneu CYD_USER_DEFINED com a hardware i modifiqueu el fitxer CYD_USER_DEFINED.h segons la vostra placa.


Sie müssen die Datei User_Setup.h entsprechend Ihrer Platine ändern und in den TFT_eSPI-Bibliotheksordner kopieren.
Wählen Sie in der Datei config.h des Programms CYD_USER_DEFINED als Hardware aus und passen Sie die Datei CYD_USER_DEFINED.h entsprechend Ihrem Board an.




config.h


////////////////////////////////////////////////////////////
// ***** USER OPTIONS *****
////////////////////////////////////////////////////////////

// Seleccione la version hardware del CYD (Cheap Yellow Display)   - Select the hardware version of CYD (Cheap Yellow Display):   
// Use el archivo User_Setup.h correcto para la libreria TFT_eSPI - Use the correct User_Setup.h file for library TFT_eSPI

#define CYD_HW_VERSION                CYD_USER_DEFINED



