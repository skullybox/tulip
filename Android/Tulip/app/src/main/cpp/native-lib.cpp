#include <jni.h>
#include <string>
#include <vector>
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
        if (conn->tls.server_fd.fd > 0)
            close(conn->tls.server_fd.fd);
        free(conn);
        conn = NULL;
    }
    conn = (tul_net_context *) calloc(1, sizeof(tul_net_context));


    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);

    conn->_use_tls = 1;
    strcpy(conn->tls.host, "stabby.no-ip.org");

    ret |= tls_client_init(&(conn->tls), 9999);
    if (!ret)
        ret |= client_login((char *) user, (char *) pass, conn);
    if (!ret)
        ret |= client_transmit(conn);
    if (!ret)
        ret |= client_get_ok(conn, (char *) pass);

    if (ret)
        if (conn) {
            if (conn->tls.server_fd.fd > 0)
                close(conn->tls.server_fd.fd);
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
    std::vector<std::string> vlist;
    std::vector<std::string>::iterator i;
    client_get_friendlist((char *) user, (char *) pass, conn, &list, &list_sz, 0);

    while (list_sz > 0) {
        count += list_sz;
        memcpy(&offset, list, 8);
        for (int i = 0; i < 30 * list_sz; i += 30) {
            memcpy(t_str, &list[8 + i], 30);
            vlist.push_back(std::string(t_str));
        }
        if (list) {
            free(list);
            list = NULL;
        }
        client_get_friendlist((char *) user, (char *) pass, conn, &list, &list_sz, offset);
    }

    ret = (jobjectArray) env->NewObjectArray(vlist.size(),
                                             env->FindClass("java/lang/String"),
                                             env->NewStringUTF(""));

    count = 0;
    for (i = vlist.begin(); i != vlist.end(); ++i) {
        env->SetObjectArrayElement(ret, count,
                                   env->NewStringUTF(i->c_str()));
        count++;
    }

    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);

    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_tulip_project_tulip_MainActivity_Logout(JNIEnv *env, jobject instance, jstring user_,
                                                 jstring pass_) {
    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);

    client_logout((char *) user, (char *) pass, conn);
    client_transmit(conn);

    if (conn) {
        if (conn->tls.server_fd.fd > 0)
            close(conn->tls.server_fd.fd);
        free(conn);
        conn = NULL;
    }


    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_tulip_project_tulip_MainActivity_FriendRequest(JNIEnv *env, jobject instance,
                                                        jstring user_, jstring pass_,
                                                        jstring request_) {

    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);
    const char *request = env->GetStringUTFChars(request_, 0);

    int ret = 0;

    ret |= client_friend_req((char *) user, (char *) request, (char *) pass, conn);
    ret |= client_recieve(conn);

    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);
    env->ReleaseStringUTFChars(request_, request);
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_org_tulip_project_tulip_MainActivity_GetFriendRequest(JNIEnv *env, jobject instance,
                                                           jstring user_, jstring pass_) {
    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);

    jobjectArray ret;
    unsigned count = 0;

    char *list = NULL;
    char t_str[30] = {0};
    unsigned list_sz = 0;
    std::vector<std::string> rlist;
    std::vector<std::string>::iterator i;

    client_get_addreqlist((char *) user, (char *) pass, conn, &list, &list_sz);

    for (int i = 0; i < 30 * list_sz; i += 30) {
        memcpy(t_str, &list[i], 30);
        rlist.push_back(std::string(t_str));
    }

    ret = (jobjectArray) env->NewObjectArray(rlist.size(),
                                             env->FindClass("java/lang/String"),
                                             env->NewStringUTF(""));

    count = 0;
    for (i = rlist.begin(); i != rlist.end(); ++i) {
        env->SetObjectArrayElement(ret, count,
                                   env->NewStringUTF(i->c_str()));
        count++;
    }

    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_tulip_project_tulip_MainActivity_AcceptFriend(JNIEnv *env, jobject instance, jstring user_,
                                                       jstring pass_, jstring freq_) {
    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);
    const char *freq = env->GetStringUTFChars(freq_, 0);

    int ret = 0;

    ret = client_accept_friend((char *) user, (char *) pass, conn, (char *) freq);

    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);
    env->ReleaseStringUTFChars(freq_, freq);

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_tulip_project_tulip_MainActivity_IgnoreFriend(JNIEnv *env, jobject instance, jstring user_,
                                                       jstring pass_, jstring freq_) {
    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);
    const char *freq = env->GetStringUTFChars(freq_, 0);

    int ret = 0;

    ret = client_ignore_friend((char *) user, (char *) pass, conn, (char *) freq);

    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);
    env->ReleaseStringUTFChars(freq_, freq);

    return ret;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_org_tulip_project_tulip_MainActivity_GetMessage(JNIEnv *env, jobject instance, jstring user_,
                                                     jstring pass_, jstring frm_user_,
                                                     jobject offset) {
    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);
    const char *frm_user = env->GetStringUTFChars(frm_user_, 0);


    int ret = 0;
    unsigned _new = 0;
    unsigned long long _offset = 0;
    unsigned long long _ret_offset = 0;
    char uname[30] = {0};
    char frm[30] = {0};
    char typ[4] = {0};
    char *msg = NULL;
    char tmp[30] = {0};

    ret = client_get_message(
            (char *) user,
            (char *) frm_user,
            (char *) pass,
            conn,
            _offset,
            &msg,
            &_ret_offset,
            &_new,
            uname,
            frm,
            typ);

    jclass jmsg = env->FindClass("org/tulip/project/tulip/Message");
    jmethodID cons = env->GetMethodID(jmsg, "<init>",
                                      "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/math/BigInteger;ZLjava/lang/String;)V");

    jclass jbigint = env->FindClass("java/math/BigInteger");
    jmethodID jbigcons = env->GetMethodID(jbigint, "<init>", "(Ljava/lang/String;)V");

    jobject retObj = NULL;

    sprintf(tmp, "%llu", _ret_offset);

    if (jmsg && cons && jbigint && jbigcons) {
        jobject _msg = env->NewStringUTF(msg);
        jobject _uname = env->NewStringUTF(uname);
        jobject _frm = env->NewStringUTF(frm);
        jobject _jstrtmp = env->NewStringUTF(tmp);
        jobject _msg_offset = env->NewObject(jbigint, jbigcons, _jstrtmp);
        jobject _type = env->NewStringUTF(uname);


        if (_new)
            retObj = env->NewObject(jmsg, cons, _msg, _uname, _frm, _msg_offset, true, _type);
        else
            retObj = env->NewObject(jmsg, cons, _msg, _uname, _frm, _msg_offset, false, _type);

        env->DeleteLocalRef(_msg);
        env->DeleteLocalRef(_uname);
        env->DeleteLocalRef(_frm);
        env->DeleteLocalRef(_msg_offset);
        env->DeleteLocalRef(_type);
        env->DeleteLocalRef(_jstrtmp);
    }


    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);
    env->ReleaseStringUTFChars(frm_user_, frm_user);
    free(msg);

    if (ret)
        return NULL;

    return retObj;
}