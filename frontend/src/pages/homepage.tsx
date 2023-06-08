import React, { useState, useEffect, useCallback } from "react";
import { DiscoveryMap } from "../components/discoverymap";
import { MazeMap } from "../components/mazemap";

export function HomePage() {
  const [mazemap, setMaze] = useState<any>(null);
  const [discoverymap, setDiscovery] = useState<any>(null);
  const [lastTime, setLastTime] = useState(new Date());
  const [currentDateTime, setCurrentDateTime] = useState(new Date());

  const fetchData = useCallback(() => {
    fetch("/displaymaze")
      .then((response) => response.json())
      .then((data) => {
        const { maze, discovery, time } = data;
        if (
          JSON.stringify(maze) !== JSON.stringify(mazemap) ||
          JSON.stringify(discovery) !== JSON.stringify(discoverymap)
        ) {
          setMaze(maze);
          setDiscovery(discovery);
          setLastTime(time);
        }
      })
      .catch((error) => {
        console.log("Error fetching data:", error);
      });
  }, []);

  useEffect(() => {
    const intervalId = setInterval(() => {
      setCurrentDateTime(new Date());
      fetchData();
    }, 1000);
    return () => clearInterval(intervalId);
  }, [fetchData]);

  return (
    <>
      <div className="flex flex-col md:w-[32rem] text-3xl md:text-4xl font-bold text-center mt-16 mb-7">
        Balance Bug Homepage
      </div>
      <h1>Current Date and Time: {currentDateTime.toLocaleString()}</h1>
      {lastTime && mazemap && discoverymap ? (
        <>
          <div>Last Updated Time: {new Date(lastTime).toLocaleString()}</div>
          <div className="flex flex-row mt-4 w-full">
            <div className="flex flex-col w-full items-center">
              <div className="text-lg font-bold">Maze Mapping</div>
              <MazeMap arrays={mazemap} />
            </div>
            <div className="flex flex-col w-full items-center">
              <div className="text-lg font-bold">Discovery Progress</div>
              <DiscoveryMap arrays={discoverymap} />
            </div>
          </div>
        </>
      ) : (
        <h1>No Update</h1>
      )}
    </>
  );
}
