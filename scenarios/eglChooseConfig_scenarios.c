#include <EGL/egl.h>
#include <stddef.h>
#include <stdio.h>

/*
 * All-in-one scenario file for eglChooseConfig(...).
 * The dpy parameter is assumed to have been initialized beforehand.
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
 * SCENARIO A - attrib_list=NULL.
 * Expected: EGL_TRUE and num_config >= 0.
 */
void scenario_a_null_attribute_list(EGLDisplay dpy)
{
    EGLint num_config = -1;

    if (eglChooseConfig(dpy, NULL, NULL, 0, &num_config) == EGL_TRUE) {
        printf("Scenario A passed: num_config=%d\n", num_config);
    } else {
        printf("Scenario A failed: %s\n",
               egl_error_name(eglGetError()));
    }
}


/*
 * SCENARIO B - RGB888 criteria.
 * Expected: EGL_TRUE; the number of matching configs is written to num_config.
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
        printf("Scenario B passed: num_config=%d\n", num_config);
    } else {
        printf("Scenario B failed: %s\n",
               egl_error_name(eglGetError()));
    }
}


/*
 * SCENARIO C - Excessively high criteria with no match.
 * Expected: EGL_TRUE and num_config=0.
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
        printf("Scenario C failed: %s\n",
               egl_error_name(eglGetError()));
        return;
    }

    if (num_config == 0) {
        printf("Scenario C passed: EGL_TRUE, num_config=0.\n");
    } else {
        printf("Scenario C returned a different result: num_config=%d\n", num_config);
    }
}


/*
 * SCENARIO D - config_size=1.
 * Expected: at most one EGLConfig is written to the buffer.
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
        printf("Scenario D passed: num_config=%d\n", num_config);
    } else {
        printf("Scenario D failed: %s\n",
               egl_error_name(eglGetError()));
    }
}


/*
 * SCENARIO E - dpy=EGL_NO_DISPLAY.
 * Expected: EGL_FALSE and EGL_BAD_DISPLAY.
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
            printf("Scenario E passed: received EGL_BAD_DISPLAY.\n");
        } else {
            printf("Scenario E returned a different error: %s\n",
                   egl_error_name(error));
        }
    } else {
        printf("Scenario E returned an unexpected result: EGL_TRUE.\n");
    }
}


/*
 * SCENARIO F - Unrecognized attribute.
 * Expected: EGL_FALSE and EGL_BAD_ATTRIBUTE.
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
            printf("Scenario F passed: received EGL_BAD_ATTRIBUTE.\n");
        } else {
            printf("Scenario F returned a different error: %s\n",
                   egl_error_name(error));
        }
    } else {
        printf("Scenario F returned an unexpected result: EGL_TRUE.\n");
    }
}


/*
 * SCENARIO G - num_config=NULL.
 * Expected: EGL_FALSE and EGL_BAD_PARAMETER.
 */
void scenario_g_null_num_config(EGLDisplay dpy)
{
    if (eglChooseConfig(dpy, NULL, NULL, 0, NULL) == EGL_FALSE) {
        EGLint error = eglGetError();

        if (error == EGL_BAD_PARAMETER) {
            printf("Scenario G passed: received EGL_BAD_PARAMETER.\n");
        } else {
            printf("Scenario G returned a different error: %s\n",
                   egl_error_name(error));
        }
    } else {
        printf("Scenario G returned an unexpected result: EGL_TRUE.\n");
    }
}
