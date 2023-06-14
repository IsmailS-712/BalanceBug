import React from 'react';
import ImageRenderer from './imagerenderer';

interface DiscoveryMapProps {
  arrays: number[][];
}

export function DiscoveryMap({ arrays }: DiscoveryMapProps) {
  const maxValue = Math.max(...arrays.flat());

  function calculateColor(num: number) {
    const min = 0;
    const max = 100;

    const normalizedNum = (num - min) / (max - min);
    const blue = Math.round(255 - normalizedNum * 255);
    const red = Math.round(normalizedNum * 255);
    const green = Math.round((blue + red) / 2);
    const color = red.toString(16).padStart(2, '0') + green.toString(16).padStart(2, '0') + blue.toString(16).padStart(2, '0');

    return `#${color}`;
  }

  const pixels = arrays.map((arr) =>
    arr.map((num) => calculateColor(num))
  );

  return (
    <div className="flex flex-col mt-5">
      <ImageRenderer pixels={pixels} />
    </div>
  );
}
