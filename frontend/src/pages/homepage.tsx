import React, { useState, useEffect } from "react";

export function HomePage() {
  const [payloadData, setPayloadData] = useState<any>(null);
  useEffect(() => {
    const intervalId = setInterval(() => {
      fetch("/displaymaze")
        .then((response) => response.json())
        .then((payloadData) => setPayloadData(payloadData))
        .catch((err) => console.log(err));
    }, 1000);
    return () => clearInterval(intervalId);
  }, []);

  const getCellStyle = (cellValue: any) => {
    let backgroundColor = "#ffffff";
    let color = "#000000";

    switch (cellValue) {
      case "#":
        backgroundColor = "#000000";
        break;
      case "S":
        backgroundColor = "#FFA500";
        break;
      case "E":
        backgroundColor = "#ff0000";
        break;
      case "P":
        backgroundColor = "#00FF00";
        break;
      default:
        break;
    }

    return {
      backgroundColor,
      color,
    };
  };

  return (
    <>
      <div className="flex flex-col md:w-[32rem] text-3xl md:text-4xl font-bold text-center mt-16 mb-7">
        Balance Bug Homepage
      </div>
      {payloadData && (
        <div className="mt-4">
          {/* Need to make this dynamically scale */}
          <div className={`grid grid-cols-7`}>
            {payloadData.map((row: any[], rowIndex: any) =>
              row.map((cell, colIndex) => (
                <div
                  key={`${rowIndex}-${colIndex}`}
                  style={getCellStyle(cell)}
                >
                  {cell}
                </div>
              ))
            )}
          </div>
        </div>
      )}
    </>
  );
}
