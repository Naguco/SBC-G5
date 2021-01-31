var mongoose = require('mongoose');

let Schema = mongoose.Schema;

let latestVersionSchema = new Schema({
    latestVersion: Number
});

module.exports = mongoose.model('LatestVersionSchema', latestVersionSchema);