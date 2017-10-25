#include <jni.h>
#include <string>
#include "tul_net_context.h"

JNIEXPORT jint JNICALL
Java_org_tulip_project_tulip_Tulip_ClientLogin(JNIEnv *env, jobject instance, jstring user_,
                                               jstring pass_) {
    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);

    // TODO

    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_tulip_project_tulip_Login_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

