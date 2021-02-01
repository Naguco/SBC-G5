var express = require('express');
var path = require('path');
var serveStatic = require('serve-static');
var bodyParser = require('body-parser');
var multer = require('multer');
var mongoose = require('mongoose');
var latestVersionModel = require('./models/latestVersion_model');

mongoose.connect("mongodb+srv://OTAVersion:Prueba123456789@cluster0.v9ufb.mongodb.net/OTAVersion?retryWrites=true&w=majority", { useNewUrlParser: true, useUnifiedTopology: true })
    .then(() => {console.log("Conectado")})
    .catch((err) => { console.log(err); });

const upload = multer({
    dest: 'public/' // this saves your file into a directory called "uploads"
});

var fs = require('fs');

var app = express();

app.use(bodyParser.urlencoded({
    extended: true
  }));
app.use(bodyParser.json({}));

app.use(serveStatic(path.join(__dirname, 'public')));

app.get("/latestVersion", async (req, res) => {
    latestVersion = await latestVersionModel.getLatestVersion();
    res.status(200).send(String(latestVersion[0].latestVersion));
});

app.post("/newVersion", upload.single('file-to-upload'), async (req, res) => {
    let body = req.body;

    let numeroVersion = body.newVersion;
    let latestVersion;

    latestVersion = await latestVersionModel.getLatestVersion();
    if (latestVersion[0].latestVersion >= body.newVersion) {
        return res.status(403);
    }
    if (latestVersion[0].latestVersion)
        fs.unlinkSync('./public/' + latestVersion[0].latestVersion + '.bin');

    await latestVersionModel.newVersion(body.newVersion);

    fs.renameSync('./public/' + req.file.filename, './public/' + req.file.originalname);

    return res.status(200).send(body.newVersion);
});

app.get('/latestFileVersion.bin', async (req, res) => {
    latestVersion = await latestVersionModel.getLatestVersion();
    res.sendFile(__dirname + '/public/' + latestVersion[0].latestVersion + '.bin');
});

app.get('/', (req, res) => {
    res.status(200).send("OTA NODE");
});

app.listen(process.env.PORT || 3000);