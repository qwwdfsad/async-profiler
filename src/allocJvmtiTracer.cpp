#include <jvmti.h>
#include "engine.h"
#include "allocJvmtiTracer.h"
#include "vmStructs.h"
#include "profiler.h"

Error AllocJvmtiTracer::start(const char* event, long interval) {
    jvmtiEnv* jvmti = VM::jvmti();
    jvmtiCapabilities capabilities = {0};
    capabilities.can_generate_sampled_object_alloc_events = 1;
    jvmtiError error = jvmti->AddCapabilities(&capabilities);
    if (error == JVMTI_ERROR_NOT_AVAILABLE) {
        return Error("JVMTI heap sampling is not supported (optionally implemented since Java 11)");
    }

    jvmtiEventCallbacks callbacks = {0};
    callbacks.SampledObjectAlloc = AllocJvmtiTracer::SampledObjectAlloc;
    jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));

    long sampling_interval = interval ? interval : DEFAULT_HEAP_SAMPLING_FREQUENCY; // Every megabyte by default
    jvmti->SetHeapSamplingInterval(sampling_interval);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_SAMPLED_OBJECT_ALLOC, NULL);
    return Error::OK;
}

void AllocJvmtiTracer::stop() {
    jvmtiEnv* jvmti = VM::jvmti();
    jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_SAMPLED_OBJECT_ALLOC, NULL);
}

void JNICALL AllocJvmtiTracer::SampledObjectAlloc(jvmtiEnv* jvmti_env, JNIEnv* jni_env, jthread thread, jobject object, jclass object_klass, jlong size) {
    VMSymbol* class_name = (*(java_lang_Class**)object_klass)->klass()->name();
    Profiler::_instance.recordSample(NULL, size, BCI_SYMBOL, (jmethodID) class_name);
}
