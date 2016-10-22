#include <SPI.h>  // библиотека для работы SPI интерфейса
#include "RF24.h" // библиотека для работы с wifi модулем RF24

// Если будете отвечать, просьба тратить не больше 2-3 минут на ответ, если не знаете пропускайте

// Что такое интерфейс?
// Для чего он служит
// Приведите примеры интерфейсов из жизни
// Ну или можете продолжать читать код дальше

#define CLK 11 // что это делает? (я то знаю, а вы?)
#define DIO 10 // что это делает? (смотри выше)

byte addresses[][6] = {"1Node","2Node"}; // мы создаем массив с 2мя точками доступа, одна главная (называется мастер или Master) другая ведомая (называется ведомый или slave)

bool radioNumber = 0; // установим номер радио, он у нас будет либо 1 либо 0, почему?

// 7,8 для UNO
// 9,53 для MEGA
RF24 radio(9,53); // что это делает?

bool role = 0; // роль либо master либо slave

// так выглядит объект
struct REQUEST{
  int data1;
  int data2;
  int data3;
}; 

struct REQUEST req; // объект который мы будем пересылать

void setup() {

  Serial.begin(115200); // ого какая скорость обмена \(O.O)/
  radio.begin();
  // ужасная настройка
  radio.setPALevel(RF24_PA_LOW);
  if(radioNumber == 1){
    radio.openWritingPipe(addresses[1]); // чему равен addresses[1] ?
    radio.openReadingPipe(1,addresses[0]); // число слева от 1 до 6ти
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  radio.startListening(); // Начинаем прослушивать 
}




void loop() {

// Тот кто отправляет  master
if (role == 1)  {
    
    radio.stopListening();
    
     // (@datasending) ----------------- ВНИМАНИЕ! Нам интересна эта часть!
     
     // например: 
     // req.data2 = 12;
     
     // (@datasending end) ----------------- между этими двумя комментариями вы можете менять состояние объекта, который отправляете
     
     if (!radio.write( &req, sizeof(req) )){ // отправляем данные, если они не отправились выводим failed
       Serial.println(F("failed"));
     }
     
    radio.startListening(); // снова начинаем слушать
    
    // (@waiting) этот блок нужен нам для ожидания ответа от другого устройства
    unsigned long started_waiting_at = micros();
    boolean timeout = false;
    
    while ( ! radio.available() ){
      if (micros() - started_waiting_at > 200000 ){
          timeout = true;
          break;
      }      
    }
        
    if ( timeout ){
        Serial.println(F("Failed, response timed out."));
    }else{
        radio.read( &req, sizeof(req) ); // читаем что нам ответило другое устройство
      // (@readpart) читаем ответ от устройства
      
      // например 
      // Serial.print(req.data2); // выведем что нибудь из ответа
      // req.data2 = req.data2 + 1; // прибавим к нему единичку
      
      // (@readpart end) между этими комментариями пишем код реакции на ответ от slave
    }
    // (@waiting end) конец блока ожидания
    delay(500);
  }

  // Тот кто принимает slave
  if ( role == 0 )
  {
    
    if( radio.available()){
      
      while (radio.available()) {
        radio.read( &req, sizeof(req) );
      }
      
      // (@datagot) мы считали объект и можем что нибудь делать с ним, например преобразовать и отправить обратно
      
      // например 
      // Serial.print(req.data2); // выведем что нибудь из ответа
      // req.data2 = req.data2 + 1; // прибавим к нему единичку
      
      // (@datagot end) между этими комментариями можем писать код
      
      radio.stopListening();
      radio.write( &req, sizeof(req) );     
      radio.startListening();
      
   }
 }

}
