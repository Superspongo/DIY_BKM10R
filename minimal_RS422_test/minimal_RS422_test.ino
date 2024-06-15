// Connect 'A' from Rx to 'A' from Tx module
// and     'B' from Rx to 'B' from Tx module

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  Serial1.setTX( 12 );
  Serial1.setRX( 13 );
  Serial1.begin(38400);
  delay( 1000 );
  
  while (Serial1.available()) Serial1.read();
}

void loop() 
{
  char cReadfromSerial;
  char cReadfromSerial1;
  
  // put your main code here, to run repeatedly:
  if ( Serial.available() )
  {
    cReadfromSerial = Serial.read();
    
    if ( !( cReadfromSerial == 's' ) )
    {
      Serial.write( cReadfromSerial );
      while( Serial.available() )Serial.write( Serial.read() );
    }
    else
    {
      // flush rest
      while( Serial.available() )Serial.read();
    }
  }

  if ( cReadfromSerial == 's' )
  {
    Serial.println("Received letter 's'");
    Serial.println("Printing 'c' to Serial1");
    Serial.println("");
    Serial1.print('c');
  }

  while ( Serial1.available() )
  {
    Serial.print("Received something: ");
    cReadfromSerial1 = Serial1.read();
    Serial.println( cReadfromSerial1 );
    Serial.println("");
    Serial.println("");
  }
}
