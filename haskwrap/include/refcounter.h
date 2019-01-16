#pragma once

// Non-existant type used to avoid void autocasting:
typedef struct rage_hs_internal_RefCounted rage_hs_internal_RefCounted;

typedef void (*rage_hs_Deallocator)(rage_hs_internal_RefCounted *);
typedef struct rage_hs_RefCount rage_hs_RefCount;

#define RAGE_HS_COUNTABLE(real_type, external_type) union {\
    rage_hs_RefCount * rc; \
    real_type * type; \
    void (*dealloc)(real_type *); \
    external_type * external;}

rage_hs_RefCount * rage_hs_count_ref(
    rage_hs_Deallocator dealloc, rage_hs_internal_RefCounted * ref);
#define RAGE_HS_COUNT(result, countable_t, f, p) \
    countable_t result; \
    result.type = p; \
    result.dealloc = f; \
    result.rc = rage_hs_count_ref( \
        (rage_hs_Deallocator) f, (rage_hs_internal_RefCounted *) p);

void rage_hs_depend_ref(rage_hs_RefCount * depender, rage_hs_RefCount * dependee);
#define RAGE_HS_DEPEND_REF(depender, dependee) rage_hs_depend_ref(depender.rc, dependee.rc)
void rage_hs_decrement_ref(rage_hs_RefCount * ref);
#define RAGE_HS_DECREMENT_REF(countable) rage_hs_decrement_ref(countable.rc)

rage_hs_internal_RefCounted * rage_hs_ref(rage_hs_RefCount const * rc);
#define RAGE_HS_REF(countable) ((typeof(countable.type)) rage_hs_ref(countable.rc))