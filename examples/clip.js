'use strict';

var clipper= require('../build/Release/clipper');
var util= require('util');

var outerSubject= [ [10, 0], [100, 100], [0, 10], [0, 0] ];
var innerSubject= [ [3, 3], [3, 7], [7, 7], [7, 3] ];
var polySubject= [ outerSubject, innerSubject ];

var outerClip= [ [20, 0], [150, 100], [0, 50], [0, 0] ];
var innerClip= [ [13, 13], [13, 17], [17, 17], [17, 13] ];
var polyClip= [ outerClip, innerClip ];


/*
 * compute minimum polygon from a polyShape array
 * the resulting polygon is normally a very very small triangle
 * this algorithm cannot work for two exact centered rectangles
 * it wont work for small integer numbers too, double type is recommended
 * with successive approximation a polygon offset factor is generated
 * and the shrinked result is returned
 * this process is CPU intensive for complex polygons
 *
 * result= clipper.clip(polySubject, polyClip, numberType, clipType);
 *
 * polySubject: polygons as polyShape array
 * polyClip:    polygons as polyShape array
 * numberType:  'integer' || 'double'
 * clipType:    'ctIntersection' || 'ctUnion' || 'ctDifference' || 'ctXor'
 *
 * result:     polyShape with minimum polygon
 */
console.log('polygon subject:\n', polySubject);
console.log('\n\npolygon clip:\n', polyClip);
console.log('\n\nsolution:\n', util.inspect(clipper.clip(polySubject, polyClip, 'integer', 'ctIntersection'), false, 5));
