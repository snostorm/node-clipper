'use strict';

var clipper= require('../build/Release/clipper');

var outer= [ [10, 0], [100, 100], [101, 100], [100, 100], [0, 10], [0, 0] ];
var inner= [ [3, 3], [3, 7], [7, 6], [7, 3] ];
var polyShape= [ outer, inner ];

/*
 * simplify polygon
 *
 * result= clipper.simplify(polyShape, numberType);
 *
 * polyShape:  polygons as polyShape array
 * numberType: 'integer' || 'double'
 *
 * result:     polyShape with simplified polygon
 */
console.log('original polygon:\n', polyShape);
console.log('\n\nsimplified polygon:\n', clipper.simplify(polyShape, 'integer'));
