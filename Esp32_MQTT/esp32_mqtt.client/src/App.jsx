import { useEffect, useState } from 'react';
import './App.css';

function App() {
    const [heartRate, setHeartRate] = useState();

    useEffect(() => { fetchingHeartRate(); }, [])

    async function fetchingHeartRate() {
        const response = await fetch('data');
        const data = await response.json();
        setHeartRate(data);
    }

    const contents = heartRate === undefined
        ? <p>Put your finger on the device to measure your heart rate</p>
        : <div><p>Your heart rate {heartRate.heartRate}</p></div>


    return (
        <div>
            <h1>Heart Rate</h1>
            <p>This component demonstrates fetching data from the server.</p>
            {contents }
        </div>
    );

}

export default App;

