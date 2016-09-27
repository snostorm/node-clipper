# node-clipper

A node.js wrapper for the [Clipper polygon clipper](http://www.angusj.com/delphi/clipper.php) (and helper) library

## Project State

The wrapper exposes and extends functions of clipper library to node.js. The function "minimum" is an outstanding feature to compute the most inner point of complex polygons with inner and outer borders. It is very useful to get the perfect point for a placemark inside the polygon.

## Installation

```bash
npm install --save rl3-clipper
```

## Build from source

Clone the project and build the project
```bash
node-gyp configure
node-gyp build
```

## Data structures

### Value

A value is a javascript number. In clipper library all numbers are integer. So if floating point numbers are used it will be converted in the function call. Mandantory the functions have to be called with number type "integer" | "double". Double values should not be greater than ~1000 because of the upscaling to an integer.

### Point

A point is an array of two values:
```javascript
var p= [ v1, v2 ];
```

### Polygon

A polygon is an array of Points:
```javascript
var poly= [ [ v1, v2 ], [ v3, v4 ], [ v5, v6 ], [ v7, v8 ] ];
```

### Polyshape

The polyShape structure is an array of polygons. The first element in the array is the outer border polygon (points of this polygon counterclockwise oriented) followed by any number of inner border polygons with orientation clockwise:
```javascript
var polyShape= [
  [ [ v1, v2 ], [ v3, v4 ], [ v5, v6 ], [ v7, v8 ] ],
  [ [ v9, v10 ], [ v11, v12 ], [ v13, v14 ], [ v15, v16 ] ]
];
```


## Bindings

### setDebug

Sets the debug level in the module. Default is debug level 0.
Example:
```javascript
'use strict';

var clipper= require('../build/Release/clipper');

var square= [ [ [0, 0], [0, 10], [10, 10], [10, 0] ] ];

/*
 * set debug level
 * messages goes directly to the console
 *
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
```


### orientation

Get orientation of all polygons in a polyShape array as array of boolean.
Example:
```javascript
'use strict';

var clipper= require('../build/Release/clipper');

var squareOuter= [ [10, 0], [10, 10], [0, 10], [0, 0] ];
var squareInner= [ [3, 3], [3, 7], [7, 7], [7, 3] ];
var polyShape= [ squareOuter, squareInner ];

/*
 * get orientation of all polygons in a polyShape array
 *
 * result= clipper.orientation(polyShape, numberType);
 *
 * polyShape:  polygons as polyShape array
 * numberType: 'integer' || 'double'
 *
 * result:     array of boolean, true: counterclockwise, false: clockwise
 */
console.log(clipper.orientation(polyShape, 'integer'));
```


### offset

See the [clipper documentation for offset](http://www.angusj.com/delphi/clipper/documentation/Docs/Units/ClipperLib/Functions/OffsetPaths.htm) in order to get an idea of how this is used.

```javascript
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
```


### minimum

Generate minimum polygon from polyShape. Read comments in the example to see, how it works.

```javascript
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
```


### clip

Clip two polygons. The result may produce multiple polygons. See also the [clipper documentation for clip](http://www.angusj.com/delphi/clipper/documentation/Docs/Units/ClipperLib/Types/ClipType.htm).

```javascript
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
 * clip two polygons
 * multiple types of clipping are available
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
```


### clean

Clean polygon. See also the [clipper documentation for clean](http://www.angusj.com/delphi/clipper/documentation/Docs/Units/ClipperLib/Functions/CleanPolygons.htm).

```javascript
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
```


### simplify

Simplify polygon. See also the [clipper documentation for simplify](http://www.angusj.com/delphi/clipper/documentation/Docs/Units/ClipperLib/Functions/SimplifyPolygons.htm).

```javascript
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
```


### fixOrientation

Fix orientation of all polygons an a polyShape array.

```javascript
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
```
