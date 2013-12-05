#define DEBUG

#include <iostream>

#include <node.h>
#include <v8.h>
#include "clipper.hpp"
#include "clipper.cpp"

using namespace node;
using namespace v8;
using namespace ClipperLib;
const unsigned long doubleFactor = 0x4000000000000;
int debug = 0;


v8::Handle<Value> setDebug(const Arguments& args) {
    v8::HandleScope scope;

    if (args.Length() > 0 && args[0]->IsNumber()) {
        debug = args[0]->NumberValue();
    }

    return scope.Close(v8::Number::New(debug));
}



Polygons v8ArrayToPolygons(v8::Handle<Array> inOutPolygons, bool doubleType) {
    int len = inOutPolygons->Length();
    Polygons polyshape(len);
    #ifdef DEBUG
    if (debug > 1) std::cout << "polygonArray: length: " << len << std::endl;
    #endif

    for (int i = 0; i < len; i++) {
        v8::Local<v8::Array> polyLine = v8::Local<v8::Array>::Cast(inOutPolygons->Get(i));
        #ifdef DEBUG
        if (debug > 2) std::cout << "polyLine: length: " << polyLine->Length() << std::endl;
        #endif
        for (unsigned int j = 0; j < polyLine->Length(); j++) {
            v8::Local<v8::Array> point = v8::Local<v8::Array>::Cast(polyLine->Get(j));
            v8::Local<v8::Value> x = point->Get(0);
            v8::Local<v8::Value> y = point->Get(1);
            IntPoint p;
            if (doubleType) {
                p = IntPoint(
                    x->NumberValue() * doubleFactor,
                    y->NumberValue() * doubleFactor
                );
            } else {
                p = IntPoint(
                    x->NumberValue(),
                    y->NumberValue()
                );
            }
            polyshape[i].push_back(p);
            #ifdef DEBUG
            if (debug > 3) std::cout << "polyLine: point: " << p.X << " " << p.Y << std::endl;
            #endif
        }
    }
    return polyshape;
}


long signedSum(long a, long b) {
    if ((a < 0 && b < 0) || (a > 0 && b > 0) || a == 0 || b == 0) {
        return a + b;
    }
    return a - b;
}


v8::Handle<Array> polygonsToV8Array(Polygons polygons, bool doubleType) {
    v8::Handle<v8::Array> result = v8::Array::New();
    for (unsigned int i = 0; i < polygons.size(); i++) {
        v8::Handle<v8::Array> points = v8::Array::New();
        for (unsigned int k = 0; k < polygons[i].size(); k++) {
        IntPoint ip = polygons[i][k];
            v8::Local<v8::Number> x;
            v8::Local<v8::Number> y;
            if (doubleType) {
                x = v8::Number::New((double)ip.X / (double)doubleFactor);
                y = v8::Number::New((double)ip.Y / (double)doubleFactor);
            } else {
                x = v8::Number::New(ip.X);
                y = v8::Number::New(ip.Y);
            }
            v8::Handle<v8::Array> point = v8::Array::New();
            point->Set(point->Length(), x);
            point->Set(point->Length(), y);
            points->Set(points->Length(), point);
        }
        result->Set(result->Length(), points);
    }
    return result;
}


void doFixOrientation(Polygons &polyshape) {
    if (!ClipperLib::Orientation(polyshape[0])) {
        ClipperLib::ReversePolygon(polyshape[0]);
        #ifdef DEBUG
        if (debug > 0) std::cout << "doFixOrientation: outerPoints reversed" << std::endl;
        #endif
    }

    for (unsigned int i = 1; i < polyshape.size(); i++) {
        if (ClipperLib::Orientation(polyshape[i])) {
            ClipperLib::ReversePolygon(polyshape[i]);
            #ifdef DEBUG
            if (debug > 0) std::cout << "doFixOrientation: innerPoints reversed: " << i - 1 << std::endl;
            #endif
        }
    }
}


v8::Local<String> checkOffsetArguments(const Arguments& args, int checkLength) {
    v8::Local<String> result = String::New("");

    /* check args for wrong function call
     * args[0]: array of outerPoints and innerPoints arrays
     * args[1]: pointType integer || double -> IntPoint = doubleFactor * double
     * args[2]: Polygon shrink value (negative -> shrink; positive -> expand)
     * args[3]: optional: Jointype (jtMiter, jtSquare or jtRound)
     * args[4]: optional: double MiterLimit
     */
    if (args.Length() < 2) {
        result = String::New("Too few arguments! At least 'polyshape[][][]' and 'pointType' are required!");
        return result;
    }
    if (args.Length() < checkLength) {
        result = String::New("Too few arguments!");
        return result;
    }

    if (!args[0]->IsArray()) {
        result = String::Concat(String::New("Wrong argument 'polyshape': array[shapes][points][point] required: "), args[0]->ToString());
        return result;
    }

    if (checkLength < 2) {
        return result;
    }

    if (!args[1]->IsString() || !(args[1]->Equals(String::New("double")) || args[1]->Equals(String::New("integer")))) {
        result = String::Concat(String::New("Wrong argument 'pointType': 'double' || 'integer' required: "), args[1]->ToString());
        return result;
    }

    if ((args.Length() > 2) && (checkLength > 2)) {
        if (!args[2]->IsNumber()) {
            result = String::Concat(String::New("Wrong argument 'delta' || 'distance': number required: "), args[2]->ToString());
            return result;
        }
    }

    if ((args.Length() > 3) && (checkLength > 3)) {
        if (!args[3]->IsString() || !(args[3]->Equals(String::New("jtMiter")) || args[3]->Equals(String::New("jtSquare")) || args[3]->Equals(String::New("jtRound")))) {
            result = String::Concat(String::New("Wrong argument 'joinType': 'jtMiter' || 'jtSquare' || 'jtRound' required: "), args[3]->ToString());
            return result;
        }
    }

    if ((args.Length() > 4) && (checkLength > 4)) {
        if (!args[4]->IsNumber()) {
            result = String::Concat(String::New("Wrong argument 'miterLimit': number required: "), args[4]->ToString());
            return result;
        }
    }
    return result;
}


v8::Handle<Value> orientation(const Arguments& args) {
    v8::HandleScope scope;

    bool doubleType = false;

    v8::Local<String> errMsg = checkOffsetArguments(args, 2);
    if (errMsg->Length() > 0) {
        ThrowException(Exception::TypeError(errMsg));
        return scope.Close(v8::Undefined());
    }

    if (args[1]->Equals(String::New("double"))) {
        doubleType = true;
    }

    Polygons polyshape = v8ArrayToPolygons(v8::Local<v8::Array>::Cast(args[0]), doubleType);
    if (polyshape.size() <= 0) {
        return scope.Close(v8::Undefined());
    }

    v8::Handle<v8::Array> orientations = v8::Array::New();
    for (unsigned int i = 0; i < polyshape.size(); i++) {
        bool polyOrientation = ClipperLib::Orientation(polyshape[i]);
        orientations->Set(orientations->Length(), v8::Boolean::New(polyOrientation));
    }

    return scope.Close(orientations);
}

v8::Handle<Value> offset(const Arguments& args) {
    v8::HandleScope scope;

    JoinType joinType = jtMiter;
    double miterLimit = 30.0;
    long delta;
    bool doubleType = false;

    v8::Local<String> errMsg = checkOffsetArguments(args, 3);
    if (errMsg->Length() > 0) {
        ThrowException(Exception::TypeError(errMsg));
        return scope.Close(v8::Undefined());
    }

    if (args[1]->Equals(String::New("double"))) {
        doubleType = true;
    }

    if (doubleType) {
        delta = args[2]->NumberValue() * doubleFactor;
    } else {
        delta = args[2]->NumberValue();
    }
    #ifdef DEBUG
    if (debug > 0) std::cout << "args[2]: delta: " << delta << std::endl;
    #endif

    if (args.Length() > 3) {
        if (args[3]->Equals(String::New("jtMiter"))) {
            joinType = jtMiter;
        }
        if (args[3]->Equals(String::New("jtSquare"))) {
            joinType = jtSquare;
        }
        if (args[3]->Equals(String::New("jtRound"))) {
            joinType = jtRound;
        }
        #ifdef DEBUG
        if (debug > 0) std::cout << "args[3]: joinType: " << *v8::String::AsciiValue(args[3]) << std::endl;
        #endif
    }

    if (args.Length() > 4) {
        miterLimit = args[4]->NumberValue();
        #ifdef DEBUG
        if (debug > 0) std::cout << "args[4]: miterLimit: " << miterLimit << std::endl;
        #endif
    }

    Polygons polyshape = v8ArrayToPolygons(v8::Local<v8::Array>::Cast(args[0]), doubleType);
    doFixOrientation(polyshape);
    Polygons polyshapeOut;

    #ifdef DEBUG
    if (debug > 1) std::cout << "before Offset: polyshape.size(): " << polyshape.size() << std::endl;
    #endif
    //void OffsetPolygons(const Polygons &in_polys, Polygons &out_polys, double delta, JoinType jointype = jtSquare, double MiterLimit = 2.0);
    ClipperLib::OffsetPolygons(polyshape, polyshapeOut, delta, joinType, miterLimit);
    #ifdef DEBUG
    if (debug > 1) std::cout << "after  Offset: polyshapeOut.size(): " << polyshapeOut.size() << std::endl;
    #endif

    if (polyshapeOut.size() > 0) {
        return scope.Close(polygonsToV8Array(polyshapeOut, doubleType));
    }

    return scope.Close(v8::Undefined());
}


v8::Handle<Value> minimum(const Arguments& args) {
    v8::HandleScope scope;
    const unsigned long ScaleMax = doubleFactor - 1;

    JoinType joinType = jtMiter;
    double miterLimit = 30.0;
    bool doubleType = false;

    v8::Local<String> errMsg = checkOffsetArguments(args, 2);
    if (errMsg->Length() > 0) {
        ThrowException(Exception::TypeError(errMsg));
        return scope.Close(v8::Undefined());
    }

    if (args[1]->Equals(String::New("double"))) {
        doubleType = true;
    }

    if (args.Length() > 3) {
        if (args[3]->Equals(String::New("jtMiter"))) {
            joinType = jtMiter;
        }
        if (args[3]->Equals(String::New("jtSquare"))) {
            joinType = jtSquare;
        }
        if (args[3]->Equals(String::New("jtRound"))) {
            joinType = jtRound;
        }
        #ifdef DEBUG
        if (debug > 0) std::cout << "args[3]: joinType: " << *v8::String::AsciiValue(args[3]) << std::endl;
        #endif
    }

    if (args.Length() > 4) {
        miterLimit = args[4]->NumberValue();
        #ifdef DEBUG
        if (debug > 0) std::cout << "args[4]: miterLimit: " << miterLimit << std::endl;
        #endif
    }

    Polygons polyshape = v8ArrayToPolygons(v8::Local<v8::Array>::Cast(args[0]), doubleType);
    if (polyshape.size() <= 0) {
        return scope.Close(v8::Undefined());
    }
    doFixOrientation(polyshape);

    Polygons polyshapeOut(polyshape.size());
    long xMin = 0, xMax = 0, yMin = 0, yMax = 0, xScale = 0, yScale = 0, xyScale = 0;

    for (unsigned int i = 0; i < polyshape[0].size(); i++) {
        IntPoint ip = polyshape[0][i];
        if (i == 0) {
            xMin = ip.X;
            xMax = ip.X;
            yMin = ip.Y;
            yMax = ip.Y;
            continue;
        }
        if (ip.X < xMin) xMin = ip.X;
        if (ip.X > xMax) xMax = ip.X;
        if (ip.Y < yMin) yMin = ip.Y;
        if (ip.Y > yMax) yMax = ip.Y;
    }
    #ifdef DEBUG
    if (debug > 1) std::cout << "polyshape[0]: boundingBox: " << xMin << " " << yMin << " " << xMax << " " << yMax << std::endl;
    #endif
    xScale = signedSum(xMin, xMax) / 2;
    yScale = signedSum(yMin, yMax) / 2;
    xyScale = std::min(std::abs(xScale), std::abs(yScale));

    int s = -1;
    int loops = 0;
    long scale = xyScale;
    #ifdef DEBUG
    if (debug > 1) std::cout << "OffsetPolygons: xyScale: " << xyScale << std::endl;
    #endif
    long lastScale = 2;
    do {
        loops++;
        //void OffsetPolygons(const Polygons &in_polys, Polygons &out_polys, double delta, JoinType jointype = jtSquare, double MiterLimit = 2.0);
        try {
            ClipperLib::OffsetPolygons(polyshape, polyshapeOut, s * scale, joinType, miterLimit);
        }
        catch (...) {
            miterLimit /= 2;
            if (miterLimit < 5) {
                miterLimit = 5;
            }
            std::cout << "node-clipper: exception from ClipperLib catched! miterLimit reduced: " << miterLimit<< std::endl;
            continue;
        }
        #ifdef DEBUG
        if (debug > 1) std::cout << "polyshapeOut.size():    " << polyshapeOut.size() << std::endl;
        if (debug > 1) std::cout << "polyshapeOut[0].size(): " << polyshapeOut[0].size() << std::endl;
        #endif
        if (polyshapeOut.size() <= 0) {
            scale -= xyScale / lastScale;
        } else {
            // only one line with four or less vertices -> break
            if (polyshapeOut.size() == 1 && polyshapeOut[0].size() <= 3) break;
            if (polyshapeOut.size() == 1 && polyshapeOut[0].size() <= 5 && loops > 32) break;
            scale += xyScale / lastScale;
        }
        lastScale = (lastScale << 1) & ScaleMax;
        #ifdef DEBUG
        if (debug > 1) std::cout << "OffsetPolygons: scale:     " << scale << std::endl;
        if (debug > 1) std::cout << "OffsetPolygons: lastScale: " << lastScale << std::endl;
        #endif
    } while (lastScale != 0 || loops > 64);
    #ifdef DEBUG
    if (debug > 0) std::cout << "MinimumPolygon: finished: loops: " << loops << std::endl;
    #endif

    if (polyshapeOut.size() > 0) {
        return scope.Close(polygonsToV8Array(polyshapeOut, doubleType));
    }

    return scope.Close(v8::Undefined());
}


v8::Handle<Value> clip(const Arguments& args) {
    v8::HandleScope scope;
    bool doubleType = false;
    ClipType clipType = ctIntersection;

    v8::Local<String> errMsg = checkOffsetArguments(args, 1);
    if (errMsg->Length() > 0) {
        ThrowException(Exception::TypeError(errMsg));
        return scope.Close(v8::Undefined());
    }

    if (args[2]->Equals(String::New("double"))) {
        doubleType = true;
    }

    if (args.Length() > 3) {
        if (args[3]->Equals(String::New("ctUnion"))) {
            clipType = ctUnion;
        }
        if (args[3]->Equals(String::New("ctDifference"))) {
            clipType = ctDifference;
        }
        if (args[3]->Equals(String::New("ctXor"))) {
            clipType = ctXor;
        }
        #ifdef DEBUG
        if (debug > 0) std::cout << "args[3]: clipType: " << *v8::String::AsciiValue(args[3]) << std::endl;
        #endif
    }

    Clipper clipper;

    Polygons polyshape = v8ArrayToPolygons(v8::Local<v8::Array>::Cast(args[0]), doubleType);
    if (polyshape.size() <= 0) {
        return scope.Close(v8::Undefined());
    }
    clipper.AddPolygons(polyshape, ptSubject);

    polyshape = v8ArrayToPolygons(v8::Local<v8::Array>::Cast(args[1]), doubleType);
    if (polyshape.size() <= 0) {
        return scope.Close(v8::Undefined());
    }
    clipper.AddPolygons(polyshape, ptClip);

    Polygons clipperSolution;

    clipper.Execute(clipType, clipperSolution, pftNonZero, pftNonZero);

    #ifdef DEBUG
    if (debug > 1) std::cout << "clipperSolution.size(): " << clipperSolution.size() << std::endl;
    #endif

    v8::Handle<v8::Array> solutions = v8::Array::New();
    Polygons singleSolution;
    unsigned int i = 0;
    while (i < clipperSolution.size()) {
        do {
            singleSolution.push_back(clipperSolution[i]);
            i++;
        } while (i < clipperSolution.size() && !ClipperLib::Orientation(clipperSolution[i]));
        solutions->Set(solutions->Length(), polygonsToV8Array(singleSolution, doubleType));
        singleSolution.clear();
    }
    #ifdef DEBUG
    if (debug > 1) std::cout << "solutions->Length(): " << solutions->Length() << std::endl;
    #endif

    if (solutions->Length() > 0) {
        return scope.Close(solutions);
    } else {
        return scope.Close(v8::Undefined());
    }
}


v8::Handle<Value> clean(const Arguments& args) {
    v8::HandleScope scope;
    bool doubleType = false;
    double distance = 1.415;

    v8::Local<String> errMsg = checkOffsetArguments(args, 2);
    if (errMsg->Length() > 0) {
        ThrowException(Exception::TypeError(errMsg));
        return scope.Close(v8::Undefined());
    }

    if (args[1]->Equals(String::New("double"))) {
        doubleType = true;
    }

    if (args.Length() > 2) {
        distance = args[2]->NumberValue();
        #ifdef DEBUG
        if (debug > 0) std::cout << "args[2]: distance: " << *v8::String::AsciiValue(args[2]) << std::endl;
        #endif
    }

    Polygons polyshape = v8ArrayToPolygons(v8::Local<v8::Array>::Cast(args[0]), doubleType);
    Polygons polyshapeOut(polyshape.size());

    #ifdef DEBUG
    if (debug > 1) std::cout << "before CleanPolygons: polyshape.size(): " << polyshape.size() << std::endl;
    #endif
    //void CleanPolygons(Polygons &in_polys, Polygon &out_polys, double distance = 1.415);
    ClipperLib::CleanPolygons(polyshape, polyshapeOut, distance);
    #ifdef DEBUG
    if (debug > 1) std::cout << "after  CleanPolygons: polyshapeOut.size(): " << polyshapeOut.size() << std::endl;
    #endif

    if (polyshapeOut.size() > 0) {
        return scope.Close(polygonsToV8Array(polyshapeOut, doubleType));
    }

    return scope.Close(v8::Undefined());
}


v8::Handle<Value> fixOrientation(const Arguments& args) {
    v8::HandleScope scope;
    bool doubleType = false;

    v8::Local<String> errMsg = checkOffsetArguments(args, 2);
    if (errMsg->Length() > 0) {
        ThrowException(Exception::TypeError(errMsg));
        return scope.Close(v8::Undefined());
    }

    if (args[1]->Equals(String::New("double"))) {
        doubleType = true;
    }


    Polygons polyshape = v8ArrayToPolygons(v8::Local<v8::Array>::Cast(args[0]), doubleType);
    doFixOrientation(polyshape);

    if (polyshape.size() > 0) {
        return scope.Close(polygonsToV8Array(polyshape, doubleType));
    }

    return scope.Close(v8::Undefined());
}


v8::Handle<Value> simplify(const Arguments& args) {
    v8::HandleScope scope;
    bool doubleType = false;

    v8::Local<String> errMsg = checkOffsetArguments(args, 2);
    if (errMsg->Length() > 0) {
        ThrowException(Exception::TypeError(errMsg));
        return scope.Close(v8::Undefined());
    }

    if (args[1]->Equals(String::New("double"))) {
        doubleType = true;
    }


    Polygons polyshape = v8ArrayToPolygons(v8::Local<v8::Array>::Cast(args[0]), doubleType);
    ClipperLib::SimplifyPolygons(polyshape, polyshape, pftNonZero);

    if (polyshape.size() > 0) {
        return scope.Close(polygonsToV8Array(polyshape, doubleType));
    }

    return scope.Close(v8::Undefined());
}



extern "C" void init(Handle<Object> target) {
    target->Set(String::NewSymbol("setDebug"), FunctionTemplate::New(setDebug)->GetFunction());
    target->Set(String::NewSymbol("orientation"), FunctionTemplate::New(orientation)->GetFunction());
    target->Set(String::NewSymbol("offset"), FunctionTemplate::New(offset)->GetFunction());
    target->Set(String::NewSymbol("minimum"), FunctionTemplate::New(minimum)->GetFunction());
    target->Set(String::NewSymbol("clip"), FunctionTemplate::New(clip)->GetFunction());
    target->Set(String::NewSymbol("clean"), FunctionTemplate::New(clean)->GetFunction());
    target->Set(String::NewSymbol("fixOrientation"), FunctionTemplate::New(fixOrientation)->GetFunction());
    target->Set(String::NewSymbol("simplify"), FunctionTemplate::New(simplify)->GetFunction());
}

NODE_MODULE(clipper, init)
