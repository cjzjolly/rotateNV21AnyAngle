package com.cjz.lab.rotateNV21AnyAngle;

public class ImageDataBean {
    private int mHeight;
    private int mWidth;
    private byte[] mImageData;

    public int getHeight() {
        return mHeight;
    }

    public void setHeight(int height) {
        this.mHeight = height;
    }

    public int getWidth() {
        return mWidth;
    }

    public void setWidth(int width) {
        this.mWidth = width;
    }

    public byte[] getImageData() {
        return mImageData;
    }

    public void setImageData(byte[] imageData) {
        this.mImageData = imageData;
    }
}
