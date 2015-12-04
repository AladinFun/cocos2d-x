//
//  jsb_downloader.cpp
//  cocos2d_js_bindings
//
//  Created by 李强 on 15/11/19.
//
//

#include "jsb_downloader.h"
#include "cocos2d.h"
#include "network/CCDownloader.h"
#include "spidermonkey_specifics.h"
#include "ScriptingCore.h"
#include "cocos2d_specifics.hpp"
#include "md5.h"

#ifdef MINIZIP_FROM_SYSTEM
#include <minizip/unzip.h>
#else // from our embedded sources
#include "unzip/unzip.h"
#endif

using namespace cocos2d;
using namespace cocos2d::network;

#define BUFFER_SIZE     8192
#define MAX_FILENAME    512

JSClass  *js_cocos2dx_downloader_class;
JSObject *js_cocos2dx_downloader_prototype;

static inline char hex2char(unsigned char c)
{
    if (c >=0 && c <= 9) {
        return '0' + c;
    }
    else {
        return 'a' + c - 0xa;
    }
}

static inline std::string translate_md5(unsigned char *buf)
{
    char ret[32];
    for (int i = 0; i < 16; i++) {
        unsigned char p = buf[i];
        unsigned char h = (p&0xf0) >> 4, l = p&0xf;
        ret[2 * i]     = hex2char(h);
        ret[2 * i + 1] = hex2char(l);
    }
    return std::string(ret, 32);
}

static std::string basename(const std::string& path)
{
    size_t found = path.find_last_of("/\\");
    
    if (std::string::npos != found)
    {
        return path.substr(0, found);
    }
    else
    {
        return path;
    }
}

static bool decompress(const std::string &zip)
{
    // Find root path for zip file
    auto fileutils = FileUtils::getInstance();
    size_t pos = zip.find_last_of("/\\");
    if (pos == std::string::npos)
    {
        CCLOG("jsb_downloader_unzip : no root path specified for zip file %s\n", zip.c_str());
        return false;
    }
    const std::string rootPath = zip.substr(0, pos+1);
    
    // Open the zip file
    unzFile zipfile = unzOpen(fileutils->getSuitableFOpen(zip).c_str());
    if (!zipfile)
    {
        CCLOG("jsb_downloader_unzip : can not open downloaded zip file %s\n", zip.c_str());
        return false;
    }
    
    // Get info about the zip file
    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK)
    {
        CCLOG("jsb_downloader_unzip : can not read file global info of %s\n", zip.c_str());
        unzClose(zipfile);
        return false;
    }
    
    // Buffer to hold data read from the zip file
    char readBuffer[BUFFER_SIZE];
    // Loop to extract all files.
    uLong i;
    for (i = 0; i < global_info.number_entry; ++i)
    {
        // Get info about current file.
        unz_file_info fileInfo;
        char fileName[MAX_FILENAME];
        if (unzGetCurrentFileInfo(zipfile,
                                  &fileInfo,
                                  fileName,
                                  MAX_FILENAME,
                                  NULL,
                                  0,
                                  NULL,
                                  0) != UNZ_OK)
        {
            CCLOG("jsb_downloader_unzip : can not read compressed file info\n");
            unzClose(zipfile);
            return false;
        }
        const std::string fullPath = rootPath + fileName;
        
        // Check if this entry is a directory or a file.
        const size_t filenameLength = strlen(fileName);
        if (fileName[filenameLength-1] == '/')
        {
            //There are not directory entry in some case.
            //So we need to create directory when decompressing file entry
            if (!fileutils->createDirectory(basename(fullPath)))
            {
                // Failed to create directory
                CCLOG("jsb_downloader_unzip : can not create directory %s\n", fullPath.c_str());
                unzClose(zipfile);
                return false;
            }
        }
        else
        {
            // Entry is a file, so extract it.
            // Open current file.
            if (unzOpenCurrentFile(zipfile) != UNZ_OK)
            {
                CCLOG("jsb_downloader_unzip : can not extract file %s\n", fileName);
                unzClose(zipfile);
                return false;
            }
            
            // Create a file to store current file.
            FILE *out = fopen(fileutils->getSuitableFOpen(fullPath).c_str(), "wb");
            if (!out)
            {
                CCLOG("jsb_downloader_unzip : can not create decompress destination file %s\n", fullPath.c_str());
                unzCloseCurrentFile(zipfile);
                unzClose(zipfile);
                return false;
            }
            
            // Write current file content to destinate file.
            int error = UNZ_OK;
            do
            {
                error = unzReadCurrentFile(zipfile, readBuffer, BUFFER_SIZE);
                if (error < 0)
                {
                    CCLOG("jsb_downloader_unzip : can not read zip file %s, error code is %d\n", fileName, error);
                    fclose(out);
                    unzCloseCurrentFile(zipfile);
                    unzClose(zipfile);
                    return false;
                }
                
                if (error > 0)
                {
                    fwrite(readBuffer, error, 1, out);
                }
            } while(error > 0);
            
            fclose(out);
        }
        
        unzCloseCurrentFile(zipfile);
        
        // Goto next entry listed in the zip file.
        if ((i + 1) < global_info.number_entry)
        {
            if (unzGoToNextFile(zipfile) != UNZ_OK)
            {
                CCLOG("jsb_downloader_unzip : can not read next file for decompressing\n");
                unzClose(zipfile);
                return false;
            }
        }
    }
    
    unzClose(zipfile);
    return true;
}

bool js_cocos2dx_extension_Downloader_constructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    
    if (argc == 0)
    {
        JS::RootedObject obj(cx, JS_NewObject(cx, js_cocos2dx_downloader_class, JS::RootedObject(cx, js_cocos2dx_downloader_prototype), JS::NullPtr()));
        JSObject* _JSDelegate = obj;
        
        Downloader* cobj = new Downloader();
        cobj->onTaskError = [=](const DownloadTask& task,
                                int errorCode,
                                int errorCodeInternal,
                                const std::string& errorStr)
        {
             std::string downloadID = task.identifier;
            
            // 回调JS方法
            Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
                jsval params[2];
                params[0] = c_string_to_jsval(cx, downloadID.c_str());
                params[1] = c_string_to_jsval(cx, errorStr.c_str());
                ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(_JSDelegate), "onDownloadFail", 2, params);
            });
        };
        
        cobj->onFileTaskSuccess = [=](const DownloadTask& task)
        {
            // 获取文件md5值
            MD5_CTX md5_ctx;
            MD5_Init(&md5_ctx);
            unsigned char md5_result[16] = {0};
            auto filedata = FileUtils::getInstance()->getDataFromFile(task.storagePath);
            MD5_Update(&md5_ctx, filedata.getBytes(), filedata.getSize());
            MD5_Final(md5_result, &md5_ctx);
            std::string file_md5 = translate_md5(md5_result);
            std::string storagePath = task.storagePath;
            std::string downloadID = task.identifier;
            
            // 回调JS方法
            Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
                jsval params[3];
                params[0] = c_string_to_jsval(cx, storagePath.c_str());
                params[1] = c_string_to_jsval(cx, downloadID.c_str());
                params[2] = c_string_to_jsval(cx, file_md5.c_str());
                ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(_JSDelegate), "onDownloadSucc", 3, params);
            });
        };
        
        // link the native object with the javascript object
        js_proxy_t *p = jsb_new_proxy(cobj, obj);
        JS::AddNamedObjectRoot(cx, &p->obj, "Downloader");
        
        args.rval().set(OBJECT_TO_JSVAL(obj));
        
        return true;
    }
    
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_Downloader_download(JSContext *cx, uint32_t argc, jsval *vp) {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    Downloader* cobj = (Downloader *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");
    
    if (argc == 3) {
        do
        {
            std::string srcUrl;
            jsval_to_std_string(cx, args.get(0), &srcUrl);
            std::string storagePath;
            jsval_to_std_string(cx, args.get(1), &storagePath);
            std::string downloadID;
            jsval_to_std_string(cx, args.get(2), &downloadID);
            auto fn = [=] {
                cobj->createDownloadFileTask(srcUrl, storagePath, downloadID);
            };
            std::thread th(fn);
            th.detach();
        } while (0);
        
        args.rval().setUndefined();
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_Downloader_unzip(JSContext *cx, uint32_t argc, jsval *vp) {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    Downloader* cobj = (Downloader *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");
    jsval dataVal = OBJECT_TO_JSVAL(obj);
    if (argc == 2) {
        do
        {
            std::string storagePath;
            jsval_to_std_string(cx, args.get(0), &storagePath);
            std::string identifier;
            jsval_to_std_string(cx, args.get(1), &identifier);
            auto fn = [=] {
                // unzip文件
                bool ret = decompress(storagePath);
                if (ret)
                {
                    // unzip成功，删除文件
                    FileUtils::getInstance()->removeFile(storagePath);
                }
                
                // 回调JS方法
                Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
                    jsval params[2];
                    if (ret)
                    {
                        params[0] = c_string_to_jsval(cx, storagePath.c_str());
                    }
                    else{
                        params[0] = c_string_to_jsval(cx, "");
                    }
                    params[1] = c_string_to_jsval(cx, identifier.c_str());
                    ScriptingCore::getInstance()->executeFunctionWithOwner(dataVal, "onUnzipComplete", 2, params);
                });
            };
            std::thread th(fn);
            th.detach();
        } while (0);
        
        args.rval().setUndefined();
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

static void js_cocos2dx_Downloader_finalize(JSFreeOp *fop, JSObject *obj) {
    CCLOG("jsbindings: finalizing JS object %p (Downloader)", obj);
}

void register_jsb_downloader(JSContext *cx, JS::HandleObject global) {
    js_cocos2dx_downloader_class = (JSClass *)calloc(1, sizeof(JSClass));
    js_cocos2dx_downloader_class->name = "Downloader";
    js_cocos2dx_downloader_class->addProperty = JS_PropertyStub;
    js_cocos2dx_downloader_class->delProperty = JS_DeletePropertyStub;
    js_cocos2dx_downloader_class->getProperty = JS_PropertyStub;
    js_cocos2dx_downloader_class->setProperty = JS_StrictPropertyStub;
    js_cocos2dx_downloader_class->enumerate = JS_EnumerateStub;
    js_cocos2dx_downloader_class->resolve = JS_ResolveStub;
    js_cocos2dx_downloader_class->convert = JS_ConvertStub;
    js_cocos2dx_downloader_class->finalize = js_cocos2dx_Downloader_finalize;
    js_cocos2dx_downloader_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);
    
    static JSFunctionSpec funcs[] = {
        JS_FN("download", js_cocos2dx_extension_Downloader_download, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("unzip",    js_cocos2dx_extension_Downloader_unzip,    2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FS_END
    };
    
    js_cocos2dx_downloader_prototype = JS_InitClass(
                                                   cx, global,
                                                   JS::NullPtr(),
                                                   js_cocos2dx_downloader_class,
                                                   js_cocos2dx_extension_Downloader_constructor, 0, // constructor
                                                   NULL,
                                                   funcs,
                                                   NULL, // no static properties
                                                   st_funcs);
    
    anonEvaluate(cx, global, "(function () { return Downloader; })()").toObjectOrNull();
}
