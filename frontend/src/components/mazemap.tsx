import React from "react";
import ImageRenderer from "./imagerenderer";

interface MazeMapProps {
  arrays: number[][];
}

export function MazeMap({ arrays }: MazeMapProps) {
  const maxValue = Math.max(...arrays.flat());

  function calculateColor(num: number) {
    const min = 0;
    const max = 100;

    const normalizedNum = (num - min) / (max - min);
    const shade = Math.round(255 - normalizedNum * 255);
    const color = shade.toString(16).padStart(2, "0");

    return `#${color}${color}${color}`;
  }

  const pixels = arrays.map((arr) => arr.map((num) => calculateColor(num)));

  return (
    <div className="flex flex-col mt-5">
      <ImageRenderer pixels={pixels} />
    </div>
  );
}
