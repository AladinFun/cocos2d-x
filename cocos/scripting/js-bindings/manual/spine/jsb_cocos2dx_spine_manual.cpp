/*
 * Created by ucchen on 2/12/14.
 * Copyright (c) 2014 Chukong Technologies Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "scripting/js-bindings/manual/spine/jsb_cocos2dx_spine_manual.h"
#include "scripting/js-bindings/manual/ScriptingCore.h"
#include "scripting/js-bindings/manual/cocos2d_specifics.hpp"
#include "editor-support/spine/spine-cocos2dx.h"

using namespace spine;

jsval speventdata_to_jsval(JSContext* cx, spEventData& v)
{
    JS::RootedObject tmp(cx, JS_NewObject(cx, nullptr, JS::NullPtr(), JS::NullPtr()));
    if (!tmp) return JSVAL_NULL;
    JS::RootedValue jsname(cx, c_string_to_jsval(cx, v.name));
    JS::RootedValue jsstr(cx, c_string_to_jsval(cx, v.stringValue));
    bool ok = JS_DefineProperty(cx, tmp, "name", jsname, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "intValue", v.intValue, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "floatValue", v.floatValue, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "stringValue", jsstr, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    return JSVAL_NULL;
}

jsval spevent_to_jsval(JSContext* cx, spEvent& v)
{
    JS::RootedObject tmp(cx, JS_NewObject(cx, nullptr, JS::NullPtr(), JS::NullPtr()));
    if (!tmp) return JSVAL_NULL;
    
    JS::RootedValue jsdata(cx, speventdata_to_jsval(cx, *v.data));
    JS::RootedValue jsstr(cx, c_string_to_jsval(cx, v.stringValue));
    bool ok = JS_DefineProperty(cx, tmp, "data", jsdata, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "intValue", v.intValue, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "floatValue", v.floatValue, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "stringValue", jsstr, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    
    return JSVAL_NULL;
}

jsval spbonedata_to_jsval(JSContext* cx, const spBoneData* v)
{
    JS::RootedObject tmp(cx, JS_NewObject(cx, nullptr, JS::NullPtr(), JS::NullPtr()));
    if (!tmp) return JSVAL_NULL;
    
    // root haven't parent
    JS::RootedValue parentVal(cx);
    if (strcmp(v->name, "root") && v->parent)
        parentVal = spbonedata_to_jsval(cx, v->parent);
    
    JS::RootedValue jsname(cx, c_string_to_jsval(cx, v->name));
    bool ok = JS_DefineProperty(cx, tmp, "name", jsname, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "parent", parentVal,JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "length", v->length, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "x", v->x, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "y", v->y, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "rotation", v->rotation, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "scaleX", v->scaleX, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "scaleY", v->scaleY, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "inheritScale", v->inheritScale, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "inheritRotation", v->inheritRotation, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    
    return JSVAL_NULL;
}

jsval spbone_to_jsval(JSContext* cx, spBone& v)
{
    JS::RootedObject tmp(cx, JS_NewObject(cx, nullptr, JS::NullPtr(), JS::NullPtr()));
    if (!tmp) return JSVAL_NULL;
    
    // root haven't parent
    JS::RootedValue parentVal(cx);
    if (strcmp(v.data->name, "root") && v.parent)
        parentVal = spbone_to_jsval(cx, *v.parent);
    
    JS::RootedValue jsdata(cx, spbonedata_to_jsval(cx, v.data));
    bool ok = JS_DefineProperty(cx, tmp, "data", jsdata, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "parent", parentVal, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "x", v.x, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "y", v.y, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "rotation", v.rotation, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "scaleX", v.scaleX, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "scaleY", v.scaleY, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "shearX", v.shearX, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "shearY", v.shearY, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "m00", v.a, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "m01", v.b, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "worldX", v.worldX, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "m10", v.c, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "m11", v.d, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "worldY", v.worldY, JSPROP_ENUMERATE | JSPROP_PERMANENT);
        JS_DefineProperty(cx, tmp, "worldSignX", v.worldSignX, JSPROP_ENUMERATE | JSPROP_PERMANENT);
        JS_DefineProperty(cx, tmp, "worldSignY", v.worldSignY, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    /*
        JS_DefineProperty(cx, tmp, "worldRotation", v.worldRotation, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "worldScaleX", v.worldScaleX, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "worldScaleY", v.worldScaleY, JSPROP_ENUMERATE | JSPROP_PERMANENT);
     */

    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    
    return JSVAL_NULL;
}

jsval spskeleton_to_jsval(JSContext* cx, spSkeleton& v)
{
    JS::RootedObject tmp(cx, JS_NewObject(cx, nullptr, JS::NullPtr(), JS::NullPtr()));
    if (!tmp) return JSVAL_NULL;
    
    bool ok = JS_DefineProperty(cx, tmp, "x", v.x, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "y", v.y, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "flipX", v.flipX, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "flipY", v.flipY, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "time", v.time, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "boneCount", v.bonesCount, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "slotCount", v.slotsCount, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    
    return JSVAL_NULL;
}

jsval spattachment_to_jsval(JSContext* cx, spAttachment& v)
{
    JS::RootedObject tmp(cx, JS_NewObject(cx, nullptr, JS::NullPtr(), JS::NullPtr()));
    if (!tmp) return JSVAL_NULL;
    
    JS::RootedValue jsname(cx, c_string_to_jsval(cx, v.name));
    bool ok = JS_DefineProperty(cx, tmp, "name", jsname, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "type", v.type, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    
    return JSVAL_NULL;
}

jsval spslotdata_to_jsval(JSContext* cx, spSlotData& v)
{
    JS::RootedObject tmp(cx, JS_NewObject(cx, nullptr, JS::NullPtr(), JS::NullPtr()));
    if (!tmp) return JSVAL_NULL;

    JS::RootedValue jsname(cx, c_string_to_jsval(cx, v.name));
    JS::RootedValue jsattachmentName(cx, c_string_to_jsval(cx, v.attachmentName));
    JS::RootedValue jsboneData(cx, spbonedata_to_jsval(cx, v.boneData));
    bool ok = JS_DefineProperty(cx, tmp, "name", jsname, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "attachmentName", jsattachmentName, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "r", v.r, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "g", v.g, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "b", v.b, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "a", v.a, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "blendMode", v.blendMode, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "boneData", jsboneData, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    
    return JSVAL_NULL;
}

jsval spslot_to_jsval(JSContext* cx, spSlot& v)
{
    JS::RootedObject tmp(cx, JS_NewObject(cx, nullptr, JS::NullPtr(), JS::NullPtr()));
    if (!tmp) return JSVAL_NULL;
    
    JS::RootedValue jsbone(cx, spbone_to_jsval(cx, *v.bone));
    JS::RootedValue jsattachment(cx, spattachment_to_jsval(cx, *v.attachment));
    JS::RootedValue jsdata(cx, spslotdata_to_jsval(cx, *v.data));
    bool ok = JS_DefineProperty(cx, tmp, "r", v.r, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "g", v.g, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "b", v.b, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "a", v.a, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "bone", jsbone, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        //JS_DefineProperty(cx, tmp, "skeleton", spskeleton_to_jsval(cx, *v.skeleton), NULL, NULL, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "attachment", jsattachment, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "data", jsdata, JSPROP_ENUMERATE | JSPROP_PERMANENT);
        
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    
    return JSVAL_NULL;
}

jsval sptimeline_to_jsval(JSContext* cx, spTimeline& v)
{
    JS::RootedObject tmp(cx, JS_NewObject(cx, nullptr, JS::NullPtr(), JS::NullPtr()));
    if (!tmp) return JSVAL_NULL;
    
    bool ok = JS_DefineProperty(cx, tmp, "type", v.type, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    
    return JSVAL_NULL;
}

jsval spanimationstate_to_jsval(JSContext* cx, spAnimationState& v)
{
    JS::RootedObject tmp(cx, JS_NewObject(cx, nullptr, JS::NullPtr(), JS::NullPtr()));
    if (!tmp) return JSVAL_NULL;
    
    bool ok = JS_DefineProperty(cx, tmp, "timeScale", v.timeScale, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "trackCount", v.tracksCount, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    
    return JSVAL_NULL;
}

jsval spanimation_to_jsval(JSContext* cx, spAnimation& v)
{
    JS::RootedObject tmp(cx, JS_NewObject(cx, nullptr, JS::NullPtr(), JS::NullPtr()));
    if (!tmp) return JSVAL_NULL;
    
    JS::RootedValue jsname(cx, c_string_to_jsval(cx, v.name));
    JS::RootedValue jstimelines(cx, sptimeline_to_jsval(cx, **v.timelines));
    bool ok = JS_DefineProperty(cx, tmp, "duration", v.duration, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "timelineCount", v.timelinesCount, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "name", jsname, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "timelines", jstimelines, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    
    return JSVAL_NULL;
}

bool jsb_spine_TrackEntry_get_next(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
    JS::RootedObject jsthis(cx, obj);
    js_proxy_t *proxy = jsb_get_js_proxy(jsthis);
    spTrackEntry* cobj = (spTrackEntry *)(proxy ? proxy->ptr : NULL);
    if (cobj) {
        JS::RootedValue jsret(cx, JS::NullValue());
        if (cobj->next)
        {
            jsret = sptrackentry_to_jsval(cx, *cobj->next);
        }
        vp.set(jsret);
        return true;
    }
    else {
        CCLOGERROR("jsb_spine_TrackEntry_get_next : Invalid Native Object");
        return false;
    }
}

bool jsb_spine_TrackEntry_get_previous(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
    JS::RootedObject jsthis(cx, obj);
    js_proxy_t *proxy = jsb_get_js_proxy(jsthis);
    spTrackEntry* cobj = (spTrackEntry *)(proxy ? proxy->ptr : NULL);
    if (cobj) {
        JS::RootedValue jsret(cx, JS::NullValue());
        if (cobj->previous)
        {
            jsret = sptrackentry_to_jsval(cx, *cobj->previous);
        }
        vp.set(jsret);
        return true;
    }
    else {
        CCLOGERROR("jsb_spine_TrackEntry_get_previous : Invalid Native Object");
        return false;
    }
}

jsval sptrackentry_to_jsval(JSContext* cx, spTrackEntry& v)
{
    js_proxy_t *proxy = jsb_get_native_proxy(&v);
    if (proxy)
    {
        JS::RootedObject entry(cx, proxy->obj);
        return OBJECT_TO_JSVAL(entry);
    }
    else
    {
        JS::RootedObject tmp(cx, JS_NewObject(cx, nullptr, JS::NullPtr(), JS::NullPtr()));
        if (!tmp) return JSVAL_NULL;
        
        jsb_new_proxy(&v, tmp);
        js_add_FinalizeHook(cx, tmp, false);
        
#if COCOS2D_DEBUG > 1
        CCLOG("++++++WEAK_REF++++++ Cpp(spine::TrackEntry): %p - JS: %p", &v, tmp.get());
#endif // COCOS2D_DEBUG
        
        JS::RootedValue jsanimation(cx, spanimation_to_jsval(cx, *v.animation));
        bool ok = JS_DefineProperty(cx, tmp, "delay", v.delay, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "time", v.time, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "lastTime", v.lastTime, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "endTime", v.endTime, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "timeScale", v.timeScale, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "mixTime", v.mixTime, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "mixDuration", v.mixDuration, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "animation", jsanimation, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
        JS_DefineProperty(cx, tmp, "next", JS::UndefinedHandleValue, JSPROP_ENUMERATE | JSPROP_PERMANENT, jsb_spine_TrackEntry_get_next) &&
        JS_DefineProperty(cx, tmp, "previous", JS::UndefinedHandleValue, JSPROP_ENUMERATE | JSPROP_PERMANENT, jsb_spine_TrackEntry_get_previous);
        
        if (ok)
        {
            return OBJECT_TO_JSVAL(tmp);
        }
        
        return JSVAL_NULL;
    }
}

bool jsb_cocos2dx_spine_findBone(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    SkeletonRenderer* cobj = (SkeletonRenderer *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    if (argc == 1) {
        const char* arg0;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "Error processing arguments");
        spBone* ret = cobj->findBone(arg0);
        jsval jsret = JSVAL_NULL;
        do {
            if (ret)
            {
                jsret = spbone_to_jsval(cx, *ret);
            }
        } while (0);
        
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool jsb_cocos2dx_spine_findSlot(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    SkeletonRenderer* cobj = (SkeletonRenderer *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    if (argc == 1) {
        const char* arg0;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "Error processing arguments");
        spSlot* ret = cobj->findSlot(arg0);
        jsval jsret = JSVAL_NULL;
        do {
            if (ret)
            {
                jsret = spslot_to_jsval(cx, *ret);
            }
        } while (0);
        
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool jsb_cocos2dx_spine_setDebugBones(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    SkeletonRenderer* cobj = (SkeletonRenderer *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    if (argc == 1) {
        bool enable = args.get(0).toBoolean();
        cobj->setDebugBonesEnabled(enable);
        
        args.rval().setUndefined();
        return true;
    }
    
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool jsb_cocos2dx_spine_setDebugSolots(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    SkeletonRenderer* cobj = (SkeletonRenderer *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    if (argc == 1) {
        bool enable = args.get(0).toBoolean();
        cobj->setDebugSlotsEnabled(enable);
        
        args.rval().setUndefined();
        return true;
    }
    
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool jsb_cocos2dx_spine_getAttachment(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    SkeletonRenderer* cobj = (SkeletonRenderer*)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    if (argc == 2) {
        const char* arg0;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        const char* arg1;
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "Error processing arguments");
        spAttachment* ret = cobj->getAttachment(arg0, arg1);
        jsval jsret = JSVAL_NULL;
        do {
            if (ret)
            {
                jsret = spattachment_to_jsval(cx, *ret);
            }
        } while(0);
        
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool jsb_cocos2dx_spine_getCurrent(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    spine::SkeletonAnimation* cobj = (spine::SkeletonAnimation *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    if (argc == 1) {
        int arg0;
        ok &= jsval_to_int32(cx, args.get(0), (int32_t *)&arg0);
        JSB_PRECONDITION2(ok, cx, false, "Error processing arguments");
        spTrackEntry* ret = cobj->getCurrent(arg0);
        jsval jsret = JSVAL_NULL;
        do {
            if (ret)
            {
                jsret = sptrackentry_to_jsval(cx, *ret);
            }
        } while (0);
        
        args.rval().set(jsret);
        return true;
    }
    else if (argc == 0) {
        spTrackEntry* ret = cobj->getCurrent();
        jsval jsret = JSVAL_NULL;
        do {
            if (ret)
            {
                jsret = sptrackentry_to_jsval(cx, *ret);
            }
        } while (0);
        
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool jsb_cocos2dx_spine_setAnimation(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    spine::SkeletonAnimation* cobj = (spine::SkeletonAnimation *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    if (argc == 3) {
        int arg0;
        ok &= jsval_to_int32(cx, args.get(0), (int32_t *)&arg0);
        
        const char* arg1;
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        
        bool arg2 = JS::ToBoolean(args.get(2));
        JSB_PRECONDITION2(ok, cx, false, "Error processing arguments");
        
        spTrackEntry* ret = cobj->setAnimation(arg0, arg1, arg2);
        jsval jsret = JSVAL_NULL;
        
        do {
            if (ret)
            {
                jsret = sptrackentry_to_jsval(cx, *ret);
            }
        } while(0);
        
        args.rval().set(jsret);
        return true;
    }
    
    
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool jsb_cocos2dx_spine_addAnimation(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    spine::SkeletonAnimation* cobj = (spine::SkeletonAnimation *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "Invalid Native Object");
    if (argc == 3) {
        int arg0;
        ok &= jsval_to_int32(cx, args.get(0), (int32_t *)&arg0);
        
        const char* arg1;
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        
        bool arg2 = JS::ToBoolean(args.get(2));
        JSB_PRECONDITION2(ok, cx, false, "Error processing arguments");
        
        spTrackEntry* ret = cobj->addAnimation(arg0, arg1, arg2);
        jsval jsret = JSVAL_NULL;
        
        do {
            if (ret)
            {
                jsret = sptrackentry_to_jsval(cx, *ret);
            }
        } while(0);
        
        args.rval().set(jsret);
        return true;
    } else if (argc == 4) {
        int arg0;
        ok &= jsval_to_int32(cx, args.get(0), (int32_t *)&arg0);
        
        const char* arg1;
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        
        bool arg2 = JS::ToBoolean(args.get(2));
        
        double arg3;
        ok &= JS::ToNumber(cx, args.get(3), &arg3);
        JSB_PRECONDITION2(ok, cx, false, "Error processing arguments");
        
        spTrackEntry* ret = cobj->addAnimation(arg0, arg1, arg2, arg3);
        jsval jsret = JSVAL_NULL;
        
        do {
            if (ret)
            {
                jsret = sptrackentry_to_jsval(cx, *ret);
            }
        } while(0);
        
        args.rval().set(jsret);
        return true;
    }    
    
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}



bool jsb_cocos2dx_spine_SkeletonRenderer_setAttachment2(JSContext *cx, uint32_t argc, jsval *vp)
{
	bool ok = true;
	spine::SkeletonRenderer* cobj = nullptr;

	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx);
	obj = args.thisv().toObjectOrNull();
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	cobj = (spine::SkeletonRenderer *)(proxy ? proxy->ptr : nullptr);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_spine_SkeletonRenderer_setAttachment2 : Invalid Native Object");

	do {
		if (argc == 3) {
			std::string arg0;
			ok &= jsval_to_std_string(cx, args.get(0), &arg0);
			if (!ok) { ok = true; break; }

			std::string arg1;
			ok &= jsval_to_std_string(cx, args.get(1), &arg1);
			if (!ok) { ok = true; break; }

			std::string arg2;
			ok &= jsval_to_std_string(cx, args.get(2), &arg2);
			if (!ok) { ok = true; break; }

			bool ret = cobj->setAttachment2(arg0, arg1, arg2);
			jsval jsret = JSVAL_NULL;
			jsret = BOOLEAN_TO_JSVAL(ret);
			args.rval().set(jsret);
			return true;
		}
	} while (0);

	JS_ReportError(cx, "js_cocos2dx_spine_SkeletonRenderer_setAttachment2 : wrong number of arguments");
	return false;
}

extern JSObject* jsb_spine_SkeletonRenderer_prototype;
extern JSObject* jsb_spine_SkeletonAnimation_prototype;

void register_all_cocos2dx_spine_manual(JSContext* cx, JS::HandleObject global)
{
    JS::RootedObject skeletonRenderer(cx, jsb_spine_SkeletonRenderer_prototype);
    JS_DefineFunction(cx, skeletonRenderer, "findBone", jsb_cocos2dx_spine_findBone, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, skeletonRenderer, "findSlot", jsb_cocos2dx_spine_findSlot, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, skeletonRenderer, "setDebugBones", jsb_cocos2dx_spine_setDebugBones, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, skeletonRenderer, "setDebugSolots", jsb_cocos2dx_spine_setDebugSolots, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, skeletonRenderer, "getAttachment", jsb_cocos2dx_spine_getAttachment, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
	JS_DefineFunction(cx, skeletonRenderer, "setAttachment2", jsb_cocos2dx_spine_SkeletonRenderer_setAttachment2, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);

    JS::RootedObject skeletonAnimation(cx, jsb_spine_SkeletonAnimation_prototype);
    JS_DefineFunction(cx, skeletonAnimation, "getCurrent", jsb_cocos2dx_spine_getCurrent, 1, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, skeletonAnimation, "setAnimation", jsb_cocos2dx_spine_setAnimation, 3, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineFunction(cx, skeletonAnimation, "addAnimation", jsb_cocos2dx_spine_addAnimation, 4, JSPROP_ENUMERATE | JSPROP_PERMANENT);

}
