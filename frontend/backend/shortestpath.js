function shortestPath(maze, startX, startY, endX, endY) {
  const queue = [{ x: startX, y: startY }];
  const visited = new Set();
  const path = [];

  while (queue.length > 0) {
    const { x, y } = queue.shift();

    if (visited.has(`${x},${y}`)) {
      continue;
    }

    visited.add(`${x},${y}`);
    path.push({ x, y });

    if (x === endX && y === endY) {
      return path;
    }

    const neighbors = [
      { x: x - 1, y },
      { x: x + 1, y },
      { x, y: y - 1 },
      { x, y: y + 1 },
    ];

    for (const neighbor of neighbors) {
      const { x: nx, y: ny } = neighbor;

      if (
        nx >= 0 &&
        nx < maze.length &&
        ny >= 0 &&
        ny < maze[0].length &&
        maze[nx][ny] <= 50 &&
        !visited.has(`${nx},${ny}`)
      ) {
        queue.push({ x: nx, y: ny });
      }
    }
  }

  return []; // Return an empty array if no path is found
}

module.exports = shortestPath;
