import React from "react";
import ImageRenderer from "./imagerenderer";

interface DiscoveryMapProps {
  arrays: number[][];
}

export function DiscoveryMap({ arrays }: DiscoveryMapProps) {
  const maxValue = Math.max(...arrays.flat());

  function calculateColor(num: number) {
    const startColor = [105, 210, 231]; // RGB values for #69D2E7
    const endColor = [250, 105, 0]; // RGB values for #FA6900
  
    const r = Math.round(startColor[0] + (endColor[0] - startColor[0]) * (num / 100));
    const g = Math.round(startColor[1] + (endColor[1] - startColor[1]) * (num / 100));
    const b = Math.round(startColor[2] + (endColor[2] - startColor[2]) * (num / 100));
  
    return `rgb(${r}, ${g}, ${b})`;
  }
  

  const pixels = arrays.map((arr) => arr.map((num) => calculateColor(num)));

  return (
    <div className="flex flex-col mt-5">
      <ImageRenderer pixels={pixels} />
    </div>
  );
}
