//
//  jsb_downloader.h
//  cocos2d_js_bindings
//
//  Created by 李强 on 15/11/19.
//
//

#ifndef __jsb_downloader__
#define __jsb_downloader__

#include "jsapi.h"
#include "jsfriendapi.h"

void register_jsb_downloader(JSContext* cx, JS::HandleObject global);

#endif /* __jsb_downloader__ */
