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

    ret |= tls_client_init(&(conn->tls), 9999);
    if (!ret)
        ret |= client_login((char *) user, (char *) pass, conn);
    if(!ret)
        ret |= client_transmit(conn);
    if(!ret)
        ret |= client_get_ok(conn, (char*)pass);

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

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_org_tulip_project_tulip_MainActivity_GetList(JNIEnv *env, jobject instance, jstring user_,
                                                  jstring pass_) {
    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);

    jobjectArray ret;
    unsigned count = 0;
    unsigned long long offset = 0;

    char *list = NULL;
    char t_str[30] = {0};
    unsigned list_sz = 0;
    ret= (jobjectArray)env->NewObjectArray(500,
                                           env->FindClass("java/lang/String"),env->NewStringUTF(""));
    client_get_friendlist((char*)user, (char*)pass, conn, &list, &list_sz, 0);

    while(list_sz > 0 && count < 500)
    {
        count += list_sz;
        memcpy(&offset, list, 8);
        for(int i = 0; i < 30*list_sz; i+=30)
        {
            if(count +((i+30)/30) < 500)
            {
                memcpy(t_str, &list[8+i], 30);
                env->SetObjectArrayElement(ret,(count+(i+30)/30),env->NewStringUTF(t_str));
            }
        }
        client_get_friendlist((char*)user, (char*)pass, conn, &list, &list_sz, offset);
    }

    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);

    return ret;
}