// server setup
const express = require("express");
const path = require("path");
const bodyParser = require("body-parser");
require("dotenv").config();

const app = express();
var options = {
  inflate: true,
  limit: "4000kb",
  type: "application/json",
};
app.use(bodyParser.raw(options));
app.use(express.static(path.join(__dirname, "build")));

const { Client } = require("pg");

var maze = [];
var discovery = [];
var timestamp;

// function fillMaze() {
//   const maze = [];

//   for (let i = 0; i < 240; i++) {
//     const row = [];
//     for (let j = 0; j < 360; j++) {
//       const randomValue = Math.floor(Math.random() * 100); // Generate random number between 0 and 99
//       row.push(randomValue);
//     }
//     maze.push(row);
//   }

//   return maze;
// }

// const maze = fillMaze();

app.post("/updatemaze", function (req, res) {
  const payload = JSON.parse(req.body);
  maze = payload.maze;
  discovery = payload.discovery
  timestamp = payload.timestamp;

  const client = new Client({
    connectionString: process.env.DATABASE_URL,
    ssl: { rejectUnauthorized: false },
  });

  //   client
  //     .connect()
  //     .then(() => {
  //       const query = "INSERT INTO maze_run (layout, timestamp) VALUES ($1, $2)";
  //       const values = [maze, currentTimestamp];

  //       return client.query(query, values);
  //     })
  //     .then(() => {
  //       res.status(200).json({
  //         status: "successfully updated maze and database",
  //       });
  //     })
  //     .catch((error) => {
  //       console.error("Error inserting row:", error);
  //       res.status(500).send(error.message);
  //     });
  //   client.end();
  res.status(200).json({
    status: "successfully updated maze and database",
  });
});

app.get("/displaymaze", function (req, res) {
  res.status(200).json({
    maze: maze,
    discovery: discovery,
    time: timestamp,
  });
});

// Server application
app.get("/*", function (req, res) {
  res.sendFile(path.join(__dirname, "build", "index.html"));
});

const port = process.env.PORT;
app.listen(port);
console.log("Server started on port " + port);
