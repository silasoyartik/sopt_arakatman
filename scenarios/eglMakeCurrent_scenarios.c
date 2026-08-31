#include <EGL/egl.h>
#include <stdint.h>
#include <stdio.h>

/*
 * eglMakeCurrent(dpy, draw, read, ctx) icin all-in-one senaryo dosyasi.
 * Surface ve context olusturma ayrica yapilir; fonksiyonlar gerekli fixture
 * nesnelerini parametre olarak alir. Thread/resource hatalarinin onkosullari
 * ilgili senaryolarin yorumlarinda belirtilmistir.
 */

static const char *egl_error_name(EGLint error)
{
    switch (error) {
    case EGL_SUCCESS:            return "EGL_SUCCESS";
    case EGL_NOT_INITIALIZED:    return "EGL_NOT_INITIALIZED";
    case EGL_BAD_ACCESS:         return "EGL_BAD_ACCESS";
    case EGL_BAD_ALLOC:          return "EGL_BAD_ALLOC";
    case EGL_BAD_ATTRIBUTE:      return "EGL_BAD_ATTRIBUTE";
    case EGL_BAD_CONFIG:         return "EGL_BAD_CONFIG";
    case EGL_BAD_CONTEXT:        return "EGL_BAD_CONTEXT";
    case EGL_BAD_CURRENT_SURFACE:return "EGL_BAD_CURRENT_SURFACE";
    case EGL_BAD_DISPLAY:        return "EGL_BAD_DISPLAY";
    case EGL_BAD_MATCH:          return "EGL_BAD_MATCH";
    case EGL_BAD_NATIVE_PIXMAP:  return "EGL_BAD_NATIVE_PIXMAP";
    case EGL_BAD_NATIVE_WINDOW:  return "EGL_BAD_NATIVE_WINDOW";
    case EGL_BAD_PARAMETER:      return "EGL_BAD_PARAMETER";
    case EGL_BAD_SURFACE:        return "EGL_BAD_SURFACE";
    default:                     return "UNKNOWN_EGL_ERROR";
    }
}

static void run_make_current(const char *name,
                             EGLDisplay dpy,
                             EGLSurface draw,
                             EGLSurface read,
                             EGLContext ctx,
                             EGLint expected_error)
{
    EGLBoolean result;
    EGLBoolean passed;
    EGLint error;

    (void)eglGetError();
    result = eglMakeCurrent(dpy, draw, read, ctx);
    error = eglGetError();

    if (expected_error == EGL_SUCCESS) {
        passed = (result == EGL_TRUE && error == EGL_SUCCESS);
    } else {
        passed = (result == EGL_FALSE && error == expected_error);
    }

    printf("%s: %s (result=%s, error=%s)\n",
           name,
           passed ? "PASS" : "FAIL",
           result == EGL_TRUE ? "EGL_TRUE" : "EGL_FALSE",
           egl_error_name(error));
}

/* SENARYO 1 - Ayni surface'i draw ve read olarak normal bind etme. */
void scenario_make_current_same_draw_read(EGLDisplay dpy,
                                          EGLSurface surface,
                                          EGLContext ctx)
{
    run_make_current("Senaryo 1 - same draw/read", dpy, surface, surface,
                     ctx, EGL_SUCCESS);
}

/* SENARYO 2 - Context ile uyumlu iki farkli draw/read surface. */
void scenario_make_current_separate_draw_read(EGLDisplay dpy,
                                              EGLSurface draw_surface,
                                              EGLSurface read_surface,
                                              EGLContext ctx)
{
    run_make_current("Senaryo 2 - separate draw/read", dpy, draw_surface,
                     read_surface, ctx, EGL_SUCCESS);
}

/* SENARYO 3 - Current context ve iki surface'i dogru kombinasyonla release. */
void scenario_make_current_release(EGLDisplay dpy)
{
    run_make_current("Senaryo 3 - release", dpy, EGL_NO_SURFACE,
                     EGL_NO_SURFACE, EGL_NO_CONTEXT, EGL_SUCCESS);
}

/* SENARYO 4 - dpy == EGL_NO_DISPLAY. */
void scenario_make_current_no_display(EGLSurface surface, EGLContext ctx)
{
    run_make_current("Senaryo 4 - EGL_NO_DISPLAY", EGL_NO_DISPLAY,
                     surface, surface, ctx, EGL_BAD_DISPLAY);
}

/* SENARYO 5 - EGL tarafindan uretilmemis, gecersiz display handle'i. */
void scenario_make_current_invalid_display(EGLSurface surface, EGLContext ctx)
{
    EGLDisplay invalid_dpy = (EGLDisplay)(uintptr_t)1;

    run_make_current("Senaryo 5 - invalid display", invalid_dpy,
                     surface, surface, ctx, EGL_BAD_DISPLAY);
}

/* SENARYO 6 - eglGetDisplay ile alinmis fakat initialize edilmemis display. */
void scenario_make_current_uninitialized_display(EGLDisplay uninitialized_dpy,
                                                 EGLSurface surface,
                                                 EGLContext ctx)
{
    run_make_current("Senaryo 6 - uninitialized display", uninitialized_dpy,
                     surface, surface, ctx, EGL_NOT_INITIALIZED);
}

/* SENARYO 7 - Gercek surface'ler ile EGL_NO_CONTEXT kullanmak gecersizdir. */
void scenario_make_current_no_context_with_surfaces(EGLDisplay dpy,
                                                    EGLSurface surface)
{
    run_make_current("Senaryo 7 - no context with surfaces", dpy, surface,
                     surface, EGL_NO_CONTEXT, EGL_BAD_MATCH);
}

/* SENARYO 8 - Context varken draw == EGL_NO_SURFACE. */
void scenario_make_current_missing_draw(EGLDisplay dpy,
                                        EGLSurface read_surface,
                                        EGLContext ctx)
{
    run_make_current("Senaryo 8 - missing draw", dpy, EGL_NO_SURFACE,
                     read_surface, ctx, EGL_BAD_MATCH);
}

/* SENARYO 9 - Context varken read == EGL_NO_SURFACE. */
void scenario_make_current_missing_read(EGLDisplay dpy,
                                        EGLSurface draw_surface,
                                        EGLContext ctx)
{
    run_make_current("Senaryo 9 - missing read", dpy, draw_surface,
                     EGL_NO_SURFACE, ctx, EGL_BAD_MATCH);
}

/* SENARYO 10 - Context varken hem draw hem read EGL_NO_SURFACE. */
void scenario_make_current_context_without_surfaces(EGLDisplay dpy,
                                                    EGLContext ctx)
{
    run_make_current("Senaryo 10 - context without surfaces", dpy,
                     EGL_NO_SURFACE, EGL_NO_SURFACE, ctx, EGL_BAD_MATCH);
}

/* SENARYO 11 - EGL tarafindan uretilmemis draw surface handle'i. */
void scenario_make_current_invalid_draw(EGLDisplay dpy,
                                        EGLSurface valid_read,
                                        EGLContext ctx)
{
    EGLSurface invalid_draw = (EGLSurface)(uintptr_t)1;

    run_make_current("Senaryo 11 - invalid draw", dpy, invalid_draw,
                     valid_read, ctx, EGL_BAD_SURFACE);
}

/* SENARYO 12 - EGL tarafindan uretilmemis read surface handle'i. */
void scenario_make_current_invalid_read(EGLDisplay dpy,
                                        EGLSurface valid_draw,
                                        EGLContext ctx)
{
    EGLSurface invalid_read = (EGLSurface)(uintptr_t)1;

    run_make_current("Senaryo 12 - invalid read", dpy, valid_draw,
                     invalid_read, ctx, EGL_BAD_SURFACE);
}

/* SENARYO 13 - EGL tarafindan uretilmemis non-null context handle'i. */
void scenario_make_current_invalid_context(EGLDisplay dpy,
                                           EGLSurface surface)
{
    EGLContext invalid_ctx = (EGLContext)(uintptr_t)1;

    run_make_current("Senaryo 13 - invalid context", dpy, surface, surface,
                     invalid_ctx, EGL_BAD_CONTEXT);
}

/*
 * SENARYO 14 - Draw surface'in color/ancillary buffer yapisi ctx ile uyumsuz.
 * incompatible_draw ayni display'da farkli ve uyumsuz config ile uretilmelidir.
 */
void scenario_make_current_incompatible_draw(EGLDisplay dpy,
                                             EGLSurface incompatible_draw,
                                             EGLSurface compatible_read,
                                             EGLContext ctx)
{
    run_make_current("Senaryo 14 - incompatible draw", dpy,
                     incompatible_draw, compatible_read, ctx, EGL_BAD_MATCH);
}

/*
 * SENARYO 15 - Read surface'in color/ancillary buffer yapisi ctx ile uyumsuz.
 */
void scenario_make_current_incompatible_read(EGLDisplay dpy,
                                             EGLSurface compatible_draw,
                                             EGLSurface incompatible_read,
                                             EGLContext ctx)
{
    run_make_current("Senaryo 15 - incompatible read", dpy, compatible_draw,
                     incompatible_read, ctx, EGL_BAD_MATCH);
}

/*
 * SENARYO 16 - Nesneler ayni display'a ait degil.
 * foreign_surface baska bir EGLDisplay ile olusturulmus olmalidir.
 */
void scenario_make_current_surface_from_another_display(
    EGLDisplay dpy,
    EGLSurface foreign_surface,
    EGLContext ctx)
{
    run_make_current("Senaryo 16 - surface from another display", dpy,
                     foreign_surface, foreign_surface, ctx, EGL_BAD_MATCH);
}

/*
 * SENARYO 17 - Context bu display'a ait degil.
 * foreign_ctx baska bir EGLDisplay ile olusturulmus olmalidir.
 */
void scenario_make_current_context_from_another_display(
    EGLDisplay dpy,
    EGLSurface surface,
    EGLContext foreign_ctx)
{
    run_make_current("Senaryo 17 - context from another display", dpy,
                     surface, surface, foreign_ctx, EGL_BAD_MATCH);
}

/*
 * SENARYO 18 - Context baska thread'de current.
 * Fixture worker thread'de ctx'yi current tutarken bu fonksiyonu baska bir
 * thread'de cagirmalidir. Worker release etmeden EGL_BAD_ACCESS beklenir.
 */
void scenario_make_current_context_current_on_another_thread(
    EGLDisplay dpy,
    EGLSurface surface,
    EGLContext ctx)
{
    run_make_current("Senaryo 18 - context owned by another thread", dpy,
                     surface, surface, ctx, EGL_BAD_ACCESS);
}

/*
 * SENARYO 19 - Draw surface baska thread'de bir context'e bagli.
 * ctx burada ikinci, kullanilmayan ve surface ile uyumlu bir context olmalidir.
 */
void scenario_make_current_draw_owned_by_another_thread(
    EGLDisplay dpy,
    EGLSurface busy_draw,
    EGLSurface free_read,
    EGLContext ctx)
{
    run_make_current("Senaryo 19 - draw owned by another thread", dpy,
                     busy_draw, free_read, ctx, EGL_BAD_ACCESS);
}

/* SENARYO 20 - Read surface baska thread'de bir context'e bagli. */
void scenario_make_current_read_owned_by_another_thread(
    EGLDisplay dpy,
    EGLSurface free_draw,
    EGLSurface busy_read,
    EGLContext ctx)
{
    run_make_current("Senaryo 20 - read owned by another thread", dpy,
                     free_draw, busy_read, ctx, EGL_BAD_ACCESS);
}

/*
 * SENARYO 21 - Alttaki native window yok edilmis window surface.
 * EGL 1.0 gecersiz native nesneyi her platformda algilamayi garanti etmez.
 */
void scenario_make_current_destroyed_native_window(EGLDisplay dpy,
                                                   EGLSurface affected_surface,
                                                   EGLContext ctx)
{
    run_make_current("Senaryo 21 - destroyed native window", dpy,
                     affected_surface, affected_surface, ctx,
                     EGL_BAD_NATIVE_WINDOW);
}

/*
 * SENARYO 22 - Onceki current surface gecersizken eski context'i flush etme.
 * Fixture once current surface'in native nesnesini yok etmeli ve flush edilmemis
 * GL komutlari birakmalidir. Bu kosul driver/platform bagimlidir.
 */
void scenario_make_current_bad_previous_current_surface(
    EGLDisplay dpy,
    EGLSurface new_surface,
    EGLContext new_ctx)
{
    run_make_current("Senaryo 22 - bad previous current surface", dpy,
                     new_surface, new_surface, new_ctx,
                     EGL_BAD_CURRENT_SURFACE);
}

/*
 * SENARYO 23 - Ancillary buffer allocation hatasi.
 * Fixture/driver draw-read buffer'lari icin allocation'i kontrollu olarak
 * basarisiz kilmalidir (ornegin fault injection). Normal kod bunu garanti
 * ederek uretemez.
 */
void scenario_make_current_ancillary_allocation_failure(
    EGLDisplay dpy,
    EGLSurface draw,
    EGLSurface read,
    EGLContext ctx)
{
    run_make_current("Senaryo 23 - ancillary allocation failure", dpy,
                     draw, read, ctx, EGL_BAD_ALLOC);
}

/*
 * SENARYO 24 - Draw/read birlikte desteklenemiyor veya bellekte barinamiyor.
 * Driver fault-injection ya da bu kosulu gercekten olusturan fixture gerekir.
 */
void scenario_make_current_draw_read_resource_mismatch(
    EGLDisplay dpy,
    EGLSurface draw,
    EGLSurface read,
    EGLContext ctx)
{
    run_make_current("Senaryo 24 - draw/read resource mismatch", dpy,
                     draw, read, ctx, EGL_BAD_MATCH);
}

/*
 * SENARYO 25 - Current baglantiyi baska uyumlu context/surface'e gecirme.
 * Basarili cagri eski context'i flush eder ve yeni dortluyu thread'e baglar.
 */
void scenario_make_current_switch_binding(EGLDisplay dpy,
                                          EGLSurface old_surface,
                                          EGLContext old_ctx,
                                          EGLSurface new_surface,
                                          EGLContext new_ctx)
{
    run_make_current("Senaryo 25a - establish old binding", dpy, old_surface,
                     old_surface, old_ctx, EGL_SUCCESS);
    run_make_current("Senaryo 25b - switch binding", dpy, new_surface,
                     new_surface, new_ctx, EGL_SUCCESS);
}
