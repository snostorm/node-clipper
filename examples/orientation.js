'use strict';

var clipper= require('../build/Release/clipper');

var squareOuter= [ [10, 0], [10, 10], [0, 10], [0, 0] ];
var squareInner= [ [3, 3], [3, 7], [7, 7], [7, 3] ];
var polyShape= [ squareOuter, squareInner ];

/*
 * get orientation of all polygons in a polyShape array
 *
 * clipper.orientation(polyShape, numberType);
 *
 * polyShape:  polygons as polyShape array
 * numberType: 'integer' || 'double'
 *
 * result:     array of boolean, true: counterclockwise, false: clockwise
 */
console.log(clipper.orientation(polyShape, 'integer'));
