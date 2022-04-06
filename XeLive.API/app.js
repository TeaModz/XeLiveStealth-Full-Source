const express = require("express")
const https = require("https");
const http = require('http');
const fs = require("fs");

const app = express()

app.get("/get-manifest/:file", (req, res) => {
    var IP = req.socket.remoteAddress

    console.log("get-manifest Requested by " + IP)

    var path = __dirname + "/public/manifest" + "/" + req.params.file;
    fs.access(path, fs.F_OK, (err) => {
        res.sendFile(__dirname + "/public/manifest" + "/" + req.params.file);
    });
})

app.get("/get-manifest-images/:file", (req, res) => {
    var IP = req.socket.remoteAddress

    console.log("get-manifest-images Requested by " + IP)

    var path = __dirname + "/public/manifest/images" + "/" + req.params.file;
    fs.access(path, fs.F_OK, (err) => {
        res.sendFile(__dirname + "/public/manifest/images" + "/" + req.params.file);
    });
})

app.get("/get-cheat-engine/:file", (req, res) => {
    var IP = req.socket.remoteAddress

    console.log("/get-cheat-engine Requested by " + IP)

    var path = __dirname + "/public/cheat-engine" + "/" + req.params.file;
    fs.access(path, fs.F_OK, (err) => {
        res.sendFile(__dirname + "/public/cheat-engine" + "/" + req.params.file);
    });
})

var options = {
    key: fs.readFileSync('./key.pem'),
    cert: fs.readFileSync('./cert.pem')
};

https.createServer(options, app).listen(443); //Https
http.createServer(app).listen(1337); //http