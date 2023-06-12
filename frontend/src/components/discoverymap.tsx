export function DiscoveryMap({ arrays }: { arrays: any }) {
  const maxValue = Math.max(...arrays.flat());

  function calculateColor(num: number) {
    const min = 0;
    const max = 100;
  
    const normalizedNum = (num - min) / (max - min);
    const blue = Math.round(255 - normalizedNum * 255);
    const red = Math.round(normalizedNum * 255);
    const green = Math.round((blue + red) / 2);
    const color = red.toString(16).padStart(2, "0") + green.toString(16).padStart(2, "0") + blue.toString(16).padStart(2, "0");
  
    return `#${color}`;
  }
  

  return (
    <div className="flex flex-col mt-5">
      {arrays.map((arr: any, index: any) => (
        <div className="flex" key={index}>
          {arr.map((num: any, idx: any) => (
            <div
              className="flex justify-center items-center w-[2px] h-[2px]"
              key={idx}
              style={{ backgroundColor: calculateColor(num) }}
            >
              {/* <span>{num}</span> */}
            </div>
          ))}
        </div>
      ))}
    </div>
  );
}