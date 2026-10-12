# DaemonHID

**DaemonHID** es un proyecto educativo de hardware y firmware basado en el **Arduino UNO R4 WiFi**. Su objetivo es explorar la interacción entre hardware físico, conectividad inalámbrica local y funciones de entrada USB, con un enfoque en el aprendizaje, la seguridad y las pruebas controladas.

> **Estado:** etapa inicial de desarrollo. Las funciones descritas como futuras todavía deben implementarse y validarse.

## Objetivos

- Estudiar las capacidades y limitaciones del Arduino UNO R4 WiFi.
- Desarrollar una interfaz web local mediante el punto de acceso Wi-Fi de la placa.
- Investigar la compatibilidad de la placa con funciones USB HID y validar primero una prueba mínima.
- Documentar conexiones, decisiones de diseño y resultados reproducibles.
- Incorporar controles físicos y límites claros para que las funciones de entrada se activen de forma intencional.

## Hardware

- Arduino UNO R4 WiFi.
- Cable USB de datos compatible.
- Componentes adicionales del kit de inicio, según cada prueba.

La disponibilidad de funciones HID depende del hardware, del core de Arduino y de las bibliotecas compatibles. No se debe asumir que una función está soportada hasta comprobarla en la placa real.

## Estructura del repositorio

```text
DaemonHID/
├── README.md
├── .gitignore
├── firmware/
│   ├── usb_hid/
│   └── wireless/
├── hardware/
├── docs/
└── tests/
```

Las carpetas se incorporarán conforme se necesiten; Git no conserva directorios vacíos.

## Plan de desarrollo

1. **Preparación:** documentar el proyecto y mantener el repositorio ordenado.
2. **Validación USB:** comprobar qué capacidades USB HID admite la placa y realizar una prueba local, visible e iniciada mediante una acción física.
3. **Entrada controlada:** implementar una demostración limitada y documentada, con activación explícita.
4. **Interfaz inalámbrica:** desarrollar y probar la interfaz web local de manera independiente.
5. **Integración:** definir cómo se comunican las funciones y validar los límites de seguridad.
6. **Documentación:** registrar configuración, resultados, limitaciones y pasos para reproducir las pruebas.

## Principios de seguridad

- Probar únicamente en equipos propios o con autorización explícita.
- No ejecutar acciones de entrada ocultas, persistentes o no consentidas.
- No recopilar ni transmitir credenciales o datos privados.
- Mantener las pruebas acotadas, visibles y fáciles de detener.
- No exponer la interfaz de administración a redes externas sin un diseño de seguridad revisado.

## Desarrollo

El firmware se desarrollará con el Arduino IDE. Antes de añadir bibliotecas o fijar una arquitectura definitiva, se verificará la compatibilidad con el Arduino UNO R4 WiFi y se documentarán las versiones utilizadas.

## Estado actual

- [x] Repositorio Git inicializado.
- [x] README inicial.
- [ ] Validación de USB HID en la placa.
- [ ] Primera prueba de firmware documentada.
- [ ] Interfaz web local documentada.
- [ ] Pruebas y esquemas de hardware.

## Licencia

La licencia del proyecto está pendiente de definir.
