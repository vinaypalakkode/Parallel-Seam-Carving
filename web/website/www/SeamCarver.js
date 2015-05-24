self.addEventListener("message", function(e){
	var data = e.data;
	var seamCarver = new SeamCarver(data.image).initSeams();

	var result = {
	    image: seamCarver.resize(data.adjust),
	    "action": "done"
	}
	self.postMessage(result);
    }, false);

var log = (log ||
	   function(msg){
	       self.postMessage(msg);
	   });


/**
 * Main seam carver object
 * @param {Object} image
 */
function SeamCarver(image){
    log("Seam Carver initialized");
    this.image = image;
    this.newImage = [];
}

/**
 * Gets individual pixels
 * @param {Object} x
 * @param {Object} y
 */
SeamCarver.prototype.getPixel = function(x, y){
    var base = (y * this.image.width + x) * 4;
    return {
        "red": this.image.data[base + 0],
	"green": this.image.data[base + 1],
	"blue": this.image.data[base + 2],
	"alpha": this.image.data[base + 3]
    };
}

SeamCarver.prototype.putPixel = function(x, y, color){
    var rgba = color;
    if (typeof color === "number") {
	rgba = {
	    "red": (color & 0xFF0000) >> 16,
	    "green": (color & 0x00FF00) >> 8,
	    "blue": (color & 0x0000FF),
	    "alpha": 255
	}
    }
    var base = (y * this.image.width + x) * 4;
    this.newImage[base + 0] = rgba["red"];
    this.newImage[base + 1] = rgba["green"];
    this.newImage[base + 2] = rgba["blue"];
    this.newImage[base + 3] = rgba["alpha"];
    return this;
};

SeamCarver.prototype.copyImage = function(){
    this.newImage = [];
    for (var x = 0; x < this.image.width; x++)
        for (var y = 0; y < this.image.height; y++)
            this.putPixel(x, y, this.getPixel(x, y));
}

SeamCarver.prototype.initEnergyMap = function(){
    log("Starting to calculate energymap");
    var me = this;
    var b = function(x, y){
	if (x < 0 || y < 0 || x >= me.image.width || y >= me.image.height) {
	    return 0;
	}
	var pixel = me.getPixel(x, y);
	return (pixel["red"] + pixel["green"] + pixel["blue"]);
    }

    var energyMap = [];
    var max = 0;
    for (var x = 0; x < this.image.width; x++) {
	energyMap[x] = [];
	for (var y = 0; y < this.image.height; y++) {
	    var xenergy = b(x - 1, y - 1) + 2 * b(x - 1, y) + b(x - 1, y + 1) - b(x + 1, y - 1) - 2 * b(x + 1, y) - b(x + 1, y + 1);
	    var yenergy = b(x - 1, y - 1) + 2 * b(x, y - 1) + b(x + 1, y - 1) - b(x - 1, y + 1) - 2 * b(x, y + 1) - b(x + 1, y + 1);
	    energyMap[x][y] = Math.sqrt(xenergy * xenergy + yenergy * yenergy);
	    max = (max > energyMap[x][y] ? max : energyMap[x][y]);
	}
    }
    log("EnergyMap calculation complete");
    this.energyMap = energyMap;
    this.maxEnergy = max;
    return this;
}

SeamCarver.prototype.initSeams = function(){
    log("Starting to calculate seams");
    var yseam = [];
    this.energyMap || this.initEnergyMap();

    var ylen = this.image.height - 1;
    // initialize the last row of the seams
    for (var x = 0; x < this.image.width; x++) {
	yseam[x] = [];
	yseam[x][ylen] = x;
    }

    // sort the last row of the seams
    for (var i = 0; i < yseam.length; i++) {
	for (var j = i + 1; j < yseam.length; j++) {
	    if (this.energyMap[yseam[i][ylen]][ylen] > this.energyMap[yseam[j][ylen]][ylen]) {
		var tmp = yseam[j];
                yseam[j] = yseam[i]
		yseam[i] = tmp;
	    }
	}
    }

    // get the other rows of the seams
    for (var x = 0; x < yseam.length; x++) {
	for (var y = ylen - 1; y >= 0; y--) {
	    var x1 = yseam[x][y + 1];
	    var x0 = x1 - 1;
	    // Move along till the adjacent pixel is not a part of another seam
	    while (x0 >= 0) {
		if (!isNaN(this.energyMap[x0][y]))
		    break;
		x0--;
	    }

	    var x2 = x1 + 1;
	    while (x2 < this.image.width) {
		if (!isNaN(this.energyMap[x2][y]))
		    break;
		x2++;
	    }

	    var hx0 = this.energyMap[x0] ? this.energyMap[x0][y] : Number.MAX_VALUE;
	    var hx1 = this.energyMap[x1][y] || Number.MAX_VALUE;
	    var hx2 = this.energyMap[x2] ? this.energyMap[x2][y] : Number.MAX_VALUE;

	    // Choose the least energy
	    yseam[x][y] = hx0 < hx1 ? (hx0 < hx2 ? x0 : x2) : (hx1 < hx2 ? x1 : x2);
	    this.energyMap[yseam[x][y]][y] = NaN;
	}
    }

    log("Seams calculated");
    this.seams = yseam;
    return this;
}

SeamCarver.prototype.getEnergyMap = function(){
    this.energyMap || this.initEnergyMap();
    this.copyImage();
    for (var x = 0; x < this.image.width; x++) {
	for (var y = 0; y < this.image.height; y++) {
	    var color = parseInt(this.energyMap[x][y] / this.maxEnergy * 255);
	    this.putPixel(x, y, {
		    "red": color,
			"blue": color,
			"green": color,
			"alpha": 1
			});
	}
    }
    return this.newImage;
}

SeamCarver.prototype.getSeams = function(){
    this.seams || this.initSeams();
    this.copyImage();
    var color = 255;
    var step = 1;//parseInt(color / this.image.width);
    for (var x = 0; x < this.seams.length; x++) {
	for (var y = 0; y < this.image.height; y++) {
	    this.putPixel(this.seams[x][y], y, parseInt("0x" + color + color + color, 16));
	}
	color = (color - step < 0) ? 255 : color - step;
    }

    return this.newImage;
}

SeamCarver.prototype.resize = function(dim){
    var image = this.image;
    this.seams || this.initSeams();
    log("Starting resize Reduce");
    this.newImage = [];
    var widthDiff = image.width - dim.width;
    log("Removing Seams");
    for (var y = 0; y < image.height; y++) {
	var x1 = 0; // x counter of the new image
	for (var x = 0; x < image.width; x++) {
	    this.putPixel(x, y, this.getPixel(x, y));
	    var isSkippable = false;
	    for (var i = 0; i < widthDiff; i++) {
		if (this.seams[i][y] == x) {
		    isSkippable = true;
		    break;
		}
	    }
	    if (isSkippable == false) {
		this.putPixel(x1, y, this.getPixel(x, y));
		x1++;
	    }
	}
    }
    log("Seams removed, placing empty pixels in reduced portion of image");
    for (var x = dim.width; x < image.width; x++) {
	for (var y = 0; y < image.height; y++) {
	    this.putPixel(x, y, 0xFFFFFF);
	}
    }
    log("Reduced image ready");
    return {
	"height": this.image.height,
	    "width": this.image.width,
	    "data": this.newImage
	    }
}
