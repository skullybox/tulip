#include <jni.h>
#include <string>
#include "tul_userc.h"
#include "tul_tls_client.h"
#include "tul_tls_common.h"



extern "C"
JNIEXPORT jint JNICALL
Java_org_tulip_project_tulip_Login_ClientLogin(JNIEnv *env, jobject instance, jstring user_,
                                               jstring pass_) {

    int ret = 0;
    tul_net_context conn;

    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);

    conn._use_tls = 1;
    strcpy(conn.tls.host,"127.0.0.1");

    ret |= tls_client_init(&(conn.tls), 9443);
   /* if (!ret)
        ret |= client_login((char *) user, (char *) pass, &conn);*/


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

