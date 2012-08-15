#define BUILDING_NODE_EXTENSION

#include <node.h>
#include <v8.h>
#include "clipper.hpp"
#include "clipper.cpp"

using namespace node;
using namespace v8;
using namespace ClipperLib;

v8::Handle<Value> Method(const Arguments& args) {
  v8::HandleScope scope;

  // if (args[0]->IsArray()) {
    v8::Local<v8::Array> points = v8::Array::Cast(*args[0]);

    Polygons polyshape(1);
    Polygon polyout;

    // Populate the subject polygon
    for (int i = 0, limiti = points->Length(); i < limiti; i += 2) {
      // pair = v8::Array::New(2);
      // pair->set(0, points[0]); 
      v8::Local<v8::Value> pairA = points->Get(i);
      v8::Local<v8::Value> pairB = points->Get(i+1);
      polyshape[0].push_back(
        IntPoint(
          pairA->NumberValue(),
          pairB->NumberValue()
        )
      );
    }

    ClipperLib::OffsetPolygons(polyshape, polyshape, args[1]->NumberValue(), jtMiter, 3);

    if (polyshape.size() > 0) {
      Handle<Array> outPoints = Array::New(); //polyshape[0].size() * 2
      v8::Local<v8::Number> total = v8::Number::New(polyshape[0].size());

      for (Polygon::size_type k = 0; k < polyshape[0].size(); ++k) {
        IntPoint ip = polyshape[0][k];
        v8::Local<v8::Number> x = v8::Number::New(ip.X);
        v8::Local<v8::Number> y = v8::Number::New(ip.Y);
        outPoints->Set(outPoints->Length(), x);
        outPoints->Set(outPoints->Length(), y);
      }

      return scope.Close(outPoints); 
    }

  // }
  return scope.Close(v8::Undefined());
}

extern "C" {
  static void init(Handle<Object> target) {
    target->Set(String::NewSymbol("offset"),
      FunctionTemplate::New(Method)->GetFunction());
  }
  NODE_MODULE(clipper, init)
}