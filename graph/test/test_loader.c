#include <stdlib.h>
#include "loader.h"
#include "test_factories.h"
#include "pdls.c"

static void new_stream_buffers(rage_Ports * ports, rage_InstanceSpec spec) {
    uint32_t i;
    for (i = 0; i < spec.inputs.len; i++) {
        ports->inputs[i] = calloc(256, sizeof(float));
    }
    for (i = 0; i < spec.outputs.len; i++) {
        ports->outputs[i] = calloc(256, sizeof(float));
    }
}

static void free_stream_buffers(rage_Ports * ports, rage_InstanceSpec spec) {
    uint32_t i;
    for (i = 0; i < spec.inputs.len; i++) {
        free((void *) ports->inputs[i]);
    }
    for (i = 0; i < spec.outputs.len; i++) {
        free((void *) ports->outputs[i]);
    }
}

typedef RAGE_ARRAY(rage_Interpolator *) rage_InterpolatorArray;
typedef RAGE_OR_ERROR(rage_InterpolatorArray) rage_Interpolators;

static void free_interpolators(rage_InterpolatorArray interpolators, rage_InstanceSpec spec) {
    for (uint32_t i = 0; i < interpolators.len; i++) {
        rage_interpolator_free(&spec.controls.items[i], interpolators.items[i]);
    }
    free(interpolators.items);
}

static rage_Interpolators new_interpolators(rage_Ports * ports, rage_InstanceSpec spec) {
    rage_InterpolatorArray interpolators;
    RAGE_ARRAY_INIT(&interpolators, spec.controls.len, i) {
        rage_TimeSeries ts = rage_time_series_new(&spec.controls.items[i]);
        rage_InitialisedInterpolator ii = rage_interpolator_new(
            &spec.controls.items[i], &ts, 44100, 1);
        rage_time_series_free(ts);
        if (RAGE_FAILED(ii)) {
            interpolators.len = i;
            free_interpolators(interpolators, spec);
            return RAGE_FAILURE(rage_Interpolators, RAGE_FAILURE_VALUE(ii));
        } else {
            interpolators.items[i] = RAGE_SUCCESS_VALUE(ii);
        }
        ports->controls[i] = rage_interpolator_get_view(interpolators.items[i], 0);
    }
    return RAGE_SUCCESS(rage_Interpolators, interpolators);
}

#define RAGE_AS_ERROR(errorable) (RAGE_FAILED(errorable)) ? \
    RAGE_ERROR(RAGE_FAILURE_VALUE(errorable)) : RAGE_OK

rage_Error test() {
    // FIXME: Error handling (and memory in those cases)
    rage_ElementLoader * el = rage_element_loader_new();
    rage_ElementTypes element_type_names = rage_element_loader_list(el);
    rage_Error err = RAGE_OK;
    for (unsigned i=0; i<element_type_names.len; i++) {
        rage_ElementTypeLoadResult et_ = rage_element_loader_load(
            el, element_type_names.items[i]);
        if (!RAGE_FAILED(et_)) {
            rage_ElementType * et = RAGE_SUCCESS_VALUE(et_);
            rage_Atom ** tups = generate_tuples(et->parameters);
            rage_ElementNewResult elem_ = rage_element_new(et, 44100, 256, tups);
            if (!RAGE_FAILED(elem_)) {
                rage_Element * elem = RAGE_SUCCESS_VALUE(elem_);
                rage_Ports ports = rage_ports_new(&elem->spec);
                new_stream_buffers(&ports, elem->spec);
                rage_Interpolators ii = new_interpolators(&ports, elem->spec);
                if (!RAGE_FAILED(ii)) {
                    rage_InterpolatorArray interpolators = RAGE_SUCCESS_VALUE(ii);
                    rage_element_process(elem, RAGE_TRANSPORT_ROLLING, &ports);
                    free_interpolators(interpolators, elem->spec);
                } else {
                    err = RAGE_AS_ERROR(ii);
                }
                free_stream_buffers(&ports, elem->spec);
                rage_ports_free(ports);
                rage_element_free(elem);
            } else {
                err = RAGE_AS_ERROR(elem_);
            }
            free_tuples(et->parameters, tups);
            rage_element_loader_unload(el, et);
        } else {
            err = RAGE_AS_ERROR(et_);
        }
        if (RAGE_FAILED(err)) {
            break;
        }
    }
    rage_element_loader_free(el);
    return err;
}
