#include "stdio.h"
#include "stdlib.h"
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "math.h"
// #include "./LinkedList.h"
#include <list>

class ImageData {
    public :
        int width;
        int height;
        int dataLen;
        unsigned char *data;
};

class Point {
    public:
        int x;
        int y;
};
 
using namespace std;

#define PI 3.14159265



double *rotateCoordinateTransform(float rotateDegree, int bmpX, int bmpY, int centerPointX,
                                  int centerPointY, int ccw);
int *rotateAreaTransform(float rotateDegree, int width, int height);
list<Point> drawLine(int x1, int y1, int x2, int y2, int srcLineWidth);
ImageData rotateGray(unsigned char *data, int dataLen, int width, int height, int rotateDegree);
ImageData rotateYUV420(unsigned char *data, int dataLen, int width, int height, int rotateDegree);


long getFileSize(char *path)
{
    long size = -1;
    struct stat s;
    if (stat(path, &s) >= 0)
    {
        size = s.st_size;
    }
    return size;
}

int main(){
    char *filePath = "/media/chenjiezhu/work2/其他/Download/2048x1024_1605497440349.nv21";
    unsigned long f = open(filePath, O_RDWR);
    long fileSize = getFileSize(filePath);
    void *fileMapAddress = mmap(0, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
    unsigned char temp[(int)fileSize];
    memcpy(temp, fileMapAddress, (int)fileSize);
    int i;
    for (i = 0; i <= 360; i++) {
        ImageData data = rotateGray(temp, (int)fileSize * 2 / 3, 2048, 1024, i);

        char outputName[3000];
        char numChar[25];
        memset(outputName, 0, 3000);
        strcat(outputName, "/media/chenjiezhu/work2/其他/Download/test/degree_");
        sprintf(numChar, "%d", i);
        strcat(outputName, numChar);
        strcat(outputName, "_");
        sprintf(numChar, "%d", data.width);
        strcat(outputName, numChar);
        strcat(outputName, "x");
        sprintf(numChar, "%d", data.height);
        strcat(outputName, numChar);
        strcat(outputName, ".gray");
        unsigned long f = open(outputName, O_RDWR | O_CREAT);
        printf("path:%s\n", outputName);
        int result = write(f, (void *)data.data, data.dataLen);
        printf("result : %d\n", result);
        close(f);


        free(data.data);
        // delete &data;
    }
    munmap(fileMapAddress, fileSize);
    close(f);
    return 0;
}

ImageData rotateGray(unsigned char *data, int dataLen, int width, int height, int rotateDegree) {
    int *area = rotateAreaTransform(rotateDegree, width, height);
    if (area[0] % 2 != 0)
        area[0]++; // uv要每两个单元为一组 因此宽度不能为奇数
    if (area[1] % 2 != 0)
        area[1]++;
    int pixelsLen = area[0] * area[1];
    unsigned char *pixels = (unsigned char *) malloc(pixelsLen);
    memset(pixels, 0, pixelsLen);
    int offsetX = (area[0] - width) / 2;
    int offsetY = (area[1] - height) / 2;
    // 计算旋转后2个顶点
    double *firstRowXYStard = rotateCoordinateTransform(rotateDegree, 0, 0, width / 2, height / 2, 1);
    double *firstRowXYEnd = rotateCoordinateTransform(rotateDegree, width, 0, width / 2, height / 2, 1);
    double *lastRowXYStart = rotateCoordinateTransform(rotateDegree, 0, height, width / 2, height / 2, 1);
    // 计算最后一行与第一行的偏移值dx，算出行与行之间要进行x偏移值ddx 每行的斜率是一致的不用算
    double ddx = (lastRowXYStart[0] - firstRowXYStard[0]) / height;
    double ddy = (lastRowXYStart[1] - firstRowXYStard[1]) / height;

    list<Point> linePoints = drawLine((int)firstRowXYStard[0], (int)firstRowXYStard[1], (int)firstRowXYEnd[0],
                                      (int)firstRowXYEnd[1], width);
    free(firstRowXYStard);
    free(firstRowXYEnd);
    free(lastRowXYStart);
    int i, row;

    for (i = 0, row = 0; i < dataLen; i += width, row++)
    {
        int j = i;
        for(std::list<Point>::iterator it = linePoints.begin(); it != linePoints.end();it++)
        {
            Point pointXY = *it;
            int x = (int)(pointXY.x + offsetX + ddx * row);
            int y = (int)(pointXY.y + offsetY + ddy * row);
            if (x >= 0 && y >= 0 && x < area[0] && y < area[1])
            {
                pixels[y * area[0] + x] = data[j];
                if (x - 1 >= 0 && pixels[y * area[0] + x - 1] == 0)
                { // 补足因信息不足产生的空隙区域
                    pixels[y * area[0] + x - 1] = data[j];
                }
                ++j;
            }
        }
    }

    linePoints.clear();
    ImageData imageData;
    imageData.width = area[0];
    imageData.height = area[1];
    imageData.dataLen = pixelsLen;
    imageData.data = pixels;
    free(area);
    return imageData;
}



ImageData rotateYUV420(unsigned char *data, int dataLen, int width, int height, int rotateDegree)
{
    int *area = rotateAreaTransform(rotateDegree, width, height);
    if (area[0] % 2 != 0)
        area[0]++; // uv要每两个单元为一组 因此宽度不能为奇数
    if (area[1] % 2 != 0)
        area[1]++;
    int pixelsLen = area[0] * area[1] * 3 / 2;
    unsigned char *pixels = (unsigned char *) malloc(pixelsLen);
    memset(pixels, 0, pixelsLen);
    int offsetX = (area[0] - width) / 2;
    int offsetY = (area[1] - height) / 2;
    // 计算旋转后2个顶点
    double *firstRowXYStard = rotateCoordinateTransform(rotateDegree, 0, 0, width / 2, height / 2, 1);
    double *firstRowXYEnd = rotateCoordinateTransform(rotateDegree, width, 0, width / 2, height / 2, 1);
    double *lastRowXYStart = rotateCoordinateTransform(rotateDegree, 0, height, width / 2, height / 2, 1);
    // 计算最后一行与第一行的偏移值dx，算出行与行之间要进行x偏移值ddx 每行的斜率是一致的不用算
    double ddx = (lastRowXYStart[0] - firstRowXYStard[0]) / height;
    double ddy = (lastRowXYStart[1] - firstRowXYStard[1]) / height;

    list<Point> linePoints = drawLine((int)firstRowXYStard[0], (int)firstRowXYStard[1], (int)firstRowXYEnd[0],
                                    (int)firstRowXYEnd[1], width);
    free(firstRowXYStard);
    free(firstRowXYEnd);
    free(lastRowXYStart);
    int i, row;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    long startTimeStamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    for (i = 0, row = 0; i < dataLen * 2 / 3; i += width, row++)
    {
        int j = i;
        // ListItem *cursor = linePoints->headItem;
        
        //while (cursor != NULL)

        for(std::list<Point>::iterator it = linePoints.begin(); it != linePoints.end();it++)
        {
            Point pointXY = *it;
            int x = (int)(pointXY.x + offsetX + ddx * row);
            int y = (int)(pointXY.y + offsetY + ddy * row);
            if (x >= 0 && y >= 0 && x < area[0] && y < area[1])
            {
                pixels[y * area[0] + x] = data[j];
                if (x - 1 >= 0 && pixels[y * area[0] + x - 1] == 0)
                { // 补足因信息不足产生的空隙区域
                    pixels[y * area[0] + x - 1] = data[j];
                }
                if (/* y % 2 == 0 && */ x % 2 == 0)
                { // 使uv位置对准新数组的uv位置，要注意别让数组越界 部分角度产生的y值很容易大概率落在奇数处，所以y%2==0不能用
                    int uvX;
                    if (j % width % 2 == 0)
                    {                    // 不能整除的部分即每行的x
                        uvX = j % width; // 刚好对准U
                    }
                    else
                    {
                        uvX = j % width - 1; // 对上了V 向前偏移1
                    }
                    int uvLayerOffset = area[0] * area[1] + y / 2 * area[0] + x; // u位置
                    int srcDataOffset = width * height + row / 2 * width + uvX;
                    // 更新当前行UV
                    if (uvLayerOffset >= area[0] * area[1] && uvLayerOffset < pixelsLen && srcDataOffset < dataLen)
                    {
                        pixels[uvLayerOffset] = data[srcDataOffset]; // u
                    }
                    if (uvLayerOffset + 1 >= area[0] * area[1] && uvLayerOffset + 1 < pixelsLen && srcDataOffset + 1 < dataLen)
                    {
                        pixels[uvLayerOffset + 1] = data[srcDataOffset + 1];
                    }
                    // 覆盖上行UV空洞
                    uvLayerOffset = area[0] * area[1] + (y / 2 - 1) * area[0] + x;
                    if (uvLayerOffset >= area[0] * area[1] && uvLayerOffset < pixelsLen && srcDataOffset < dataLen)
                    {
                        pixels[uvLayerOffset] = data[srcDataOffset]; // u
                    }
                    if (uvLayerOffset + 1 >= area[0] * area[1] && uvLayerOffset + 1 < pixelsLen && srcDataOffset + 1 < dataLen)
                    {
                        pixels[uvLayerOffset + 1] = data[srcDataOffset + 1]; // v
                    }
                    // 覆盖下行UV空洞
                    uvLayerOffset = area[0] * area[1] + (y / 2 + 1) * area[0] + x;
                    if (uvLayerOffset >= area[0] * area[1] && uvLayerOffset < pixelsLen && srcDataOffset < dataLen)
                    {
                        pixels[uvLayerOffset] = data[srcDataOffset]; // u
                    }
                    if (uvLayerOffset + 1 >= area[0] * area[1] && uvLayerOffset + 1 < pixelsLen && srcDataOffset + 1 < dataLen)
                    {
                        pixels[uvLayerOffset + 1] = data[srcDataOffset + 1]; // v
                    }
                }
                ++j;
            }
        }
    }

    gettimeofday(&tv, NULL);
    long endTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    printf("cycle end in %ld ms\n", endTime - startTimeStamp);

    // destory(linePoints);
    linePoints.clear();
    // delete &linePoints;
    ImageData imageData;
    imageData.width = area[0];
    imageData.height = area[1];
    imageData.dataLen = pixelsLen;
    imageData.data = pixels;
    free(area);
    return imageData;
}

/**
	 * @param centerPointX
	 *            Rotate according to center pointX
	 * @param centerPointY
	 *            Rotate according to center pointY
	 **/
double *rotateCoordinateTransform(float rotateDegree, int bmpX, int bmpY, int centerPointX,
                                  int centerPointY, int ccw)
{
    double *temp = (double *)malloc(sizeof(double) * 2);
    double degreeToRadians = rotateDegree * PI / 180.0;
    if (ccw)
    {
        temp[0] = (cos(degreeToRadians) * (bmpX - centerPointX) - sin(degreeToRadians) * (bmpY - centerPointY));
        temp[1] = (sin(degreeToRadians) * (bmpX - centerPointX) + cos(degreeToRadians) * (bmpY - centerPointY));
    }
    else
    {
        temp[0] = (cos(degreeToRadians) * (bmpX - centerPointX) + sin(degreeToRadians) * (bmpY - centerPointY));
        temp[1] = (sin(degreeToRadians) * (bmpX - centerPointX) - cos(degreeToRadians) * (bmpY - centerPointY));
    }
    temp[0] += centerPointX;
    temp[1] += centerPointY;
    return temp;
}

int *rotateAreaTransform(float rotateDegree, int width, int height)
{
    double degreeToRadians = rotateDegree * PI / 180.0;
    int *wh = (int *)malloc(sizeof(int) * 2);
    wh[0] = (int)(fabs(cos(degreeToRadians) * width) + fabs(sin(degreeToRadians) * height));
    wh[1] = (int)(fabs(cos(degreeToRadians) * height) + fabs(sin(degreeToRadians) * width));
    printf("%d, %d\n", wh[0], wh[1]);
    return wh;
}

list<Point> drawLine(int x1, int y1, int x2, int y2, int srcLineWidth)
{ // cjzmark 输入起始点和终结点，返回线条坐标点阵
    int dx = abs(x2 - x1), dy = abs(y2 - y1), yy = 0;
    list<Point> pointList;

    if (dx < dy)
    {
        yy = 1;

        int temp = x1;
        x1 = y1;
        y1 = temp;

        temp = x2;
        x2 = y2;
        y2 = temp;

        temp = dx;
        dx = dy;
        dy = temp;
    }

    int ix = (x2 - x1) > 0 ? 1 : -1, iy = (y2 - y1) > 0 ? 1 : -1, cx = x1, cy = y1, n2dy = dy * 2,
        n2dydx = (dy - dx) * 2, d = dy * 2 - dx;
    int i;
    for (i = 0; cx != x2 && i < srcLineWidth; i++)
    {
        // while (cx != x2) {
        if (d < 0)
        {
            d += n2dy;
        }
        else
        {
            cy += iy;
            d += n2dydx;
        }
        if (yy > 0)
        { // 如果直线与 x 轴的夹角大于 45 度
            // int *content = (int *)malloc(sizeof(int) * 2);
            Point content;
            content.x = cy;
            content.y = cx;
            // findMethodByObject(pointList, add, (void *)content);
            pointList.push_back(content);
        }
        else
        { // 如果直线与 x 轴的夹角小于 45 度
            // int *content = (int *)malloc(sizeof(int) * 2);
            Point content;
            content.x = cx;
            content.y = cy;
            // findMethodByObject(pointList, add, (void *)content);
            pointList.push_back(content);
        }
        cx += ix;
    }
    return pointList;
}