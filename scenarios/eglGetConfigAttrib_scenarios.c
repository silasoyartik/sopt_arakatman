#include <EGL/egl.h>
#include <stdint.h>
#include <stdio.h>

/*
 * eglGetConfigAttrib(dpy, config, attribute, value) icin senaryolar.
 * dpy initialized, config ise ayni display'dan eglGetConfigs veya
 * eglChooseConfig ile alinmis olmalidir (negatif senaryolar haric).
 */

struct config_attribute {
    EGLint attribute;
    const char *name;
};

static const struct config_attribute egl10_attributes[] = {
    { EGL_BUFFER_SIZE,             "EGL_BUFFER_SIZE" },
    { EGL_RED_SIZE,                "EGL_RED_SIZE" },
    { EGL_GREEN_SIZE,              "EGL_GREEN_SIZE" },
    { EGL_BLUE_SIZE,               "EGL_BLUE_SIZE" },
    { EGL_ALPHA_SIZE,              "EGL_ALPHA_SIZE" },
    { EGL_CONFIG_CAVEAT,           "EGL_CONFIG_CAVEAT" },
    { EGL_CONFIG_ID,               "EGL_CONFIG_ID" },
    { EGL_DEPTH_SIZE,              "EGL_DEPTH_SIZE" },
    { EGL_LEVEL,                   "EGL_LEVEL" },
    { EGL_MAX_PBUFFER_WIDTH,       "EGL_MAX_PBUFFER_WIDTH" },
    { EGL_MAX_PBUFFER_HEIGHT,      "EGL_MAX_PBUFFER_HEIGHT" },
    { EGL_MAX_PBUFFER_PIXELS,      "EGL_MAX_PBUFFER_PIXELS" },
    { EGL_NATIVE_RENDERABLE,       "EGL_NATIVE_RENDERABLE" },
    { EGL_NATIVE_VISUAL_ID,        "EGL_NATIVE_VISUAL_ID" },
    { EGL_NATIVE_VISUAL_TYPE,      "EGL_NATIVE_VISUAL_TYPE" },
    { EGL_SAMPLE_BUFFERS,          "EGL_SAMPLE_BUFFERS" },
    { EGL_SAMPLES,                 "EGL_SAMPLES" },
    { EGL_STENCIL_SIZE,            "EGL_STENCIL_SIZE" },
    { EGL_SURFACE_TYPE,            "EGL_SURFACE_TYPE" },
    { EGL_TRANSPARENT_TYPE,        "EGL_TRANSPARENT_TYPE" },
    { EGL_TRANSPARENT_RED_VALUE,   "EGL_TRANSPARENT_RED_VALUE" },
    { EGL_TRANSPARENT_GREEN_VALUE, "EGL_TRANSPARENT_GREEN_VALUE" },
    { EGL_TRANSPARENT_BLUE_VALUE,  "EGL_TRANSPARENT_BLUE_VALUE" }
};

static const char *egl_error_name(EGLint error)
{
    switch (error) {
    case EGL_SUCCESS:         return "EGL_SUCCESS";
    case EGL_NOT_INITIALIZED: return "EGL_NOT_INITIALIZED";
    case EGL_BAD_ACCESS:      return "EGL_BAD_ACCESS";
    case EGL_BAD_ALLOC:       return "EGL_BAD_ALLOC";
    case EGL_BAD_ATTRIBUTE:   return "EGL_BAD_ATTRIBUTE";
    case EGL_BAD_CONFIG:      return "EGL_BAD_CONFIG";
    case EGL_BAD_CONTEXT:     return "EGL_BAD_CONTEXT";
    case EGL_BAD_DISPLAY:     return "EGL_BAD_DISPLAY";
    case EGL_BAD_MATCH:       return "EGL_BAD_MATCH";
    case EGL_BAD_PARAMETER:   return "EGL_BAD_PARAMETER";
    case EGL_BAD_SURFACE:     return "EGL_BAD_SURFACE";
    default:                  return "UNKNOWN_EGL_ERROR";
    }
}

static void report_query(const char *name, EGLBoolean result, EGLint error,
                         EGLint expected_error, EGLint value)
{
    EGLBoolean passed;

    if (expected_error == EGL_SUCCESS) {
        passed = (result == EGL_TRUE && error == EGL_SUCCESS);
    } else {
        passed = (result == EGL_FALSE && error == expected_error);
    }

    printf("%s: %s (result=%s, error=%s, value=%d)\n",
           name,
           passed ? "PASS" : "FAIL",
           result == EGL_TRUE ? "EGL_TRUE" : "EGL_FALSE",
           egl_error_name(error), value);
}

/* SENARYO 1 - Gecerli tek bir attribute sorgusu. */
void scenario_get_config_attrib_valid(EGLDisplay dpy,
                                      EGLConfig config,
                                      EGLint attribute)
{
    EGLint value = 0;
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglGetConfigAttrib(dpy, config, attribute, &value);
    error = eglGetError();
    report_query("Senaryo 1 - valid attribute", result, error,
                 EGL_SUCCESS, value);
}

/*
 * SENARYO 2 - EGL 1.0 Table 3.1'deki tum attribute'lari tek tek sorgulama.
 * Transparent RGB component degerleri EGL_TRANSPARENT_TYPE == EGL_NONE iken
 * tanimsizdir; fonksiyon basarili olsa bile bu degerler kullanilmamalidir.
 */
void scenario_get_config_attrib_all_egl10_attributes(EGLDisplay dpy,
                                                     EGLConfig config)
{
    unsigned int i;

    for (i = 0; i < sizeof(egl10_attributes) /
                    sizeof(egl10_attributes[0]); ++i) {
        EGLint value = 0;
        EGLBoolean result;
        EGLint error;

        (void)eglGetError();
        result = eglGetConfigAttrib(dpy, config,
                                    egl10_attributes[i].attribute, &value);
        error = eglGetError();
        report_query(egl10_attributes[i].name, result, error,
                     EGL_SUCCESS, value);
    }
}

/* SENARYO 3 - EGL_SURFACE_TYPE sonucunu integer esitligiyle degil bitmask ile kullanma. */
void scenario_get_config_attrib_surface_type_bits(EGLDisplay dpy,
                                                  EGLConfig config)
{
    EGLint value = 0;
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglGetConfigAttrib(dpy, config, EGL_SURFACE_TYPE, &value);
    error = eglGetError();
    report_query("Senaryo 3 - EGL_SURFACE_TYPE", result, error,
                 EGL_SUCCESS, value);

    if (result == EGL_TRUE) {
        printf("  WINDOW=%s PIXMAP=%s PBUFFER=%s\n",
               (value & EGL_WINDOW_BIT) ? "yes" : "no",
               (value & EGL_PIXMAP_BIT) ? "yes" : "no",
               (value & EGL_PBUFFER_BIT) ? "yes" : "no");
    }
}

/*
 * SENARYO 4 - Transparent RGB degerlerini yalnizca type uygunken okuma.
 * EGL_NONE durumunda component degerleri tanimsiz oldugundan sorgulanmaz.
 */
void scenario_get_config_attrib_transparency(EGLDisplay dpy, EGLConfig config)
{
    EGLint type = EGL_NONE;
    EGLint red = 0;
    EGLint green = 0;
    EGLint blue = 0;

    (void)eglGetError();
    if (eglGetConfigAttrib(dpy, config, EGL_TRANSPARENT_TYPE, &type) == EGL_FALSE) {
        printf("Senaryo 4 FAIL: type error=%s\n",
               egl_error_name(eglGetError()));
        return;
    }

    if (type == EGL_NONE) {
        printf("Senaryo 4 PASS: EGL_TRANSPARENT_TYPE=EGL_NONE\n");
        return;
    }

    if (type != EGL_TRANSPARENT_RGB) {
        printf("Senaryo 4 FAIL: bilinmeyen transparent type=%d\n", type);
        return;
    }

    if (eglGetConfigAttrib(dpy, config, EGL_TRANSPARENT_RED_VALUE, &red) &&
        eglGetConfigAttrib(dpy, config, EGL_TRANSPARENT_GREEN_VALUE, &green) &&
        eglGetConfigAttrib(dpy, config, EGL_TRANSPARENT_BLUE_VALUE, &blue)) {
        printf("Senaryo 4 PASS: transparent RGB=(%d, %d, %d)\n",
               red, green, blue);
    } else {
        printf("Senaryo 4 FAIL: component error=%s\n",
               egl_error_name(eglGetError()));
    }
}

/* SENARYO 5 - dpy == EGL_NO_DISPLAY. */
void scenario_get_config_attrib_no_display(EGLConfig config)
{
    EGLint value = 12345;
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglGetConfigAttrib(EGL_NO_DISPLAY, config, EGL_RED_SIZE, &value);
    error = eglGetError();
    report_query("Senaryo 5 - EGL_NO_DISPLAY", result, error,
                 EGL_BAD_DISPLAY, value);
}

/* SENARYO 6 - EGL tarafindan uretilmemis, gecersiz display handle'i. */
void scenario_get_config_attrib_invalid_display(EGLConfig config)
{
    EGLDisplay invalid_dpy = (EGLDisplay)(uintptr_t)1;
    EGLint value = 12345;
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglGetConfigAttrib(invalid_dpy, config, EGL_RED_SIZE, &value);
    error = eglGetError();
    report_query("Senaryo 6 - invalid display", result, error,
                 EGL_BAD_DISPLAY, value);
}

/* SENARYO 7 - Gecerli fakat initialize edilmemis display. */
void scenario_get_config_attrib_uninitialized_display(
    EGLDisplay uninitialized_dpy,
    EGLConfig config)
{
    EGLint value = 12345;
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglGetConfigAttrib(uninitialized_dpy, config,
                                EGL_RED_SIZE, &value);
    error = eglGetError();
    report_query("Senaryo 7 - uninitialized display", result, error,
                 EGL_NOT_INITIALIZED, value);
}

/* SENARYO 8 - EGL tarafindan uretilmemis, gecersiz config handle'i. */
void scenario_get_config_attrib_invalid_config(EGLDisplay dpy)
{
    EGLConfig invalid_config = (EGLConfig)(uintptr_t)1;
    EGLint value = 12345;
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglGetConfigAttrib(dpy, invalid_config, EGL_RED_SIZE, &value);
    error = eglGetError();
    report_query("Senaryo 8 - invalid config", result, error,
                 EGL_BAD_CONFIG, value);
}

/* SENARYO 9 - Config baska bir EGLDisplay'a ait. */
void scenario_get_config_attrib_config_from_another_display(
    EGLDisplay dpy,
    EGLConfig foreign_config)
{
    EGLint value = 12345;
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglGetConfigAttrib(dpy, foreign_config, EGL_RED_SIZE, &value);
    error = eglGetError();
    report_query("Senaryo 9 - config from another display", result, error,
                 EGL_BAD_CONFIG, value);
}

/* SENARYO 10 - EGL 1.0 Table 3.1'de olmayan attribute. */
void scenario_get_config_attrib_invalid_attribute(EGLDisplay dpy,
                                                  EGLConfig config)
{
    EGLint invalid_attribute = (EGLint)0x7fffffff;
    EGLint value = 12345;
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglGetConfigAttrib(dpy, config, invalid_attribute, &value);
    error = eglGetError();
    report_query("Senaryo 10 - invalid attribute", result, error,
                 EGL_BAD_ATTRIBUTE, value);
}

/*
 * SENARYO 11 - Terminate edilmis display ile eski config handle'ini sorgulama.
 * eglTerminate sonrasi config handle'larinin omru biter. dpy daha sonra tekrar
 * initialize edilirse eski config yine kullanilmamali, config yeniden alinmalidir.
 */
void scenario_get_config_attrib_after_terminate(EGLDisplay dpy,
                                               EGLConfig stale_config)
{
    EGLint value = 12345;
    EGLBoolean result;
    EGLint error;

    if (eglTerminate(dpy) == EGL_FALSE) {
        printf("Senaryo 11 kurulamadı: eglTerminate error=%s\n",
               egl_error_name(eglGetError()));
        return;
    }

    (void)eglGetError();
    result = eglGetConfigAttrib(dpy, stale_config, EGL_RED_SIZE, &value);
    error = eglGetError();
    report_query("Senaryo 11 - after terminate", result, error,
                 EGL_NOT_INITIALIZED, value);
}

/*
 * SENARYO 12 - value == NULL.
 * EGL 1.0 NULL output pointer icin bir hata sonucu tanimlamaz. Gercek cagrinin
 * davranisi tanimsizdir ve process crash olabilir; bu nedenle guvenli senaryo
 * cagrinin bilerek yapilmadigini gosterir.
 */
void scenario_get_config_attrib_null_value_is_invalid_usage(EGLDisplay dpy,
                                                            EGLConfig config)
{
    EGLint *value = NULL;

    (void)dpy;
    (void)config;
    (void)value;
    printf("Senaryo 12 SKIP: value=NULL ile eglGetConfigAttrib cagrilmaz.\n");
}

/*
 * SENARYO 13 - Terminate edilip yeniden initialize edilmis display'da eski
 * config handle'ini kullanma. Reinitialize eski handle'i diriltmez; fixture
 * stale_config'i terminate oncesinde alip dpy'yi yeniden initialize etmelidir.
 */
void scenario_get_config_attrib_stale_config_after_reinitialize(
    EGLDisplay reinitialized_dpy,
    EGLConfig stale_config)
{
    EGLint value = 12345;
    EGLBoolean result;
    EGLint error;

    (void)eglGetError();
    result = eglGetConfigAttrib(reinitialized_dpy, stale_config,
                                EGL_RED_SIZE, &value);
    error = eglGetError();
    report_query("Senaryo 13 - stale config after reinitialize", result,
                 error, EGL_BAD_CONFIG, value);
}
