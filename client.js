const mqtt = require('mqtt');

const MQTT_BROKER_URL = 'mqtt://broker.hivemq.com:1883'; // HiveMQ public broker
const MQTT_TOPIC = 'Smart_Lighting_In_Hotel';
const MQTT_BASE_TOPIC = 'Smart_Lighting_In_Hotel'; // Base MQTT topic
const ALERT_BROKEN = 1; // THE LUMINOSITY  at which globe breaks

var room = 0;
var floor = 0;
var luminosity = 0;
var movement = true;
var manualOff = true;

function detectOff()
{
  var random = Math.floor((Math.random() * 40) / 40); // Generates a random integer between 0 and 1
  // if random = 1, light has NOT been switched off manually, therefore manualOff is false
  if(random == 1){return manualOff = false}
  else{ return manualOff = true};

}
function createLuminosity()
{    
  luminosity = Math.floor((Math.random() * 40) / 10); // Generates a random integer between 0 and 3
  if(movement == false){luminosity = 0};
  if(manualOff == true){luminosity = 0};
  return luminosity;
}

function createTimestamp()
{
  const nowTime = new Date();
  const randomMillis = Math.floor(Math.random() * 60 * 60 * 1000);
  const randomTime = new Date(nowTime - randomMillis);
  return randomTime;
}

function createRoom()
{    
  room++;
  if(room == 11){room = 1}; 
  return room;  
}

function createFloor()
{
  if(room ==1)(floor++);
  return floor;
}

function detectMovement()
{
  var random = Math.floor((Math.random() * 200) / 100);
  if (random == 1){ return movement = true;}
  else{ return movement = false}
}

 function createBroken()
 {
  if (luminosity == 0 && manualOff == false)
  {    
    if (movement == true){return true;}    
    else{return false;}    
  }
  return false;
 }

function generateData(NoLights) {
  const data = [];
  for (let i = 0; i < NoLights; i++) {
    const light_id = i;
    const room = createRoom();
    const floor = createFloor();
    const manualOff = detectOff();   
    const movement = detectMovement();
    const luminosity = createLuminosity();
    const broken = createBroken();
    const timestamp = createTimestamp();
    data.push({ light_id, room, floor, manualOff, movement, luminosity, broken, timestamp });
  }
  return data;
}

for (var i = 0; i<1000; i++){
const mqttClient = mqtt.connect(MQTT_BROKER_URL);

mqttClient.on('connect', () => {
  console.log('Connected to MQTT broker successfully');

  const randData = generateData(200);

  // Check for high-intensity lights and generate alerts
  for (const data of randData) {
    // Check for broken light globes and generate alerts
    if (data.luminosity < ALERT_BROKEN && data.broken == true) {
      console.log(`Alert: Light globe${data.light_id} in room ${data.room} on floor ${data.floor} is broken. Please replace light globe`);
    }
  }

  mqttClient.publish(MQTT_TOPIC, JSON.stringify(randData), (err) => {
    if (err) {
      console.error('Error publishing data: ', err);
    } else {
      console.log('Data successfully published to MQTT topic:', MQTT_TOPIC);
    }

    // Publish light intensity data to MQTT topics
    for (const data of randData) {
      const topic = `${MQTT_BASE_TOPIC}/${data.light_id}/${data.room}/${data.floor}/intensity`;
      mqttClient.publish(topic, data.luminosity.toString(), (err) => {
        if (err) {
          console.error('Error publishing data: ', err);
        } else {
          console.log(`Data successfully published to MQTT topic: ${topic}`);
        }
      });

      // Check for broken light globes and generate alerts
      if (data.luminosity < ALERT_BROKEN && data.broken == true) {
        console.log(`Alert: Light globe${data.light_id} in room ${data.room} on floor ${data.floor} is broken. Please replace light globe`);
      }
    }

    mqttClient.end();
  
  });
});
}