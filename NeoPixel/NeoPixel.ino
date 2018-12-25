#include <Adafruit_NeoPixel.h>

const int PIN       =  6;
const int NUMPIXELS = 239;
int       interval  =  5;

// Constructor: number of LEDs, pin number, LED type
Adafruit_NeoPixel pixels = Adafruit_NeoPixel( NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800 );

double phase = PI;

char input[6];
char rStr[3], gStr[3], bStr[3];

int r = 255,
    g = 0,
    b = 0;

//----------------------------------------
void setup()
{
  Serial.begin( 9600 );
  
  while( !Serial ){}// wait for serial port to connect. Needed for native USB port only
  
  pixels.begin();
}

//----------------------------------------
void loop()
{
  while( Serial.available() > 0 )
  {
    int numBytes = Serial.readBytesUntil(';', input, 6);

    if( numBytes == 6 )
    {
      rStr[0] = input[0];
      rStr[1] = input[1];
      rStr[2] = '\0';// null character
      
      gStr[0] = input[2];
      gStr[1] = input[3];
      gStr[2] = '\0';
      
      bStr[0] = input[4];
      bStr[1] = input[5];
      bStr[2] = '\0';
  
      r = strToHex( rStr );
      g = strToHex( gStr );
      b = strToHex( bStr );
      
      Serial.println( ( String )r + "," + ( String )g + "," + ( String )b );
//      phase = PI;
    }
  }

  float s = cos( phase ) * 0.5f + 0.5f;
  setColor( r * s, g * s, b * s );

  phase += 0.01f;
  delay(interval);
}

//----------------------------------------
void setColor( int _r, int _g, int _b )
{
  for( int i = 0; i < NUMPIXELS; ++i )
  {
    pixels.setPixelColor( i, pixels.Color( _r, _g, _b ) );
  }
  pixels.show();
}

//----------------------------------------
int strToHex( char str[] )
{
  return ( int )strtol( str, 0, 16 );
}
