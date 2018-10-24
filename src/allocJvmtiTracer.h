#ifndef _ALLOCJVMTITRACER_H
#define _ALLOCJVMTITRACER_H

#include <jvmti.h>
#include "engine.h"
#include "vmEntry.h"

class AllocJvmtiTracer : public Engine {
  private:

  public:
    const char* name() {
        return "alloc_jvmti";
    }

    static void JNICALL SampledObjectAlloc(jvmtiEnv* jvmti_env, JNIEnv* jni_env, jthread thread, jobject object, jclass object_klass, jlong size);
    Error start(const char* event, long interval);
    void stop();

};

#endif // _ALLOCJVMTITRACER_H
