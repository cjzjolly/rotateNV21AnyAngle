package com.cjz.lab.rotateNV21AnyAngle;

public class YUV420RotateJNI {
    public static native ImageDataBean rotateNV21DataAnyAngle(byte yuvBuf[], int width, int height, int rotateDegree);

    public static native ImageDataBean rotateGrayDataAnyAngle(byte yuvBuf[], int width, int height, int rotateDegree);
}
