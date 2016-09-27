'use strict';

var clipper= require('../build/Release/clipper');

var squareOuter= [ [10, 0], [100, 100], [0, 10], [0, 0] ];
var squareInner= [ [3, 3], [3, 7], [7, 7], [7, 3] ];
var polyShape= [ squareOuter, squareInner ];

/*
 * compute minimum polygon from a polyShape array
 * the resulting polygon is normally a very very small triangle
 * this algorithm cannot work for two exact centered rectangles
 * it wont work for small integer numbers too, double type is recommended
 * with successive approximation a polygon offset factor is generated
 * and the shrinked result is returned
 * this process is CPU intensive for complex polygons
 *
 * result= clipper.minimum(polyShape, numberType, unused, joinType, miterLimit);
 *
 * polyShape:  polygons as polyShape array
 * numberType: 'integer' || 'double'
 * unused:     unused
 * joinType:   'jtMiter' || 'jtSquare' || 'jtRound'
 * miterLimit: limit of iterations for offset calculation in clipper lib
 *
 * result:     polyShape with minimum polygon
 */
console.log('original polygon:\n', polyShape);
console.log('\n\nminimum polygon:\n', clipper.minimum(polyShape, 'integer', 0, 'jtMiter', 10));
