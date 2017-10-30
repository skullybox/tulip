#include <jni.h>
#include <string>
#include "tul_userc.h"
#include "tul_tls_client.h"
#include "tul_tls_common.h"


tul_net_context *conn = NULL;

extern "C"
JNIEXPORT jint JNICALL
Java_org_tulip_project_tulip_Login_ClientLogin(JNIEnv *env, jobject instance, jstring user_,
                                               jstring pass_) {

    int ret = 0;
    if (conn) {
        if (conn->_sock > 0)
            close(conn->_sock);
        free(conn);
    }
    conn = (tul_net_context *) calloc(1, sizeof(tul_net_context));


    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);

    conn->_use_tls = 1;
    strcpy(conn->tls.host, "stabby.no-ip.org");

    ret = tls_client_init(&(conn->tls), 9999);
    if (!ret)
        ret += client_login((char *) user, (char *) pass, conn);

    if (ret)
        if (conn)
        {
            free(conn);
            conn = NULL;
        }

    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);

    return ret;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_tulip_project_tulip_Login_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

