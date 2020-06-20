var express = require('express');
var app = express();
var server = require("http").Server(app);
var io = require("socket.io")(server);
var net = require('net');
var store = require('store');
const cors = require('cors')
const bodyParser = require('body-parser')

app.use(bodyParser.urlencoded({ extended: true }))
app.use(cors())

var netServer = net.createServer(function (c) {
    console.log('CONNECTED: ' + c.remoteAddress + ':' + c.remotePort);

    c.on('data', function (data) {
        console.log('DATA ' + c.remoteAddress + ': ' + data);
        io.emit('potentiometer', data);
        c.write(data);
    });

    c.on('end', function () {
        console.log('client disconnected');
    });

    c.pipe(c);
});

// main service listing to any service connection on port 3000
netServer.listen(3000);

app.get('/', function (req, res) {
    res.sendFile(__dirname + '/index.html');
});

app.get('/api/light', (req, res) => {
    res.status(200).json(store.get('light'));
});

app.use(express.static(__dirname + '/static'));

io.on('connection', function (socket) {
    socket.on('light', function(msg) {
        store.set('light', msg);
    });
});


server.listen(3001);