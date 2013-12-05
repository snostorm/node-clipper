'use strict';

var clipper= require('../build/Release/clipper');

var counterclockwise= [ [10, 0], [10, 10], [0, 10], [0, 0] ];
var clockwise= [ [3, 3], [7, 7], [3, 7], [7, 3] ];
var polyShape= [ counterclockwise, clockwise ];

/*
 * fix orientation of polygons in a polyShape array
 * first element (outer border): orientation must be true
 * all other elements (inner borders): orientation false
 *
 * result= clipper.fixOrientation(polyShape, numberType);
 *
 * polyShape:  polygons as polyShape array
 * numberType: 'integer' || 'double'
 *
 * result:     polyShape with fixed orientations
 */
console.log('original polygon:\n', polyShape);
console.log('\n\norientation of inner and outer borders fixed:\n', clipper.fixOrientation(polyShape, 'integer'));
