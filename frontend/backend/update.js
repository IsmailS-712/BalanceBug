const fetch = require("node-fetch");

// Maze mapping
// 0 - 100 Status of the maze (0: space, 100: wall)
// -1 - undiscovered
// 200 - current position of the rover
// 300 - path

// Discovery
// 0 - 100 Amount of discovery

function parseInput(xpos, ypos, orientation, lines, maze, discovery) {
  const width = 23;
  const height = 11;
  const offset = 2;

  const angle_rad = (orientation * Math.PI) / 180;
  const cos_angle = Math.cos(angle_rad);
  const sin_angle = Math.sin(angle_rad);

  const x_start = xpos;
  const y_start = ypos;

  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++) {
      const x_offset = x - Math.floor(width / 2);
      const y_offset = y - Math.floor(height / 2);

      const rotated_x = Math.round(cos_angle * x_offset - sin_angle * y_offset);
      const rotated_y = Math.round(sin_angle * x_offset + cos_angle * y_offset);

      const maze_x = x_start + rotated_x;
      const maze_y = y_start + rotated_y;

      const distance = Math.sqrt(rotated_x ** 2 + rotated_y ** 2);

      if (
        maze_x >= 0 &&
        maze_x < maze[0].length &&
        maze_y >= 0 &&
        maze_y < maze.length
      ) {
        maze[maze_y][maze_x] = 100;
      }
    }
  }

  fill(maze);

  const payload = {
    maze: maze,
    discovery: discovery,
    timestamp: new Date().toISOString(),
  };

  fetch("http://localhost:3000/api/updatemaze", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(payload),
  }).catch((error) => {
    console.error("Error:", error);
  });
}

function fill(maze) {
  for (let i = 0; i < 240; i++) {
    for (let j = 0; j < 360; j++) {
      if (
        i > 0 &&
        i < 240 - 1 &&
        maze[i + 1][j] === maze[i - 1][j] &&
        j > 0 &&
        maze[i][j - 1] === maze[i - 1][j] &&
        j < 360 - 1 &&
        maze[i][j + 1] === maze[i - 1][j]
      ) {
        maze[i][j] = maze[i - 1][j];
      }
    }
  }
}

module.exports = parseInput;
