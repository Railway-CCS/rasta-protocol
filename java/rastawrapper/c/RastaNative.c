#include <stdio.h>
#include <jni.h>
#include "RastaNative.h"
#include <rasta_new.h>
#include <rmemory.h>
#include <rastaredundancy_new.h>

struct rasta_handle * handle;
static JavaVM * javaVM;
jobject global_obj;

void onReceive(struct rasta_notification_result *result){

    JNIEnv *env;
    (*javaVM)->AttachCurrentThread(javaVM, &env, NULL);

    jclass cls = (*env)->FindClass(env, "librasta/RastaNative");
    jmethodID mid = (*env)->GetMethodID(env, cls, "librasta_onReceive", "(J[B)V");

    if (mid == 0){
        printf("CALLBACK NOT FOUND!\n");
    }

    rastaApplicationMessage message = sr_get_received_data(result->handle, &result->connection);

    jbyteArray array = (*env)->NewByteArray(env, message.appMessage.length);
    (*env)->SetByteArrayRegion(env, array, 0, message.appMessage.length, (jbyte *)message.appMessage.bytes);

    (*env)->CallVoidMethod(env, global_obj, mid, message.id, array);
}

void onDisconnection(struct rasta_notification_result *result, unsigned short reason, unsigned short details){
    JNIEnv *env;
    (*javaVM)->AttachCurrentThread(javaVM, &env, NULL);

    jclass cls = (*env)->FindClass(env, "librasta/RastaNative");
    jmethodID mid = (*env)->GetMethodID(env, cls, "librasta_onDisconnection", "(JSS)V");

    if (mid == 0){
        printf("CALLBACK NOT FOUND!\n");
    }

    (*env)->CallVoidMethod(env, global_obj, mid, (long)result->connection.remote_id, reason, details);
}

void onTimeout(struct rasta_notification_result *result){
    JNIEnv *env;
    (*javaVM)->AttachCurrentThread(javaVM, &env, NULL);

    jclass cls = (*env)->FindClass(env, "librasta/RastaNative");
    jmethodID mid = (*env)->GetMethodID(env, cls, "librasta_onDisconnection", "(JSS)V");

    if (mid == 0){
        printf("CALLBACK NOT FOUND!\n");
    }

    (*env)->CallVoidMethod(env, global_obj, mid, (long)result->connection.remote_id, 0xFF, 0xFF);
}

void onNewConnection(struct rasta_notification_result *result){
    JNIEnv *env;
    (*javaVM)->AttachCurrentThread(javaVM, &env, NULL);

    jclass cls = (*env)->FindClass(env, "librasta/RastaNative");
    jmethodID mid = (*env)->GetMethodID(env, cls, "librasta_onNewConnection", "(J[Ljava/lang/String;[I)V");

    if (mid == 0){
        printf("CALLBACK NOT FOUND!\n");
    }
    rasta_redundancy_channel * channel = redundancy_mux_get_channel(&result->handle->mux, result->connection.remote_id);

    jobjectArray ip_array = 0;
    jintArray port_array = (*env)->NewIntArray(env, channel->connected_channel_count);
    ip_array = (*env)->NewObjectArray(env, channel->connected_channel_count, (*env)->FindClass(env,"java/lang/String"),0);



    int ports[channel->connected_channel_count];
    for (int i = 0; i < channel->connected_channel_count; ++i) {
        ports[i] = channel->connected_channels[i].port;

        printf("Channel %d: %s:%d\n", i, channel->connected_channels[i].ip_address, channel->connected_channels[i].port);
        jstring str = (*env)->NewStringUTF(env, channel->connected_channels[i].ip_address);
        (*env)->SetObjectArrayElement(env, ip_array, i, str);
    }

    (*env)->SetIntArrayRegion(env, port_array, 0, channel->transport_channel_count, ports);

    (*env)->CallVoidMethod(env, global_obj, mid, result->connection.remote_id, ip_array, port_array);
}



JNIEXPORT void JNICALL Java_librasta_RastaNative_librasta_1init
  (JNIEnv * env, jobject obj, jstring configFile){
        handle = rmalloc(sizeof(struct rasta_handle));

        const char * nativeFile = (*env)->GetStringUTFChars(env, configFile, 0);
  		sr_init_handle(handle, nativeFile);

        global_obj = (*env)->NewGlobalRef(env, obj);
        (*env)->GetJavaVM(env, &javaVM);

  		handle->notifications.on_receive = onReceive;
  		handle->notifications.on_disconnection_request_received = onDisconnection;
  		handle->notifications.on_handshake_complete = onNewConnection;
  		handle->notifications.on_heartbeat_timeout = onTimeout;
  }


JNIEXPORT void JNICALL Java_librasta_RastaNative_librasta_1send
  (JNIEnv * env, jobject obj, jlong receiverId, jbyteArray data){
        struct RastaMessageData dataToSend;
        allocateRastaMessageData(&dataToSend, 1);

        jsize dataLen = (*env)->GetArrayLength(env,data);
        char * dataArray = (char*)(*env)->GetByteArrayElements(env, data, 0);


        struct RastaByteArray array;
        allocateRastaByteArray(&array, (unsigned int)dataLen);
        rmemcpy(array.bytes, dataArray, (unsigned int)dataLen);

        dataToSend.data_array[0] = array;

  		sr_send(handle, (unsigned long)receiverId, dataToSend);

  		freeRastaMessageData(&dataToSend);
  }


JNIEXPORT void JNICALL Java_librasta_RastaNative_librasta_1disconnectFrom
  (JNIEnv * env, jobject obj, jlong id, jint reason, jlong details){
  		sr_disconnect(handle, (unsigned int) id);
  }

JNIEXPORT void JNICALL Java_librasta_RastaNative_librasta_1cleanup
  (JNIEnv * env, jobject obj){
        sr_cleanup(handle);
        rfree(handle);
        (*env)->DeleteGlobalRef(env, global_obj);
  }
