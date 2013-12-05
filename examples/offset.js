clipper = require('../build/Release/clipper')

/*a
  ## Offset
  * arguments: flattened_coordinate_path, offset
  * where: offset can be positive or negative (negative will go inside the polygon, positive will expand outside)
*/
flattened_coordinate_path = [[[0, 0], [0, 10], [10, 10], [10, 0]]]

// A negative offset gets an offset inside our Polygon:
inner_highlight_path = clipper.offset(flattened_coordinate_path, 'integer', -2)
console.log(inner_highlight_path)
// returns [ [ 2, 2, 8, 2, 8, 8, 2, 8 ] ]

// A positive offset gets an offset outside our Polygon:
outter_highlight_path = clipper.offset(flattened_coordinate_path, 'integer', 2)
console.log(outter_highlight_path)
// returns [ [ -2, 12, -2, -2, 12, -2, 12, 12 ] ]