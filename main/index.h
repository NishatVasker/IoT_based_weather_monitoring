<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Sensor Dashboard</title>
    <script>
    function fetchData() {
        fetch("/gas")
            .then(response => response.json())
            .then(data => {
                document.getElementById("gasValue").innerText = data.value + " ppm";
            })
            .catch(error => console.error('Error fetching data: ', error));
    }

    setInterval(fetchData, 1000); // Fetch data every 1000 milliseconds
    </script>
</head>
<body>
    <h1>Sensor Data</h1>
    <p>Gas Concentration: <span id="gasValue">Loading...</span></p>
</body>
</html>
