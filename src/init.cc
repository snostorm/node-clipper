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

  if (args[0]->IsArray()) {
    try {
      v8::Local<v8::Array> points = v8::Array::Cast(*args[0]);

      ClipperLib::Polygons polyshape(1), polydone;

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

      bool wasReversed = false;
      if (!ClipperLib::Orientation(polyshape[0])) {
        ClipperLib::ReversePolygons(polyshape[0]);
        wasReversed = true;
      }

      double miterLimit = 3.0;

      ClipperLib::OffsetPolygons(polyshape, polydone, args[1]->NumberValue(), jtMiter, miterLimit);

      if (!wasReversed && ClipperLib::Orientation(polydone[0])) {
        ClipperLib::ReversePolygons(polydone);
      }

      if (polydone.size() > 0) {
        Handle<Array> outPolygons = Array::New();
        for (Polygon::size_type g = 0; g < polydone.size(); ++g) {
          Handle<Array> outPoints = Array::New();
          for (Polygon::size_type k = 0; k < polydone[g].size(); ++k) {
            IntPoint ip = polydone[g][k];
            v8::Local<v8::Number> x = v8::Number::New(ip.X);
            v8::Local<v8::Number> y = v8::Number::New(ip.Y);
            outPoints->Set(outPoints->Length(), x);
            outPoints->Set(outPoints->Length(), y);
          }
          outPolygons->Set(g, outPoints);
        }
        return scope.Close(outPolygons); 
      }
    } catch (...) {
      return scope.Close(v8::Boolean::New(false));
    }
  }
  return scope.Close(v8::Undefined());
}

extern "C" {
  static void init(Handle<Object> target) {
    target->Set(String::NewSymbol("offset"),
      FunctionTemplate::New(Method)->GetFunction());
  }
  NODE_MODULE(clipper, init)
}