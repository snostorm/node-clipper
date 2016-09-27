'use strict';

var clipper= require('../build/Release/clipper');

var squareOuter= [ [10, 0], [10, 10], [0, 10], [0, 0] ];
var squareInner= [ [3, 3], [3, 7], [7, 7], [7, 3] ];
var polyShape= [ squareOuter, squareInner ];

/*
 * offset polygons in a polyShape array.
 * positive offset: outer borders are expanded, inner borders are shrinked
 * negative offset: outer borders are shrinked, inner borders are expanded
 *
 * result= clipper.offset(polyShape, numberType, offset);
 *
 * polyShape:  polygons as polyShape array
 * numberType: 'integer' || 'double'
 * offset:     number
 *
 * result:     polyShape with offset
 */
console.log('original polygon:\n', polyShape);
console.log('\n\nshrinked, offset= -1:\n', clipper.offset(polyShape, 'integer', -1));
console.log('\n\nexpanded, offset= 2, inner border disappears:\n', clipper.offset(polyShape, 'integer', 2));
