import React, { useState, useEffect } from "react";
import { Line } from "react-chartjs-2";
import { Chart, registerables } from "chart.js";

Chart.register(...registerables);

function StreamData() {
  const [data, setData] = useState([]);
  const [timestamps, setTimestamps] = useState([]);

  useEffect(() => {
    const intervalId = setInterval(() => {
      fetch("/api/hr")
        .then((response) => response.json())
        .then((data) => setData(data))
        .catch((err) => console.log(err));

      fetch("/api/timestamps")
        .then((response) => response.json())
        .then((timestamps) => setTimestamps(timestamps))
        .catch((err) => console.log(err));
    }, 1000);
    return () => clearInterval(intervalId);
  }, []);

  const [chartData, setChartData] = useState({
    labels: [],
    datasets: [
      {
        label: "Heart rate",
        data: [],
        backgroundColor: "rgba(255, 99, 132, 0.2)",
        borderColor: "rgba(255, 99, 132, 1)",
        borderWidth: 1,
        pointRadius: 0,
        fill: false,
      },
    ],
  });

  useEffect(() => {
    if (data.length && timestamps.length) {
      setChartData({
        labels: timestamps,
        datasets: [
          {
            label: "Heart rate",
            data: data,
            backgroundColor: "rgba(255, 99, 132, 0.2)",
            borderColor: "rgba(255, 99, 132, 1)",
            borderWidth: 1,
            pointRadius: 0,
            fill: false,
          },
        ],
      });
    }
  }, [data, timestamps]);

  let currhr = data[data.length - 1];

  const options = {
    scales: {
      y: {
        suggestedMin: 30,
        suggestedMax: 150,
      },
    },
  };

  return (
    <>
      <div className="w-full text-6xl text-red-700 align-center text-center">
        {currhr}
      </div>
      <Line data={chartData} options={options} />
    </>
  );
}

export function HomePage() {
  return (
    <>
      <div className="flex flex-col md:w-[32rem] text-3xl md:text-4xl font-bold text-center mt-16 mb-7">
        Balance Bug Homepage
      </div>
      <StreamData />
    </>
  );
}
