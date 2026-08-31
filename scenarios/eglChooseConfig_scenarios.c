#include <EGL/egl.h>
#include <stddef.h>

/*
 * eglChooseConfig(...) icin all-in-one senaryo dosyasi.
 * dpy parametresinin onceden initialize edilmis oldugu varsayilir.
 */

static const char *egl_error_name(EGLint error)
{
    switch (error) {
    case EGL_SUCCESS:         return "EGL_SUCCESS";
    case EGL_NOT_INITIALIZED: return "EGL_NOT_INITIALIZED";
    case EGL_BAD_ACCESS:      return "EGL_BAD_ACCESS";
    case EGL_BAD_ALLOC:       return "EGL_BAD_ALLOC";
    case EGL_BAD_ATTRIBUTE:   return "EGL_BAD_ATTRIBUTE";
    case EGL_BAD_CONTEXT:     return "EGL_BAD_CONTEXT";
    case EGL_BAD_CONFIG:      return "EGL_BAD_CONFIG";
    case EGL_BAD_SURFACE:     return "EGL_BAD_SURFACE";
    case EGL_BAD_DISPLAY:     return "EGL_BAD_DISPLAY";
    case EGL_BAD_MATCH:       return "EGL_BAD_MATCH";
    case EGL_BAD_PARAMETER:   return "EGL_BAD_PARAMETER";
    default:                  return "UNKNOWN_EGL_ERROR";
    }
}


/*
 * SENARYO A - attrib_list=NULL.
 * Beklenen: EGL_TRUE ve num_config >= 0.
 */
void scenario_a_null_attribute_list(EGLDisplay dpy)
{
    EGLint num_config = -1;

    if (eglChooseConfig(dpy, NULL, NULL, 0, &num_config) == EGL_TRUE) {
        printf("Senaryo A basarili: num_config=%d\n", num_config);
    } else {
        printf("Senaryo A hatali: %s\n",
               egl_error_name(eglGetError()));
    }
}


/*
 * SENARYO B - RGB888 kriterleri.
 * Beklenen: EGL_TRUE; eslesen config sayisi num_config'a yazilir.
 */
void scenario_b_rgb888(EGLDisplay dpy)
{
    const EGLint attrs[] = {
        EGL_RED_SIZE,   8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE,  8,
        EGL_NONE
    };

    EGLint num_config = -1;

    if (eglChooseConfig(dpy, attrs, NULL, 0, &num_config) == EGL_TRUE) {
        printf("Senaryo B basarili: num_config=%d\n", num_config);
    } else {
        printf("Senaryo B hatali: %s\n",
               egl_error_name(eglGetError()));
    }
}


/*
 * SENARYO C - Eslesmeyen cok yuksek kriterler.
 * Beklenen: EGL_TRUE ve num_config=0.
 */
void scenario_c_no_matching_config(EGLDisplay dpy)
{
    const EGLint attrs[] = {
        EGL_RED_SIZE,     64,
        EGL_GREEN_SIZE,   64,
        EGL_BLUE_SIZE,    64,
        EGL_DEPTH_SIZE,  128,
        EGL_STENCIL_SIZE, 64,
        EGL_NONE
    };

    EGLint num_config = -1;

    if (eglChooseConfig(dpy, attrs, NULL, 0, &num_config) == EGL_FALSE) {
        printf("Senaryo C hatali: %s\n",
               egl_error_name(eglGetError()));
        return;
    }

    if (num_config == 0) {
        printf("Senaryo C basarili: EGL_TRUE, num_config=0.\n");
    } else {
        printf("Senaryo C farkli sonuc: num_config=%d\n", num_config);
    }
}


/*
 * SENARYO D - config_size=1.
 * Beklenen: buffer'a en fazla bir EGLConfig yazilir.
 */
void scenario_d_limited_output_buffer(EGLDisplay dpy)
{
    const EGLint attrs[] = {
        EGL_RED_SIZE,   8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE,  8,
        EGL_NONE
    };

    EGLConfig configs[1];
    EGLint num_config = -1;

    if (eglChooseConfig(dpy, attrs, configs, 1, &num_config) == EGL_TRUE) {
        printf("Senaryo D basarili: num_config=%d\n", num_config);
    } else {
        printf("Senaryo D hatali: %s\n",
               egl_error_name(eglGetError()));
    }
}


/*
 * SENARYO E - dpy=EGL_NO_DISPLAY.
 * Beklenen: EGL_FALSE ve EGL_BAD_DISPLAY.
 */
void scenario_e_invalid_display(void)
{
    EGLint num_config = -1;

    if (eglChooseConfig(
            EGL_NO_DISPLAY,
            NULL,
            NULL,
            0,
            &num_config) == EGL_FALSE) {

        EGLint error = eglGetError();

        if (error == EGL_BAD_DISPLAY) {
            printf("Senaryo E basarili: EGL_BAD_DISPLAY alindi.\n");
        } else {
            printf("Senaryo E farkli hata: %s\n",
                   egl_error_name(error));
        }
    } else {
        printf("Senaryo E beklenmeyen sonuc: EGL_TRUE dondu.\n");
    }
}


/*
 * SENARYO F - Taninmayan attribute.
 * Beklenen: EGL_FALSE ve EGL_BAD_ATTRIBUTE.
 */
void scenario_f_invalid_attribute(EGLDisplay dpy)
{
    const EGLint attrs[] = {
        0x7FFFFFFF, 1,
        EGL_NONE
    };

    EGLint num_config = -1;

    if (eglChooseConfig(dpy, attrs, NULL, 0, &num_config) == EGL_FALSE) {
        EGLint error = eglGetError();

        if (error == EGL_BAD_ATTRIBUTE) {
            printf("Senaryo F basarili: EGL_BAD_ATTRIBUTE alindi.\n");
        } else {
            printf("Senaryo F farkli hata: %s\n",
                   egl_error_name(error));
        }
    } else {
        printf("Senaryo F beklenmeyen sonuc: EGL_TRUE dondu.\n");
    }
}


/*
 * SENARYO G - num_config=NULL.
 * Beklenen: EGL_FALSE ve EGL_BAD_PARAMETER.
 */
void scenario_g_null_num_config(EGLDisplay dpy)
{
    if (eglChooseConfig(dpy, NULL, NULL, 0, NULL) == EGL_FALSE) {
        EGLint error = eglGetError();

        if (error == EGL_BAD_PARAMETER) {
            printf("Senaryo G basarili: EGL_BAD_PARAMETER alindi.\n");
        } else {
            printf("Senaryo G farkli hata: %s\n",
                   egl_error_name(error));
        }
    } else {
        printf("Senaryo G beklenmeyen sonuc: EGL_TRUE dondu.\n");
    }
}