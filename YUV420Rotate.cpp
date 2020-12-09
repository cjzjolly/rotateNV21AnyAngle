#include <jni.h>
#include <string.h>
#include <malloc.h>
#include "rotateUtilCPPVersion.cpp"

JNIEXPORT jobject JNICALL Java_com_cjz_lab_rotateNV21AnyAngle_YUV420Rotate_rotateNV21DataAnyAngle(JNIEnv *env, jclass instance, jbyteArray yuvBuf, jint width, jint height, jint rotateDegree)
{
    unsigned char *yuvBufAddress = (unsigned char *)env->GetByteArrayElements(yuvBuf, NULL);
    int yuvBufLen = env->GetArrayLength(yuvBuf);
    ImageData imageData = rotateYUV420(yuvBufAddress, yuvBufLen, width, height, rotateDegree); //get proccessed data from native
    env->ReleaseByteArrayElements(yuvBuf, (jbyte *)yuvBufAddress, 0);                          //release java byte array
    //convert native byte array to java byte array.
    jbyteArray rtnbytes = env->NewByteArray(imageData.dataLen);
    env->SetByteArrayRegion(rtnbytes, 0, imageData.dataLen,
                            reinterpret_cast<const jbyte *>(imageData.data));
    //convert finished.The native data can be delete from memory.
    free(imageData.data);
    //set java bean
    jclass dataBeanClass = env->FindClass("com/meizu/media/cameraAlgorithm/yuv/ImageDataBean");

    // 获取类的构造函数，记住这里是调用无参的构造函数
    jmethodID id = env->GetMethodID(dataBeanClass, "<init>", "()V");
    // 创建一个新的对象
    jobject dataBeanObj = env->NewObject(dataBeanClass, id);

    //set width and height
    jmethodID midSetWidth = env->GetMethodID(dataBeanClass, "setWidth", "(I)V");
    env->CallVoidMethod(dataBeanObj, midSetWidth, imageData.width);
    jmethodID midSetHeight = env->GetMethodID(dataBeanClass, "setHeight", "(I)V");
    env->CallVoidMethod(dataBeanObj, midSetHeight, imageData.height);
    //set yuv data
    jmethodID midSetImageData = env->GetMethodID(dataBeanClass, "setImageData", "([B)V");
    env->CallVoidMethod(dataBeanObj, midSetImageData, rtnbytes);

    return dataBeanObj;
}

JNIEXPORT jobject JNICALL Java_com_cjz_lab_rotateNV21AnyAngle_YUV420Rotate_rotateGrayDataAnyAngle(JNIEnv *env, jclass instance, jbyteArray yuvBuf, jint width, jint height, jint rotateDegree)
{
    unsigned char *yuvBufAddress = (unsigned char *)env->GetByteArrayElements(yuvBuf, NULL);
    int yuvBufLen = env->GetArrayLength(yuvBuf);
    ImageData imageData = rotateGray(yuvBufAddress, yuvBufLen, width, height, rotateDegree); //get proccessed data from native
    env->ReleaseByteArrayElements(yuvBuf, (jbyte *)yuvBufAddress, 0);                        //release java byte array
    //convert native byte array to java byte array.
    jbyteArray rtnbytes = env->NewByteArray(imageData.dataLen);
    env->SetByteArrayRegion(rtnbytes, 0, imageData.dataLen,
                            reinterpret_cast<const jbyte *>(imageData.data));
    //convert finished.The native data can be delete from memory.
    free(imageData.data);
    //set java bean
    jclass dataBeanClass = env->FindClass("com/meizu/media/cameraAlgorithm/yuv/ImageDataBean");

    // 获取类的构造函数，记住这里是调用无参的构造函数
    jmethodID id = env->GetMethodID(dataBeanClass, "<init>", "()V");
    // 创建一个新的对象
    jobject dataBeanObj = env->NewObject(dataBeanClass, id);

    //set width and height
    jmethodID midSetWidth = env->GetMethodID(dataBeanClass, "setWidth", "(I)V");
    env->CallVoidMethod(dataBeanObj, midSetWidth, imageData.width);
    jmethodID midSetHeight = env->GetMethodID(dataBeanClass, "setHeight", "(I)V");
    env->CallVoidMethod(dataBeanObj, midSetHeight, imageData.height);
    //set yuv data
    jmethodID midSetImageData = env->GetMethodID(dataBeanClass, "setImageData", "([B)V");
    env->CallVoidMethod(dataBeanObj, midSetImageData, rtnbytes);

    return dataBeanObj;
}
