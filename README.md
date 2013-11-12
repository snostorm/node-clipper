# node-clipper

A node.js wrapper for the [Clipper polygon clipper](http://www.angusj.com/delphi/clipper.php) (and helper) library

## Project State

Please note that this very light wrapper was built for an internal project, so almost
no effort has gone on to documentation of 1-to-1 binding of functions we haven't needed.

But we would be interested in expanding it over time and will happily accept pull requests
to expand the bindings.

## Bindings

### Offset

See the [clipper documentation for Offset](http://www.angusj.com/delphi/clipper/documentation/Docs/Units/ClipperLib/Functions/OffsetPaths.htm))
in order to get an idea of how this is used.

```javascript
clipper = require('../build/Release/clipper')

/*a
  ## Offset
  * arguments: flattened_coordinate_path, offset
  * where: offset can be positive or negative (negative will go inside the polygon, positive will expand outside)
*/
flattened_coordinate_path = [0, 0, 0, 10, 10, 10, 10, 0]

// A negative offset gets an offset inside our Polygon:
inner_highlight_path = clipper.offset(flattened_coordinate_path, -2)
console.log(inner_highlight_path)
// returns [ [ 2, 2, 8, 2, 8, 8, 2, 8 ] ]

// A positive offset gets an offset outside our Polygon:
outter_highlight_path = clipper.offset(flattened_coordinate_path, 2)
console.log(outter_highlight_path)
// returns [ [ -2, 12, -2, -2, 12, -2, 12, 12 ] ]
```
