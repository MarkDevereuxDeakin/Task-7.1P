const mongoose = require('mongoose');

module.exports = mongoose.model('lights', new mongoose.Schema({
    light_id: Number,
    room: Number,    
    floor: Number,
    manualOff: Boolean,
    movement: Boolean,
    luminosity: Number,
    broken, Boolean,
    timestamp: Date
}));