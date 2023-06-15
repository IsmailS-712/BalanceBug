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
const { randomgen, reset } = require("./backend/generation");
const pathing = require("./backend/pathing");
const parseInput = require("./backend/update");

var maze = [];
var discovery = [];
var timestamp;

app.get("/api/pathing", function (req, res) {
  try {
    const data = JSON.parse(req.body);
    var xpos = data.xpos;
    var ypos = data.discovery;
    var orientation = data.orientation;

    const payload = pathing(xpos, ypos, orientation, discovery);

    res.status(200).json(payload);
  } catch (error) {
    const errorType = error.constructor.name;
    res.status(500).json({ error: errorType });
  }
});

app.post("/api/mockupdate", function (req, res) {
  try {
    randomgen();
    res.status(200).json({
      status: "successfully mock updated maze",
    });
  } catch (error) {
    const errorType = error.constructor.name;
    res.status(500).json({ error: errorType });
  }
});

app.post("/api/reset", function (req, res) {
  try {
    reset();
    res.status(200).json({
      status: "successfully reset",
    });
  } catch (error) {
    const errorType = error.constructor.name;
    res.status(500).json({ error: errorType });
  }
});

app.post("/api/update", function (req, res) {
  try {
    const data = JSON.parse(req.body);
    var xpos = data.xpos;
    var ypos = data.ypos;
    var orientation = data.orientation;
    var lines = data.lines;

    parseInput(xpos, ypos, orientation, lines, maze, discovery);

    res.status(200).json({
      status: "successfully parsed data",
    });
  } catch (error) {
    const errorType = error.constructor.name;
    res.status(500).json({ error: errorType });
  }
});

app.post("/api/updatemaze", function (req, res) {
  // const client = new Client({
  //   connectionString: process.env.DATABASE_URL,
  //   ssl: { rejectUnauthorized: false },
  // });

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

  // console.log(maze);
  // console.log(discovery);
  // console.log(timestamp);
  try {
    const payload = JSON.parse(req.body);
    maze = payload.maze;
    discovery = payload.discovery;
    timestamp = payload.timestamp;
    res.status(200).json({
      status: "successfully updated maze and database",
    });
  } catch (error) {
    const errorType = error.constructor.name;
    res.status(500).json({ error: errorType });
  }
});

app.get("/api/displaymaze", function (req, res) {
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
