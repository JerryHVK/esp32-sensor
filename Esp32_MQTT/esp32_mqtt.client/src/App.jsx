import { useState, useEffect } from 'react';
import './App.css';
import mqtt from 'mqtt';

function App() {
    const [heartRate, setHeartRate] = useState();

    useEffect(() => {
        const client = mqtt.connect("wss://broker.emqx.io:8084/mqtt");

        client.on("connect", () => {
            client.subscribe("khang/server", (err) => {
                if (err) {
                    console.log("Subscribe failed");
                }
            });
        });

        client.on("message", (topic, payload) => {
            setHeartRate(payload.toString());
        });

        return () => {
            client.end();
        };
    }, []);

    const saveDB = (heartRate) => {
        const requestOptions = {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ heartRate: `${heartRate}` })
        };
        fetch('data', requestOptions);
    }

    const toggleLight = () => {
        fetch("esp32/toggleLight");
    }

    return (
        <div>
            <p>Your Heart Rate: {heartRate}</p>
            <div>
                <button onClick={() => saveDB(heartRate)}>
                    Save data
                </button>
            </div>
            <div>
                <button type="button" onClick={() => toggleLight()}>
                    Toggle light
                </button>
                {/*or*/}
                {/*<button type="button" onClick={toggleLight}>*/}
                {/*    Toggle light*/}
                {/*</button>*/}
            </div>
        </div>
    );
}

export default App;