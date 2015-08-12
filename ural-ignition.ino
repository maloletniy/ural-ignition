/**
 * Ардуино получет сигналы с оптического датчика. Датчик срабатывает 2 раза за оборот.
 * 
 * Вход в область датчика - переключение с 0 на 1. 
 * Измеряется время между двумя входами и по этим показаниям 
 * вычисляемся кол во оборотов в минуту. 60сек / 2 / Время оборота.
 * 
 * По оборотам выбирается угол опережения по зашитой карте и формируется фронт 
 * зажигания с учетом того, что катушку нужно зарядить перед искрой (олокол 1мс)
 * 
 * Время угла вычисляется как время одного градуса угла т.е 180 градусов / время пол оборота 
 * умноженое на УОЗ 
 * 
 * Каждые 5 сек выводится статистика в консоль.
 * 
 *   
 */


#define IGNITION_PIN 5
#define LED_PIN 13
#define COIL_PIN 4
#define COIL_CHARGE_TIME 1

float angleArray[7] = {6.5,28.2,39.3,43.9,45.5,45,45};

//times for a half round
long time = 0;
long prevTime = 0;

bool val = false;
bool prevVal = false;

int rpm  = 0;
float angle = 0;

long statTimer = 0;

void setup()
{
	pinMode(IGNITION_PIN, INPUT);
	pinMode(LED_PIN, OUTPUT);
	
	digitalWrite(COIL_PIN,LOW);
	digitalWrite(LED_PIN,LOW);
	
	Serial.begin(9600);
	
	Serial.println("Ignition: ready to work");
}


void loop()
{
	val  = digitalRead(IGNITION_PIN) == HIGH;

	//first time when sensor detects 1
	if(val && prevVal != val)
	{
		//get time of a round (rpm)
		time = millis();

		rpm = 30000 / (time - prevTime);

		//check that it's not first iteration
		if(prevTime > 0)
		{
			//detect angle and angle delay time;
			int index = rpm/1000;
			angle = angleArray[index];
			
			//time of selected angle
			//calculated as a time of one degree on selected rpms * angle value from map
			float angleTime = angle * (time - prevTime) / 180;
			float ignitionTime = COIL_CHARGE_TIME+angleTime;
			
			//calculate delay before VMT (currently not correct)
			
			//power on the coil
			digitalWrite(LED_PIN,HIGH);
			digitalWrite(COIL_PIN,HIGH);
			//wait until coil charges and set up angle time;
			delay(ignitionTime);
			//power of
			digitalWrite(COIL_PIN,LOW);
			digitalWrite(LED_PIN,LOW);
		}


		prevTime = time;
		

		//Collect and print some statistics
		if(millis() - statTimer > 1000)
		{
			statTimer = millis();
			Serial.print(" rpm=");
			Serial.print(rpm);
			Serial.print(" angle=");
			Serial.print(angle);
			Serial.println();
			
		}

	}

	prevVal = val;

}

