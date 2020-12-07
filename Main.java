package com.cjz.lab.rotateNV21AnyAngle;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Executor;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

public class Main {

	private static final ThreadFactory sThreadFactory = new ThreadFactory() {
		private final AtomicInteger mCount = new AtomicInteger(1);

		public Thread newThread(Runnable r) {
			return new Thread(r, "AsyncTaskEx #" + mCount.getAndIncrement());
		}
	};
	private static final BlockingQueue<Runnable> sPoolWorkQueue = new LinkedBlockingQueue<Runnable>(360);
	public final Executor TEST_THREAD_POOL_EXECUTOR = new ThreadPoolExecutor(24, 24, 0, TimeUnit.SECONDS,
			sPoolWorkQueue, sThreadFactory);

	public static void main(String args[]) {
		System.out.println("矩阵任意角度旋转和NV21任意角度旋转");
		try {
			FileInputStream fis = new FileInputStream(
					new File("/media/chenjiezhu/work2/其他/Download/2048x1024_1605497440349.nv21"));
			byte data[] = new byte[fis.available()];
			fis.read(data);
			fis.close();
			Main m = new Main();
			for (int i = 0; i <= 360; i += 1) {
				final int degree = i;
				m.TEST_THREAD_POOL_EXECUTOR.execute(new Runnable() {
					@Override
					public void run() {
						m.rotateNV21Image(data, 2048, 1024, degree);
					}
				});
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return;
	}

	public byte[] rotateMatrix(byte data[], int width, int height, float rotateDegree) {
		try {
			long startTimeStamp = System.currentTimeMillis();
			int area[] = rotateAreaTransform(rotateDegree, width, height);
			if (area[0] % 2 != 0)
				area[0]++; // uv要每两个单元为一组 因此宽度不能为奇数
			if (area[1] % 2 != 0)
				area[1]++;
			byte pixels[] = new byte[area[0] * area[1]];
			int offsetX = (area[0] - width) / 2;
			int offsetY = (area[1] - height) / 2;

			for (int i = 0; i < data.length; i += width) {
				double startXY[] = rotateCoordinateTransform(rotateDegree, 0, i / width, width / 2, height / 2, true);
				double endXY[] = rotateCoordinateTransform(rotateDegree, width - 1, i / width, width / 2, height / 2,
						true);
				List<int[]> linePoints = drawLine((int) Math.round(startXY[0]), (int) Math.round(startXY[1]),
						(int) Math.round(endXY[0]), (int) Math.round(endXY[1]), width);
				int j = i;
				for (int[] pointXY : linePoints) {
					int x = (pointXY[0] + offsetX);
					int y = (pointXY[1] + offsetY);
					if (x >= 0 && y >= 0 && x < area[0] && y < area[1]) {
						pixels[y * area[0] + x] = data[j];
						if (x - 1 >= 0 && pixels[y * area[0] + x - 1] == 0) { // 补足因信息不足产生的空隙区域
							pixels[y * area[0] + x - 1] = data[j];
						}
						++j;
					}
				}
			}
			System.out.println("cycle end in " + (System.currentTimeMillis() - startTimeStamp) + " ms");
			File file = new File("/media/chenjiezhu/work2/其他/Download/test/" + area[0] + "x" + area[1] + "_"
					+ System.currentTimeMillis() + ".nv21");
			if (!file.exists()) {
				file.createNewFile();
			}
			FileOutputStream fileOutputStream = new FileOutputStream(file);
			fileOutputStream.write(pixels);
			fileOutputStream.flush();
			fileOutputStream.close();
			return pixels;
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}

	public byte[] rotateNV21Image(byte data[], int width, int height, float rotateDegree) {
		long startTimeStamp = System.currentTimeMillis();
		int area[] = rotateAreaTransform(rotateDegree, width, height);
		if (area[0] % 2 != 0) {
			area[0]++; // uv要每两个单元为一组 因此宽度不能为奇数
		}
		if (area[1] % 2 != 0) {
			area[1]++;
		}
		byte pixels[] = new byte[area[0] * area[1] * 3 / 2];
		int offsetX = (area[0] - width) / 2;
		int offsetY = (area[1] - height) / 2;
		// 计算旋转后2个顶点
		double firstRowXYStard[] = rotateCoordinateTransform(rotateDegree, 0, 0, width / 2, height / 2, true);
		double firstRowXYEnd[] = rotateCoordinateTransform(rotateDegree, width, 0, width / 2, height / 2, true);
		double lastRowXYStart[] = rotateCoordinateTransform(rotateDegree, 0, height, width / 2, height / 2, true);
		double lastRowXYEnd[] = rotateCoordinateTransform(rotateDegree, width, height, width / 2, height / 2, true);
		// 计算最后一行与第一行的偏移值dx，算出行与行之间要进行x偏移值ddx 每行的斜率是一致的不用算
		double ddx = (lastRowXYStart[0] - firstRowXYStard[0]) / height;
		double ddy = (lastRowXYStart[1] - firstRowXYStard[1]) / height;
		System.out.println("ddx:" + ddx + ", ddy:" + ddy); // 如果ddx和ddy即使是整数也无法做到点对点，但ddy
															// ddx任意一个为0可以做到锯齿的对齐，主要是锯齿在偏移后无法咬合
		List<int[]> linePoints = drawLine((int) firstRowXYStard[0], (int) firstRowXYStard[1], (int) firstRowXYEnd[0],
				(int) firstRowXYEnd[1], width);

		System.out.println("len:" + linePoints.size());
		System.out.println("x:" + firstRowXYEnd[0] + ", y:" + firstRowXYEnd[1]);

		for (int i = 0, row = 0; i < data.length * 2 / 3; i += width, row++) {
			int j = i;
			for (int[] pointXY : linePoints) {
				int x = (int) /* Math.round */(pointXY[0] + offsetX + ddx * row);
				int y = (int) /* Math.round */(pointXY[1] + offsetY + ddy * row);
				if (x >= 0 && y >= 0 && x < area[0] && y < area[1]) {
					pixels[y * area[0] + x] = data[j];
					if (x - 1 >= 0 && pixels[y * area[0] + x - 1] == 0) { // 补足因信息不足产生的空隙区域
						pixels[y * area[0] + x - 1] = data[j];
					}
					if (/* y % 2 == 0 && */x % 2 == 0) {// 使uv位置对准新数组的uv位置，要注意别让数组越界 部分角度产生的y值很容易大概率落在奇数处，所以y%2==0不能用
						int uvX;
						if (j % width % 2 == 0) { // 不能整除的部分即每行的x
							uvX = j % width; // 刚好对准U
						} else {
							uvX = j % width - 1; // 对上了V 向前偏移1
						}
						int uvLayerOffset = area[0] * area[1] + y / 2 * area[0] + x; // u位置
						int srcDataOffset = width * height + row / 2 * width + uvX;
						// 更新当前行UV
						if (uvLayerOffset >= area[0] * area[1] && uvLayerOffset < pixels.length
								&& srcDataOffset < data.length) {
							pixels[uvLayerOffset] = data[srcDataOffset];// u
						}
						if (uvLayerOffset + 1 >= area[0] * area[1] && uvLayerOffset + 1 < pixels.length
								&& srcDataOffset + 1 < data.length) {
							pixels[uvLayerOffset + 1] = data[srcDataOffset + 1];
						}
						// 覆盖上行UV空洞
						uvLayerOffset = area[0] * area[1] + (y / 2 - 1) * area[0] + x;
						if (uvLayerOffset >= area[0] * area[1] && uvLayerOffset < pixels.length
								&& srcDataOffset < data.length) {
							pixels[uvLayerOffset] = data[srcDataOffset];// u
						}
						if (uvLayerOffset + 1 >= area[0] * area[1] && uvLayerOffset + 1 < pixels.length
								&& srcDataOffset + 1 < data.length) {
							pixels[uvLayerOffset + 1] = data[srcDataOffset + 1];// v
						}
						// 覆盖下行UV空洞
						uvLayerOffset = area[0] * area[1] + (y / 2 + 1) * area[0] + x;
						if (uvLayerOffset >= area[0] * area[1] && uvLayerOffset < pixels.length
								&& srcDataOffset < data.length) {
							pixels[uvLayerOffset] = data[srcDataOffset];// u
						}
						if (uvLayerOffset + 1 >= area[0] * area[1] && uvLayerOffset + 1 < pixels.length
								&& srcDataOffset + 1 < data.length) {
							pixels[uvLayerOffset + 1] = data[srcDataOffset + 1];// v
						}
					}
					++j;
				}
			}
		}
		System.out.println("cycle end in " + (System.currentTimeMillis() - startTimeStamp) + " ms");
		File file = new File("/media/chenjiezhu/work2/其他/Download/test/degree_" + rotateDegree + "_area_" + area[0]
				+ "x" + area[1] + "_" + System.currentTimeMillis() + ".nv21");
		FileOutputStream fileOutputStream;
		try {
			fileOutputStream = new FileOutputStream(file);
			fileOutputStream.write(pixels);
			fileOutputStream.flush();
			fileOutputStream.close();
			return pixels;
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * @param centerPointX
	 *            Rotate according to center pointX
	 * @param centerPointY
	 *            Rotate according to center pointY
	 **/
	private double[] rotateCoordinateTransform(float rotateDegree, int bmpX, int bmpY, int centerPointX,
			int centerPointY, boolean ccw) {
		double temp[] = new double[2];
		double degreeToRadians = Math.toRadians(rotateDegree);
		if (ccw) {
			temp[0] = (Math.cos(degreeToRadians) * (bmpX - centerPointX)
					- Math.sin(degreeToRadians) * (bmpY - centerPointY));
			temp[1] = (Math.sin(degreeToRadians) * (bmpX - centerPointX)
					+ Math.cos(degreeToRadians) * (bmpY - centerPointY));
		} else {
			temp[0] = (Math.cos(degreeToRadians) * (bmpX - centerPointX)
					+ Math.sin(degreeToRadians) * (bmpY - centerPointY));
			temp[1] = (Math.sin(degreeToRadians) * (bmpX - centerPointX)
					- Math.cos(degreeToRadians) * (bmpY - centerPointY));
		}
		temp[0] += centerPointX;
		temp[1] += centerPointY;
		return temp;
	}

	private int[] rotateAreaTransform(float rotateDegree, int width, int height) {
		double degreeToRadians = Math.toRadians(rotateDegree);
		int newWidth = (int) (Math.abs(Math.cos(degreeToRadians) * width)
				+ Math.abs(Math.sin(degreeToRadians) * height));
		int newHeight = (int) (Math.abs(Math.cos(degreeToRadians) * height)
				+ Math.abs(Math.sin(degreeToRadians) * width));
		return new int[] { (int) (newWidth), (int) (newHeight) };
	}

	private List<int[]> drawLine(int x1, int y1, int x2, int y2, int srcLineWidth) { // cjzmark 输入起始点和终结点，返回线条坐标点阵
		int dx = Math.abs(x2 - x1), dy = Math.abs(y2 - y1), yy = 0;
		List<int[]> pointList = new LinkedList<>();

		if (dx < dy) {
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
		for (int i = 0; cx != x2 && i < srcLineWidth; i++) {
			// while (cx != x2) {
			if (d < 0) {
				d += n2dy;
			} else {
				cy += iy;
				d += n2dydx;
			}
			if (yy > 0) { // 如果直线与 x 轴的夹角大于 45 度
				pointList.add(new int[] { cy, cx });
			} else { // 如果直线与 x 轴的夹角小于 45 度
				pointList.add(new int[] { cx, cy });
			}
			cx += ix;
		}
		return pointList;
	}
}
