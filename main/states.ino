
/*  -----------------------------------------------------------------------
    -----------------------------------------------------------------------
    --------------------- ESTADO IRRIGATE ---------------------------------
    -----------------------------------------------------------------------
    -----------------------------------------------------------------------
*/

void irrigateState() {
  encenderValidoVerde();
  Serial.println("--------------------------------------");
  Serial.print("Estado Irrigate: Abrimos el riego, nos dormimos 10 min \n y volvemos a comprobar si hay que regar\n");

  while (humedadTierra <= 30) {
    Serial.print("Estado Irrigate: \n");

    pinMode(GPIO_NUM_18, OUTPUT);
    digitalWrite(18, HIGH);
    /*rtc_gpio_init(GPIO_NUM_18);
      rtc_gpio_set_direction(GPIO_NUM_18, RTC_GPIO_MODE_OUTPUT_ONLY);
      rtc_gpio_set_level(GPIO_NUM_18,1);
      esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);*/

    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * us_to_seconds );
    Serial.flush();
    Serial.println("Seguimos regando");
    esp_light_sleep_start();

    humedadTierra = readMoisture();
    Serial.print("Humedad de la Tierra:"); Serial.print(humedadTierra); Serial.println("%");
  }

  state = wifiOn;
}

/*
   ------------------------------------------------------------------------
*/

/*  -----------------------------------------------------------------------
    -----------------------------------------------------------------------
    --------------------- ESTADO WIFI ON ----------------------------------
    -----------------------------------------------------------------------
    -----------------------------------------------------------------------
*/

void wifiOnState() {
  encenderValidoVerde();
  Serial.println("--------------------------------------");
  Serial.print("Estado con wifi: Activamos el wifi y mandamos los datos a thingsboard.\n");

  //Encendemos el wifi y mandamos los datos a Thingsboard
  wifiInit();
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  mqttSetup();
  mqttReconnect();
  delay(1000);

  //Parte reservada para enviar datos
  publishData("temperatura_ambiental", temperatura);
  delay(500);
  publishData("humedad_ambiental", humedad);
  delay(500);
  publishData("humedad_tierra", humedadTierra);
  delay(500);
  publishData("nivel_agua", Deposito);
  delay(500);
  //publishData("regando", True);
  //delay(500);
  state = lowPowerMode;
}

/*
   ------------------------------------------------------------------------
*/

/*  -----------------------------------------------------------------------
   -----------------------------------------------------------------------
   --------------------- ESTADO WIFI Off ---------------------------------
   -----------------------------------------------------------------------
   -----------------------------------------------------------------------
*/

void wifiOffState() {
  encenderValidoVerde();
  Serial.println("--------------------------------------");
  Serial.print("Estado sin wifi: Leemos los sensores y actualizamos los actuadores.\n");

  //Parte reservada para leer sensores
  temperatura = leerTemperatura();
  Serial.print("Temperatura:"); Serial.println(temperatura);
  humedad = leerHumedad();
  Serial.print("Humedad Ambiental:"); Serial.println(humedad);
  humedadTierra = readMoisture();
  Serial.print("Humedad de la Tierra:"); Serial.print(humedadTierra); Serial.println("%");
  distancia = readDistance();
  Serial.println(distancia);
  Deposito = 100 - (distancia / 10) * 100 ;
  Serial.print("Nivel Agua:"); Serial.print(Deposito); Serial.println("%");

  //Decidimos si regamos o no
  if (humedadTierra <= 20) {
    Serial.println("El nivel de Humedad de la tierra es menor al 20%. Es necesario regar.");
    state = irrigate;
  }
  else {
    Serial.println("El nivel de Humedad de la tierra es mayor al 20%. No es necesario regar.");
    if (hayWifi) {
      state = wifiOn;
    } else {
      state = toExcel;
    }
  }
  //Activamos el codigo de error del deposito si es necesario
  if (Deposito <= 30) {
    depositoBajo = true;
  }
}

/*
   ------------------------------------------------------------------------
*/

/*  ----------------------------------------------------------------------
   -----------------------------------------------------------------------
   --------------------- ESTADO Low Power --------------------------------
   -----------------------------------------------------------------------
   -----------------------------------------------------------------------
*/

void lowPowerModeState() {
  encenderValidoVerde();
  Serial.println("--------------------------------------");
  Serial.print("Estado low power: Apagamos todo y nos vamos a dormir\n");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  btStop();
  esp_wifi_stop();
  esp_bt_controller_disable();
  state = wifiOff;

  if (depositoBajo == true) {
    int i;
    Serial.println("Deposito con poca agua");
    for (i = 0; i < 10; i++) {
      apagarLEDs();
      delay(1000);
      encenderErrorDepositoMorado();
      delay(1000);
      ESP.restart();
    }
  }

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * us_to_seconds);
  Serial.flush();
  esp_deep_sleep_start();
}

/*
   ------------------------------------------------------------------------
*/

/*  ----------------------------------------------------------------------
   -----------------------------------------------------------------------
   --------------------- ESTADO toExcel --------------------------------
   -----------------------------------------------------------------------
   -----------------------------------------------------------------------
*/

void toExcelState() {
  Serial.println("Aquí tendría que escribir en el .csv");
  delay(8000);
  state = lowPowerMode;
}
