'use strict';

var clipper= require('../build/Release/clipper');

var square= [ [ [0, 0], [0, 10], [10, 10], [10, 0] ] ];

/*
 * set debug level
 * messages goes directly to the console
 * clipper.setDebug(debugLevel);
 *
 * debugLevel: integer 0..4
 * 0:    no debug output
 * 1..3: more debug output
 * >=4:  all debug messages
 */
console.log('function "fixOrientation" with debugLevel 0:\n');
clipper.fixOrientation(square, 'integer');

console.log('\n\nfunction "fixOrientation" with debugLevel 3:\n');
clipper.setDebug(3);
clipper.fixOrientation(square, 'integer');

console.log('\n\nfunction "fixOrientation" with debugLevel 4:\n');
clipper.setDebug(4);
clipper.fixOrientation(square, 'integer');
