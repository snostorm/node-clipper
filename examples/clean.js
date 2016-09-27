'use strict';

var clipper= require('../build/Release/clipper');

var outer= [ [10, 0], [100, 100], [0, 10], [0, 0] ];
var inner= [ [3, 3], [3, 7], [7, 6], [7, 3] ];
var polyShape= [ outer, inner ];

/*
 * clean polygon
 *
 * result= clipper.clean(polyShape, numberType, distance);
 *
 * polyShape:  polygons as polyShape array
 * numberType: 'integer' || 'double'
 * distance:   distance of neighbour vertices
 *
 * result:     polyShape with minimum polygon
 */
console.log('original polygon:\n', polyShape);
console.log('\n\ncleaned polygon:\n', clipper.clean(polyShape, 'integer', 3));
