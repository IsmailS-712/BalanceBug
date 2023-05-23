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

var maze = [];

app.post("/updatemaze", function (req, res) {
  const payload = JSON.parse(req.body);
  maze = payload.maze
  res.status(200).json({
    status: "success",
  });
});

app.get("/displaymaze", function (req, res) {
  console.log(maze);
  res.status(200).json(maze);
});

// Server application
app.get("/*", function (req, res) {
  res.sendFile(path.join(__dirname, "build", "index.html"));
});

const port = process.env.PORT;
app.listen(port);
console.log("Server started on port " + port);
