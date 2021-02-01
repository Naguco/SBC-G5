const latestVersionSchema = require('./schemas/latestVersion_schema');

module.exports.newVersion = async function(newVersion) {

    let idDelDocumento;

    await latestVersionSchema.find().exec(async (err, res) => {
        if (res.length == 0) {
            new latestVersionSchema({
                latestVersion: newVersion
            }).save();
            return;
        } else {
            idDelDocumento = res[0]._id;
            await latestVersionSchema.updateOne({ _id: res[0]._id }, { latestVersion: newVersion });
        }
    });
    return;
};

module.exports.getLatestVersion = function() {
    return latestVersionSchema.find().exec();
};