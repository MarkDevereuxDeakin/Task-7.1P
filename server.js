const mqtt = require('mqtt');
const mongoose = require('mongoose');

const MQTT_BROKER_URL = 'mqtt://broker.hivemq.com:1883';
const MQTT_TOPIC = 'Smart_Lighting_In_Hotel';

const MONGO_URI = 'mongodb+srv://<username>:<password>@sit314.pluuj6f.mongodb.net/?retryWrites=true&w=majority';
const ALERT_BROKEN = 1; // Set your desired threshold for light intensity

const MQTTClient = mqtt.connect(MQTT_BROKER_URL);

const smartLightingInHotelSchema = new mongoose.Schema({
  light_id: Number,
  room: Number,
  floor: Number,
  manualOff: Boolean,
  movement: Boolean,
  luminosity: Number,
  broken: Boolean,
  timestamp: Date,
});

const Lighting = mongoose.model('Smart-Lighting', smartLightingInHotelSchema);

MQTTClient.on('connect', () => {
  console.log('Connected to MQTT broker');

  MQTTClient.subscribe(MQTT_TOPIC, (err) => {
    if (err) {
      console.error('MQTT subscription UNSUCCESSFUL :', err);
    } else {
      console.log('Subscription to MQTT topic :', MQTT_TOPIC, '  SUCCESSFUL');
    }
  });
});

MQTTClient.on('message', (topic, message) => {
  console.log('Message Received from :', topic);
  console.log('Message: ', message.toString());
  console.log('\n');

  const lightData = JSON.parse(message.toString());

  mongoose.connect(MONGO_URI, {
    useNewUrlParser: true,
    useUnifiedTopology: true,
  });

  const db = mongoose.connection;

  console.log('\n');
  db.on('error', (err) => {
    console.error('Error connecting to MongoDB:', err);
  });

  db.once('open', () => {
    console.log('Connected to MongoDB');

    // Inserting the data into the 'lightings' collection
    Lighting.insertMany(lightData)
      .then((result) => {
        console.log(`Successfully Inserted ${result.length} rows of data into Smart-Lighting collection`);
        // Check for high-intensity lights and generate alerts
        for (const data of lightData) {
          // Check for broken light globes and generate alerts
        if (data.luminosity < ALERT_BROKEN && data.broken == true) {
        console.log(`Alert: Light globe${data.light_id} in room ${data.room} on floor ${data.floor} is broken. Please replace light globe`);
      }
        }

        // Close the MongoDB connection
        mongoose.connection.close();
      })
      .catch((err) => {
        console.error('Error inserting data into MongoDB:', err);
        // Close the MongoDB connection
        mongoose.connection.close();
      });
  });
});
