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

  return (
    <>
      <div className="flex flex-col md:w-[32rem] text-3xl md:text-4xl font-bold text-center mt-16 mb-7">
        Balance Bug Homepage
      </div>
      {payloadData && (
        <div className="mt-4">
          <h3>Received Payload:</h3>
          <code>{JSON.stringify(payloadData, null, 2)}</code>
        </div>
      )}
    </>
  );
}
