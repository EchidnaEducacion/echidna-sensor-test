# Test de Sensores - Placa Robótica Educativa

Programa de test secuencial para probar todos los sensores de una placa de robótica educativa basada en Arduino Uno.

## Descripción

Este programa permite realizar pruebas automáticas y secuenciales de todos los sensores de la placa en dos modos de funcionamiento diferentes:

- **Modo Normal**: Prueba los sensores estándar de la placa
- **Modo MkMk**: Prueba los pines configurables en modo MkMk

El usuario controla el flujo del test mediante el pulsador SR (derecho) y puede ver los valores de los sensores en tiempo real a través del Serial Monitor.

## Hardware Requerido

- Arduino Uno
- Placa de robótica educativa con los siguientes componentes:

### Sensores - Modo Normal

| Pin | Sensor |
|-----|--------|
| D2 | SR - Pulsador derecho |
| D3 | SL - Pulsador izquierdo |
| A0 | Joystick - Eje X (analógico) |
| A1 | Joystick - Eje Y (analógico) |
| A4, A5 | Acelerómetro LIS3DH (I2C) |
| A3 | LDR - Sensor de luz (analógico) |
| A6 | Sensor de temperatura (analógico) |
| A7 | Micrófono (analógico) |

### Pines - Modo MkMk

- A0, A1, A2, A3, A6, A7 (pines analógicos)
- D2, D3 (pines digitales)

## Librerías Necesarias

Instala las siguientes librerías desde el Gestor de Librerías del Arduino IDE:

1. **Adafruit LIS3DH** - Driver para el acelerómetro LIS3DH
2. **Adafruit Unified Sensor** - Librería de sensores unificados (dependencia)
3. **Wire** - Comunicación I2C (incluida por defecto)

### Instalación de Librerías

1. Abre Arduino IDE
2. Ve a `Herramientas` → `Administrar Bibliotecas...`
3. Busca "Adafruit LIS3DH" e instala
4. Busca "Adafruit Unified Sensor" e instala

## Instalación

1. Clona o descarga este repositorio
2. Abre el archivo `echidna-sensor-test.ino` en Arduino IDE
3. Instala las librerías necesarias (ver sección anterior)
4. Conecta tu Arduino Uno al ordenador
5. Selecciona la placa: `Herramientas` → `Placa` → `Arduino Uno`
6. Selecciona el puerto correcto: `Herramientas` → `Puerto`
7. Sube el sketch: `Programa` → `Subir`

## Uso

1. **Abre el Serial Monitor**
   - Ve a `Herramientas` → `Monitor Serie`
   - Configura la velocidad a **9600 baudios**

2. **Inicia el Test de Modo Normal**
   - El programa mostrará un mensaje de bienvenida
   - Pulsa **SR (pulsador derecho)** para comenzar

3. **Prueba cada sensor**
   - El programa mostrará el nombre del sensor a probar
   - Pulsa **SR** para iniciar la lectura del sensor
   - Observa los valores en tiempo real (actualización cada 0.5 segundos)
   - Pulsa **SR** nuevamente para pasar al siguiente sensor

4. **Cambia al Modo MkMk**
   - Cuando termine el Modo Normal, el programa pedirá cambiar al Modo MkMk
   - **Cambia el interruptor físico** de la placa a modo MkMk
   - Pulsa **SR** para continuar

5. **Prueba los pines en Modo MkMk**
   - Repite el proceso para cada pin (A0-A7, D2, D3)

6. **Finalización**
   - Al completar todos los tests, verás un mensaje de confirmación
   - Para reiniciar, presiona el botón **RESET** del Arduino

## Estructura del Código

```
echidna-sensor-test.ino
├── setup()                    - Inicialización
├── loop()                     - Máquina de estados principal
├── iniciarModoNormal()        - Inicio del test Modo Normal
├── ejecutarTestModoNormal()   - Ejecución del test Modo Normal
├── iniciarModoMkMk()          - Inicio del test Modo MkMk
├── ejecutarTestModoMkMk()     - Ejecución del test Modo MkMk
├── finalizarTest()            - Mensaje de finalización
├── leerSensorNormal()         - Lectura de sensores en Modo Normal
├── leerSensorMkMk()           - Lectura de pines en Modo MkMk
├── leerPulsadorSR()           - Detección de pulsación con debounce
└── esperarPulsacionSR()       - Espera bloqueante de pulsación
```

## Características Técnicas

- **Frecuencia de actualización**: 500ms (0.5 segundos)
- **Debounce de pulsadores**: 250ms
- **Comunicación serial**: 9600 baudios
- **Rango del acelerómetro**: ±4g
- **Dirección I2C del LIS3DH**: 0x18 (configurable a 0x19)

## Solución de Problemas

### El acelerómetro no se detecta

Si ves el error "No se pudo inicializar el LIS3DH":

1. Verifica las conexiones I2C (pines A4 y A5)
2. Prueba cambiar la dirección I2C en la línea 76:
   ```cpp
   if (!lis.begin(0x19)) {  // Cambia 0x18 por 0x19
   ```

### Los pulsadores no responden

- Los pulsadores usan `INPUT_PULLUP`, por lo que se activan con nivel bajo (LOW)
- Verifica que estén correctamente conectados a D2 y D3
- El debounce está configurado a 250ms

### Los valores analógicos son erráticos

- Verifica las conexiones de los sensores analógicos
- Algunos sensores pueden necesitar calibración
- Los valores van de 0 a 1023 (10 bits)

### No aparece nada en el Serial Monitor

- Verifica que la velocidad esté configurada a 9600 baudios
- Asegúrate de que el Arduino esté conectado al puerto correcto
- Intenta cerrar y volver a abrir el Serial Monitor

## Valores Esperados

### Sensores Analógicos (0-1023)
- **Joystick X/Y**: ~512 en reposo, 0-1023 al mover
- **LDR**: Mayor valor = más luz
- **Temperatura**: Varía según el sensor (leer datasheet)
- **Micrófono**: ~512 en silencio, varía con el sonido

### Sensores Digitales
- **SR/SL**: "PULSADO" o "NO PULSADO"
- **D2/D3 (MkMk)**: "LOW (0)" o "HIGH (1)"

### Acelerómetro LIS3DH
- **En reposo**: X≈0, Y≈0, Z≈9.8 m/s² (gravedad)
- **Al inclinar**: Valores cambian según la orientación

## Licencia

Este proyecto es de código abierto y está disponible para uso educativo.

## Autor

Programa de test para placa de robótica educativa basada en Arduino Uno.

## Versión

1.0.0 - Versión inicial con soporte para Modo Normal y Modo MkMk
