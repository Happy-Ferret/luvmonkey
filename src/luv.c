#include "uv.h"
#include "luv.h"
#include "luv_handle.h"
#include "luv_stream.h"
#include "luv_tcp.h"

#ifndef PATH_MAX
#define PATH_MAX (8096)
#endif

static JSBool luv_run(JSContext *cx, uintN argc, jsval *vp) {
  uv_run(uv_default_loop());
  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_ref(JSContext *cx, uintN argc, jsval *vp) {
  uv_ref(uv_default_loop());
  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_unref(JSContext *cx, uintN argc, jsval *vp) {
  uv_unref(uv_default_loop());
  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_exepath(JSContext *cx, uintN argc, jsval *vp) {
  size_t size = 2*PATH_MAX;
  char exec_path[2*PATH_MAX];
  if (uv_exepath(exec_path, &size)) {
    uv_err_t err = uv_last_error(uv_default_loop());
    JS_ReportError(cx, "uv_exepath: %s", uv_strerror(err));
    return JS_FALSE;
  }
  JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyN(cx, exec_path, size)));
  return JS_TRUE;
}

static JSBool luv_get_free_memory(JSContext *cx, uintN argc, jsval *vp) {
  uint64_t size = uv_get_free_memory();
  JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(size));
  return JS_TRUE;
}

static JSBool luv_get_total_memory(JSContext *cx, uintN argc, jsval *vp) {
  uint64_t size = uv_get_total_memory();
  JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(size));
  return JS_TRUE;
}

static JSBool luv_loadavg(JSContext *cx, uintN argc, jsval *vp) {
  double avg[3];
  uv_loadavg(avg);
  jsval values[] = {
    DOUBLE_TO_JSVAL(avg[0]),
    DOUBLE_TO_JSVAL(avg[1]),
    DOUBLE_TO_JSVAL(avg[2])
  };
  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JS_NewArrayObject(cx, 3, values)));
  return JS_TRUE;
}

static JSBool luv_uptime(JSContext *cx, uintN argc, jsval *vp) {
  double uptime;
  uv_uptime(&uptime);
  JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(uptime));
  return JS_TRUE;
}

static JSFunctionSpec luv_functions[] = {
  JS_FS("run", luv_run, 0, JSPROP_ENUMERATE),
  JS_FS("ref", luv_ref, 0, JSPROP_ENUMERATE),
  JS_FS("unref", luv_unref, 0, JSPROP_ENUMERATE),
  JS_FS("exepath", luv_exepath, 0, JSPROP_ENUMERATE),
  JS_FS("get_free_memory", luv_get_free_memory, 0, JSPROP_ENUMERATE),
  JS_FS("get_total_memory", luv_get_total_memory, 0, JSPROP_ENUMERATE),
  JS_FS("loadavg", luv_loadavg, 0, JSPROP_ENUMERATE),
  JS_FS("uptime", luv_uptime, 0, JSPROP_ENUMERATE),
  JS_FS_END
};

int luv_init(JSContext* cx, JSObject *uv) {
  if (!JS_DefineFunctions(cx, uv, luv_functions)) {
    fprintf(stderr, "Error in define functions\n");
    return 1;
  }
  if (luv_handle_init(cx, uv)) {
    fprintf(stderr, "Error in handle_init\n");
    return 1;
  }
  if (luv_stream_init(cx, uv)) {
    fprintf(stderr, "Error in stream_init\n");
    return 1;
  }
  if (luv_tcp_init(cx, uv)) {
    fprintf(stderr, "Error in tcp_init\n");
    return 1;
  }
  return 0;
}


