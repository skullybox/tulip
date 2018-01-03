#include <jni.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include "tul_userc.h"
#include "tul_tls_client.h"
#include "tul_tls_common.h"


tul_net_context *conn = NULL;
tul_net_context *message_conn = NULL;
// char host_string[100] = "stabby.no-ip.org";
char host_string[100] = "skullybox.no-ip.net";

extern "C"
int connection_reset(char *user, char *pass, tul_net_context **_conn){

    
    int ret = 0;
    if (*_conn) {
        if ((*_conn)->tls.server_fd.fd > 0)
            close((*_conn)->tls.server_fd.fd);
        free(*_conn);
        *_conn = NULL;
    }
    *_conn = (tul_net_context *) calloc(1, sizeof(tul_net_context));

    (*_conn)->_use_tls = 1;
    strcpy((*_conn)->tls.host, host_string);

    ret |= tls_client_init(&((*_conn)->tls), 9999);
    if (!ret)
        ret |= client_login((char *) user, (char *) pass, *_conn);
    if (!ret)
        ret |= client_transmit(*_conn);
    if (!ret)
        ret |= client_get_ok(*_conn, (char *) pass);

    
    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_tulip_project_tulip_Login_ClientLogin(JNIEnv *env, jobject instance, jstring user_,
                                               jstring pass_) {

    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);

    int ret = connection_reset((char*)user, (char*) pass, &conn);

    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);

    return ret;
}

extern "C"
void connection_logout(char *user, char *pass, tul_net_context **_conn)
{
    client_logout((char *) user, (char *) pass, *_conn);
    client_transmit(*_conn);
    if (*_conn) {
        if ((*_conn)->tls.server_fd.fd > 0)
            close((*_conn)->tls.server_fd.fd);
        free(*_conn);
        *_conn = NULL;
    }
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

    tul_net_context *conn_userlist = NULL;
    int net_ret = connection_reset((char *) user, (char *) pass, &conn_userlist);
    if(!net_ret)
        client_get_friendlist((char *) user, (char *) pass, conn_userlist, &list, &list_sz, 0);
    else
        goto RETURN_LIST;
    

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
        
        client_get_friendlist((char *) user, (char *) pass, conn_userlist, &list, &list_sz, offset);
        
    }

    ret = (jobjectArray) env->NewObjectArray(vlist.size(),
                                             env->FindClass("java/lang/String"),
                                             env->NewStringUTF(""));
RETURN_LIST:
    connection_logout((char *) user, (char *) pass, &conn_userlist);
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

    connection_logout((char *) user, (char *) pass, &conn);

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
    // using a separate connection for sending messages
    // so that there isn't a race condition with background
    // processes
    tul_net_context *lconn = NULL;
    ret = connection_reset((char *) user, (char *) pass, &lconn);

    ret |= client_friend_req((char *) user, (char *) request, (char *) pass, lconn);
    ret |= client_recieve(lconn);

    connection_logout((char *) user, (char *) pass, &lconn);

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
    tul_net_context *lconn = NULL;

    int _ret = connection_reset((char*)user, (char*) pass, &lconn);

    if(!_ret)
    {
        client_get_addreqlist((char *) user, (char *) pass, lconn, &list, &list_sz);

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

        connection_logout((char *) user, (char *) pass, &lconn);
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
    tul_net_context *lconn = NULL;

    ret = connection_reset((char*)user, (char*) pass, &lconn);
    if(!ret)
        ret = client_accept_friend((char *) user, (char *) pass, lconn, (char *) freq);
    
    connection_logout((char*)user, (char*) pass, &lconn);

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
    tul_net_context *lconn = NULL;

    ret = connection_reset((char*)user, (char*) pass, &lconn);
    if(!ret)
        ret = client_ignore_friend((char *) user, (char *) pass, lconn, (char *) freq);

    connection_logout((char*)user, (char*) pass, &lconn);

    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);
    env->ReleaseStringUTFChars(freq_, freq);

    return ret;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_org_tulip_project_tulip_MainActivity_GetMessage(JNIEnv *env, jobject instance, jstring user_,
                                                     jstring pass_, jstring frm_user_,
                                                     jstring offset) {
    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);
    const char *frm_user = env->GetStringUTFChars(frm_user_, 0);
    const char *bigint_str = env->GetStringUTFChars(offset, 0);


    int ret = 0;
    unsigned _new = 0;
    unsigned long long _offset = 0;
    unsigned long long _ret_offset = 0;
    char uname[30] = {0};
    char frm[30] = {0};
    char typ[4] = {0};
    char *msg = NULL;
    char tmp[30] = {0};

    //big int stuff
    _offset = strtoull(bigint_str,(char **)NULL, 10 );

    
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
    

    jobject retObj = NULL;

    if(strlen(uname) == 0 || ret)
    {
        connection_reset((char*)user, (char*) pass, &conn);
        env->ReleaseStringUTFChars(user_, user);
        env->ReleaseStringUTFChars(pass_, pass);
        env->ReleaseStringUTFChars(frm_user_, frm_user);
        env->ReleaseStringUTFChars(offset, bigint_str);
        return retObj;
    }

    jclass jmsg = env->FindClass("org/tulip/project/tulip/Message");
    jmethodID cons = env->GetMethodID(jmsg, "<init>",
                                      "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/math/BigInteger;ZLjava/lang/String;)V");

    jclass jbigint = env->FindClass("java/math/BigInteger");
    jmethodID jbigcons = env->GetMethodID(jbigint, "<init>", "(Ljava/lang/String;)V");


    sprintf(tmp, "%llu", _ret_offset);

    if (jmsg && cons && jbigint && jbigcons) {
        jobject _msg;
        if(msg)
            _msg = env->NewStringUTF(msg);
        else
           _msg = env->NewStringUTF("");

        jobject _uname = env->NewStringUTF(uname);
        jobject _frm = env->NewStringUTF(frm);
        jobject _jstrtmp = env->NewStringUTF(tmp);
        jobject _msg_offset = env->NewObject(jbigint, jbigcons, _jstrtmp);
        jobject _type = env->NewStringUTF(typ);


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
    env->ReleaseStringUTFChars(offset, bigint_str);
    if(msg)
        free(msg);

    return retObj;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_tulip_project_tulip_Chat_SendMessage
(JNIEnv *env, jobject instance, jstring user_, jstring pass_, jstring to_user_,
 jstring msg_) {

    const char *user = env->GetStringUTFChars(user_, 0);
    const char *pass = env->GetStringUTFChars(pass_, 0);
    const char *to_user = env->GetStringUTFChars(to_user_, 0);
    const char *msg = env->GetStringUTFChars(msg_, 0);

    int ret = 0;

    // using a separate connection for sending messages
    // so that there isn't a race condition with background
    // processes
    if(!message_conn)
    {
        ret = connection_reset((char *) user, (char *) pass, &message_conn);
    }

    if(!ret)
        ret |= client_message((char*)user, (char*)to_user, (char*)pass, message_conn, (char*)msg, strlen(msg) );

    if(ret)
        connection_reset((char*)user, (char*) pass, &message_conn);

    env->ReleaseStringUTFChars(user_, user);
    env->ReleaseStringUTFChars(pass_, pass);
    env->ReleaseStringUTFChars(to_user_, to_user);
    env->ReleaseStringUTFChars(msg_, msg);

    return ret;
 }