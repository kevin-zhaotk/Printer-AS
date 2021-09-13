package com.printer.phoneapp.UIs;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.graphics.drawable.ColorDrawable;
import android.media.AudioAttributes;
import android.media.AudioManager;
import android.media.Image;
import android.media.ImageReader;
import android.media.SoundPool;
import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.v4.media.session.PlaybackStateCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.util.Size;
import android.util.SparseIntArray;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.ImageView;
import android.widget.PopupWindow;
import android.widget.Toast;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureFailure;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;

import com.google.zxing.BinaryBitmap;
import com.google.zxing.LuminanceSource;
import com.google.zxing.RGBLuminanceSource;
import com.google.zxing.Result;
import com.google.zxing.common.HybridBinarizer;
import com.google.zxing.qrcode.QRCodeReader;
import com.printer.phoneapp.Devices.ConnectDevice;
import com.printer.phoneapp.Devices.ConnectDeviceManager;
import com.printer.phoneapp.R;
import com.printer.phoneapp.Sockets.SocketThread;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.Arrays;

/**
 * Created by hmwan on 2021/9/7.
 */

public class BarcodeScanPopupWindow {
    public static final String TAG = BarcodeScanPopupWindow.class.getSimpleName();

    private Context mContext = null;
    private PopupWindow mPopupWindow = null;

    private ConnectDeviceManager mConDevManager = null;
    private SocketThread.OnSocketStringListener mListener = null;

    private HTNCCameraTextureView mTextureView = null;
    private RectF mClipRect = null;
    private ImageView mCloseBtn = null;
    private ImageView mProcessBar = null;
    private Animation mScanningBar = null;

    private final SparseIntArray ORIENTATIONS = new SparseIntArray();

    private Size mPreviewSize;
    private CameraDevice mCameraDevice = null;
    private CameraCaptureSession mCaptureSession = null;
    private CaptureRequest.Builder mPreviewRequestBuilder;
    private ImageReader mImageReader = null;
    Handler mChildHandler = null;
    Handler mMainHandler = null;

    private boolean mTaking = false;
    private boolean mFound = false;

    private final int MSG_SURFACE_READY         = 101;
    private final int MSG_TAKE_PICTURE          = 104;
    private final int MSG_QRCODE_FOUND          = 108;

    private Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_SURFACE_READY:
                    mProcessBar.setVisibility(View.VISIBLE);
                    mProcessBar.setAnimation(mScanningBar);
                    mScanningBar.startNow();
                    break;
                case MSG_TAKE_PICTURE:
                    takePicture();
                    break;
                case MSG_QRCODE_FOUND:
//                    if(null != mProcessBar.getAnimation()) mProcessBar.getAnimation().cancel();
                    mScanningBar.cancel();
                    mProcessBar.setAnimation(null);
                    mProcessBar.setVisibility(View.GONE);
                    Toast.makeText(mContext, "" + msg.obj, Toast.LENGTH_LONG).show();
                    if(null != mConDevManager) {
                        if(null != mConDevManager) {
                            mConDevManager.sendString((String)msg.obj, mListener);
                        }
                    }
                    mPopupWindow.dismiss();
                    break;
//                case MSG_UNKNOWN_QRCODE:
//                    Toast.makeText(HTNCQRCodeScanActivity.this, R.string.strUnknownQRCode, Toast.LENGTH_SHORT).show();
//                    break;
            }
        }
    };

    public BarcodeScanPopupWindow(Context ctx) {
        mContext = ctx;
        mConDevManager = ConnectDeviceManager.getInstance(ctx);
    }

    public void show(View v) {
        if(null == mContext) {
            return;
        }

        View popupView = LayoutInflater.from(mContext).inflate(R.layout.phone_barcode_scan, null);

        mPopupWindow = new PopupWindow(popupView, ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT, true);
        mPopupWindow.setBackgroundDrawable(new ColorDrawable(Color.parseColor("#CC000000")));
        mPopupWindow.setOutsideTouchable(true);
        mPopupWindow.setTouchable(true);
        mPopupWindow.update();

        mTextureView = (HTNCCameraTextureView) popupView.findViewById(R.id.idTextureView);
        mTextureView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction() & MotionEvent.ACTION_MASK) {
                    case MotionEvent.ACTION_DOWN:
                        if(mClipRect.contains(event.getX(), event.getY())) {
                            try {
                                mCaptureSession.setRepeatingRequest(mPreviewRequestBuilder.build(), sessonCaptureCallback, mChildHandler);
                            } catch(Exception e) {
                                Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
                            }
                        }
                        break;
                }
                return true;
            }
        });

        mTextureView.setSurfaceTextureListener(new TextureView.SurfaceTextureListener() {
            @Override
            public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
                ORIENTATIONS.append(Surface.ROTATION_0, 90);
                ORIENTATIONS.append(Surface.ROTATION_90, 0);
                ORIENTATIONS.append(Surface.ROTATION_180, 270);
                ORIENTATIONS.append(Surface.ROTATION_270, 180);

                Log.d(TAG, "width = " + width + "; height = " + height);

                openCamera(width, height);

                mClipRect = new RectF(0, 0, width, height);

                mHandler.sendEmptyMessage(MSG_SURFACE_READY);
            }

            @Override
            public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
            }

            @Override
            public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
                if(mImageReader != null) {
                    mImageReader.close();
                }
                if(mCameraDevice != null) {
                    mCameraDevice.close();
                }

                return false;
            }

            @Override
            public void onSurfaceTextureUpdated(SurfaceTexture surface) {
            }
        });

        mCloseBtn = (ImageView)popupView.findViewById(R.id.idCloseBtn);
        mCloseBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mPopupWindow.dismiss();
            }
        });

        mProcessBar = (ImageView)popupView.findViewById(R.id.idScaningBar);

        mScanningBar = new TranslateAnimation(
                Animation.RELATIVE_TO_SELF, 0,
                Animation.RELATIVE_TO_SELF, 0,
                Animation.RELATIVE_TO_PARENT, 0.05f,
                Animation.RELATIVE_TO_PARENT, 0.6f);
        mScanningBar.setDuration(4000);
        mScanningBar.setRepeatMode(Animation.RESTART);
        mScanningBar.setRepeatCount(Animation.INFINITE);

        mPopupWindow.showAtLocation(v, Gravity.NO_GRAVITY, 0, 0);
    }

    public void setSocketListener(SocketThread.OnSocketStringListener l) {
        mListener = l;
    }

    private void beepOnce() {
        SoundPool soundPool;
        if (Build.VERSION.SDK_INT >= 21) {
            SoundPool.Builder builder = new SoundPool.Builder();
            //传入音频的数量
            builder.setMaxStreams(1);
            //AudioAttributes是一个封装音频各种属性的类
            AudioAttributes.Builder attrBuilder = new AudioAttributes.Builder();
            //设置音频流的合适属性
            attrBuilder.setLegacyStreamType(AudioManager.STREAM_MUSIC);
            builder.setAudioAttributes(attrBuilder.build());
            soundPool = builder.build();

            //第一个参数Context,第二个参数资源Id，第三个参数优先级
            soundPool.load(mContext, R.raw.alarm, 1);
            soundPool.setOnLoadCompleteListener(new SoundPool.OnLoadCompleteListener() {
                @Override
                public void onLoadComplete(SoundPool soundPool, int sampleId, int status) {
                    soundPool.play(1, 1, 1, 0, 0, 1);
                }
            });
        } else {
        }
    }

    private String decodeBarcode(Bitmap barBitmap) {
        if(null == barBitmap) {
            return "";
        }

        int[] intArray = new int[barBitmap.getWidth() * barBitmap.getHeight()];
        barBitmap.getPixels(intArray, 0, barBitmap.getWidth(), 0, 0, barBitmap.getWidth(), barBitmap.getHeight());
        LuminanceSource source = new RGBLuminanceSource(barBitmap.getWidth(), barBitmap.getHeight(),intArray);
        BinaryBitmap bitmap = new BinaryBitmap(new HybridBinarizer(source));
        QRCodeReader reader = new QRCodeReader();
//        MultiFormatReader reader = new MultiFormatReader();
        try {
            Result result = reader.decode(bitmap);
            if(null != result) {
                return result.getText();
            }
        } catch (Exception e) {
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        } finally {
        }
        return "";
    }

    // 保存拍照图片的接口
    private ImageReader.OnImageAvailableListener onImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        // 当照片数据可用时激发该方法
        @Override
        public void onImageAvailable(ImageReader reader) {
            final Image image = reader.acquireNextImage();
            new Thread(new Runnable() {
                @Override
                public void run() {
                    Bitmap bigBitmap = null;

                    try {
                        // 获取捕获的照片数据
                        ByteBuffer buffer = image.getPlanes()[0].getBuffer();
                        byte[] data = new byte[buffer.remaining()];     // This is compressed data, not raw data.
                        buffer.get(data);

                        WeakReference<Bitmap> bmpRef = new WeakReference<Bitmap>(BitmapFactory.decodeByteArray(data, 0, data.length));
                        bigBitmap = bmpRef.get();

                        if(null != bigBitmap) {
                            Log.d(TAG, "Bitmap: [" + bigBitmap.getWidth() + " x " + bigBitmap.getHeight() + "]");
//                            bu.writeFile(Bitmap.CompressFormat.JPEG, bigBitmap, Environment.getExternalStorageDirectory().toString() + File.separator + "QRCode0.jpg");

                            if(null != bigBitmap) {
//                                Log.d(TAG, "Recognization Start: [" + System.currentTimeMillis() + "]");
                                String text = decodeBarcode(bigBitmap);
//                                Log.d(TAG, "Recognization End: [" + System.currentTimeMillis() + "]");
                                Log.d(TAG, "Recognized: [" + text + "]");

                                if (null != text && !text.isEmpty()) {
                                    beepOnce();

                                    mFound = true;

                                    Message msg = mHandler.obtainMessage();
                                    msg.what = MSG_QRCODE_FOUND;
                                    msg.obj = text;
                                    mHandler.sendMessage(msg);
                                }
                            } else {
                            }
                        } else {
                        }

                    } catch(Exception e) {
                        Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
                    } finally {
                        if(null != bigBitmap && !bigBitmap.isRecycled()) {
                            bigBitmap.recycle();
                        }
                        image.close();

                        try {Thread.sleep(50);} catch (Exception e) {};
                        mTaking = false;

                        System.gc();
                    }
                }
            }).start();
        }
    };

    // 打开相机操作的回调接口，由OpenCamera调用
    private CameraDevice.StateCallback deviceStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(@NonNull CameraDevice cameraDevice) {
            mCameraDevice = cameraDevice;
            takePreview(cameraDevice);
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice cameraDevice) {
            cameraDevice.close();
            mCameraDevice = null;
        }

        @Override
        public void onError(@NonNull CameraDevice cameraDevice, int error) {
            Log.e(TAG, "Camera pen error: " + error);
            cameraDevice.close();
            mCameraDevice = null;
            mPopupWindow.dismiss();
        }
    };

    // 预览取图状况的回调接口，由setRepeatingRequest函数调用
    private CameraCaptureSession.CaptureCallback sessonCaptureCallback = new CameraCaptureSession.CaptureCallback() {
        @Override
        public void onCaptureStarted(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, long timestamp, long frameNumber) {
            super.onCaptureStarted(session, request, timestamp, frameNumber);
        }

        @Override
        public void onCaptureProgressed(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull CaptureResult partialResult) {
            super.onCaptureProgressed(session, request, partialResult);
        }

        @Override
        public void onCaptureCompleted(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull TotalCaptureResult result) {
//            if((result.get(CaptureResult.CONTROL_AF_STATE) == CaptureResult.CONTROL_AF_STATE_PASSIVE_FOCUSED ||
//                    result.get(CaptureResult.CONTROL_AF_STATE) == CaptureResult.CONTROL_AF_STATE_FOCUSED_LOCKED) &&
//                    result.get(CaptureResult.CONTROL_AE_STATE) == CaptureResult.CONTROL_AE_STATE_CONVERGED) {
            if(!mTaking && !mFound) {
                mTaking = true;
                mHandler.sendEmptyMessage(MSG_TAKE_PICTURE);
            }
//            }
            super.onCaptureCompleted(session, request, result);
        }

        @Override
        public void onCaptureFailed(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull CaptureFailure failure) {
            super.onCaptureFailed(session, request, failure);
        }

        @Override
        public void onCaptureSequenceCompleted(@NonNull CameraCaptureSession session, int sequenceId, long frameNumber) {
            super.onCaptureSequenceCompleted(session, sequenceId, frameNumber);
        }

        @Override
        public void onCaptureSequenceAborted(@NonNull CameraCaptureSession session, int sequenceId) {
            super.onCaptureSequenceAborted(session, sequenceId);
        }

        @Override
        public void onCaptureBufferLost(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull Surface target, long frameNumber) {
            super.onCaptureBufferLost(session, request, target, frameNumber);
        }
    };

    // Session创建状况的回调接口，由createCaptureSession调用
    private CameraCaptureSession.StateCallback sessonStateCallback = new CameraCaptureSession.StateCallback() {
        @Override
        public void onConfigured(CameraCaptureSession cameraCaptureSession) {
            // 如果摄像头为null，直接结束方法
            if (null == mCameraDevice) {
                return;
            }


            // 当摄像头已经准备好时，开始显示预览
            mCaptureSession = cameraCaptureSession;

            try {
                // 设置自动对焦模式
                mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                // 设置自动曝光模式
                mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);

                // 设置预览时连续捕获图像数据
                mCaptureSession.setRepeatingRequest(mPreviewRequestBuilder.build(), sessonCaptureCallback, mChildHandler);
            } catch (CameraAccessException e) {
                Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
            }
        }

        @Override
        public void onConfigureFailed(CameraCaptureSession cameraCaptureSession) {
            Log.e(TAG, "Configuration failed.");
            mPopupWindow.dismiss();
        }
    };

    private void setUpCameraOutputs(CameraManager cameraManager, int width, int height) throws Exception {
        // 获取指定摄像头的特性
        CameraCharacteristics characteristics = cameraManager.getCameraCharacteristics("" + CameraCharacteristics.LENS_FACING_FRONT);

        // 获取摄像头支持的配置属性
        StreamConfigurationMap map = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);

        // 找到一个正好覆盖了预览画面的最小例图尺寸
        mPreviewSize = chooseOptimalSize(map.getOutputSizes(SurfaceTexture.class), new Size(Math.max(width, height), Math.min(width, height)));
        // 该方法是取最大的分辨率图片作为预览
//        mPreviewSize = map.getOutputSizes(SurfaceTexture.class)[0];
        Log.d(TAG, "PreviewSize: " + mPreviewSize.toString());

        // 生成图片处理模块，拍照的图片与最大图片保持一致，这样可以按照同样的比例处理图片，方便后续提取名片部分处理
        mImageReader = ImageReader.newInstance(Math.max(width, height), Math.min(width, height), ImageFormat.JPEG, 1);
        // 设置获取作为预览图片的最大的分辨率图片作为摄取图片
//        mImageReader = ImageReader.newInstance(Math.max(mPreviewSize.getWidth(), mPreviewSize.getHeight()), Math.min(mPreviewSize.getWidth(), mPreviewSize.getHeight()), ImageFormat.JPEG, 1);
        mImageReader.setOnImageAvailableListener(onImageAvailableListener, mMainHandler);

        // 根据选中的预览尺寸来调整预览组件（TextureView）的长宽比
        int orientation = mContext.getResources().getConfiguration().orientation;
        if (orientation == Configuration.ORIENTATION_LANDSCAPE) {
            mTextureView.setAspectRatio(mPreviewSize.getWidth(), mPreviewSize.getHeight());
        } else {
            mTextureView.setAspectRatio(mPreviewSize.getHeight(), mPreviewSize.getWidth());
        }
    }

    private Size chooseOptimalSize(Size[] choices, Size viewSize) {
        Size optimalSize = choices[0];  // 最大的那个

        for (Size choice : choices) {
            Log.d(TAG, "Camera Conf: " + choice.toString());
            if (choice.getWidth() >= viewSize.getWidth() &&
                    choice.getHeight() >= viewSize.getHeight() &&
                    choice.getWidth() < optimalSize.getWidth() &&
                    choice.getHeight() < optimalSize.getHeight()) {
                optimalSize = choice;
            }
        }
        return optimalSize;
    }

    // 打开摄像头
    private void openCamera(int width, int height) {
        try {
            CameraManager camaeraManager = (CameraManager)((Activity)mContext).getSystemService(Context.CAMERA_SERVICE);
            setUpCameraOutputs(camaeraManager, width, height);
            camaeraManager.openCamera("" + CameraCharacteristics.LENS_FACING_FRONT, deviceStateCallback, mMainHandler);
        } catch (CameraAccessException e) {
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
            mPopupWindow.dismiss();
        } catch (SecurityException e) {
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
            mPopupWindow.dismiss();
        } catch (Exception e) {
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
            mPopupWindow.dismiss();
        }
    }

    private void takePreview(CameraDevice cameraDevice) {
        try {
            SurfaceTexture texture = mTextureView.getSurfaceTexture();
            Surface surface = new Surface(texture);
            texture.setDefaultBufferSize(mPreviewSize.getWidth(), mPreviewSize.getHeight());

            // 创建作为预览的CaptureRequest.Builder
            mPreviewRequestBuilder = cameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);

            // 将textureView的surface作为CaptureRequest.Builder的目标
            mPreviewRequestBuilder.addTarget(surface);

            // 创建CameraCaptureSession，该对象负责管理处理预览请求和拍照请求
            cameraDevice.createCaptureSession(Arrays.asList(surface, mImageReader.getSurface()), sessonStateCallback, mChildHandler);
        }
        catch (CameraAccessException e) {
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        }
    }

    private void takePicture() {
        try {
            if (mCameraDevice == null) {
                return;
            }

            // 创建作为拍照的CaptureRequest.Builder
            final CaptureRequest.Builder captureRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_STILL_CAPTURE);

            // 将imageReader的surface作为CaptureRequest.Builder的目标
            captureRequestBuilder.addTarget(mImageReader.getSurface());
            // 设置自动对焦模式
            captureRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
            // 设置自动曝光模式
            captureRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);
            // 获取设备方向
            int rotation = ((AppCompatActivity)mContext).getWindowManager().getDefaultDisplay().getRotation();

            // 根据设备方向计算设置照片的方向
            captureRequestBuilder.set(CaptureRequest.JPEG_ORIENTATION, ORIENTATIONS.get(rotation));
            // 停止连续取景
            mCaptureSession.stopRepeating();
            // 捕获静态图像
            mCaptureSession.capture(captureRequestBuilder.build(), new CameraCaptureSession.CaptureCallback() {
                // 拍照完成时激发该方法
                @Override
                public void onCaptureCompleted(CameraCaptureSession session, CaptureRequest request, TotalCaptureResult result) {
                    if(mFound) return;
                    try {
                        // 设置自动对焦模式
                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
//                                // 重设自动对焦模式
//                                mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER, CameraMetadata.CONTROL_AF_TRIGGER_CANCEL);
                        // 设置自动曝光模式
                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);
                        // 打开连续取景模式
                        mCaptureSession.setRepeatingRequest(mPreviewRequestBuilder.build(), sessonCaptureCallback, mChildHandler);
                    } catch (CameraAccessException e) {
                        Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
                    }
                }
            }, mChildHandler);
        } catch (CameraAccessException e) {
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        } catch (Exception e) {
            Log.e(TAG, e.getClass().getSimpleName() + ": " + e.getMessage());
        }
    }

}
