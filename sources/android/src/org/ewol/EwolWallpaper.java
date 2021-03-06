/**
 * @author Edouard DUPIN
 *
 * @copyright 2011, Edouard DUPIN, all right reserved
 *
 * @license APACHE v2.0 (see license file)
 */

package org.ewol;

import android.app.ActivityManager;
import android.content.pm.ApplicationInfo;
import android.content.pm.ConfigurationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.os.Build;
import android.service.wallpaper.WallpaperService;
import android.util.Log;
import android.view.SurfaceHolder;
import org.ewol.EwolSurfaceViewGL;
import android.view.MotionEvent;

import android.net.Uri;
import android.content.Intent;
import android.content.ActivityNotFoundException;

import org.ewol.Ewol;

public abstract class EwolWallpaper extends WallpaperService implements EwolCallback, EwolConstants
{
	private GLEngine mGLView;
	private Ewol EWOL;
	static {
		System.loadLibrary("ewol");
	}
	
	protected void initApkPath(String org, String vendor, String project) {
		StringBuilder sb = new StringBuilder();
		sb.append(org).append(".");
		sb.append(vendor).append(".");
		sb.append(project);
		String apkFilePath = null;
		ApplicationInfo appInfo = null;
		PackageManager packMgmr = getPackageManager();
		try {
			appInfo = packMgmr.getApplicationInfo(sb.toString(), 0);
		} catch (NameNotFoundException e) {
			e.printStackTrace();
			throw new RuntimeException("Unable to locate assets, aborting...");
		}
		apkFilePath = appInfo.sourceDir;
		EWOL.paramSetArchiveDir(0, apkFilePath);
	}
	
	@Override public Engine onCreateEngine() {
		// set the java evironement in the C sources :
		EWOL = new Ewol(this, EWOL_APPL_TYPE_WALLPAPER);
		
		// Load the application directory
		EWOL.paramSetArchiveDir(1, getFilesDir().toString());
		EWOL.paramSetArchiveDir(2, getCacheDir().toString());
		// to enable extarnal storage: add in the manifest the restriction needed ...
		//packageManager.checkPermission("android.permission.READ_SMS", myPackage) == PERMISSION_GRANTED; 
		//Ewol.paramSetArchiveDir(3, getExternalCacheDir().toString());
		
		
		//! DisplayMetrics metrics = new DisplayMetrics();
		//! getWindowManager().getDefaultDisplay().getMetrics(metrics);
		//! EWOL.displayPropertyMetrics(metrics.xdpi, metrics.ydpi);
		
		// call C init ...
		EWOL.onCreate();
		
		// create bsurface system
		mGLView = new GLEngine(EWOL);
		
		return mGLView;
	}
	
	public class GLEngine extends Engine {
		private Ewol EWOL;
		public GLEngine(Ewol ewolInstance) {
			EWOL = ewolInstance;
		}
		
		class WallpaperGLSurfaceView extends EwolSurfaceViewGL
		{
			private static final String TAG = "WallpaperGLSurfaceView";
			WallpaperGLSurfaceView(Context context, Ewol ewolInstance) {
				super(context, ewolInstance);
				Log.d(TAG, "WallpaperGLSurfaceView(" + context + ")");
			}
			@Override public SurfaceHolder getHolder() {
				Log.d(TAG, "getHolder(): returning " + getSurfaceHolder());
				return getSurfaceHolder();
			}
			public void onDestroy() {
				Log.d(TAG, "onDestroy()");
				super.onDetachedFromWindow();
			}
		}
		
		private static final String TAG = "GLEngine";
		private WallpaperGLSurfaceView glSurfaceView;
		
		@Override public void onCreate(SurfaceHolder surfaceHolder) {
			Log.d(TAG, "onCreate(" + surfaceHolder + ")");
			super.onCreate(surfaceHolder);
			
			glSurfaceView = new WallpaperGLSurfaceView(EwolWallpaper.this, EWOL);
			
			// Check if the system supports OpenGL ES 2.0.
			final ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
			final ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();
			final boolean supportsEs2 = configurationInfo.reqGlEsVersion >= 0x20000;
			
			if (false==supportsEs2) {
				Log.d("LiveWallpaper", "does not support board with only open GL ES 1");
				return;
			}
			// Request an OpenGL ES 2.0 compatible context.
			//setEGLContextClientVersion(2);
			
			// On Honeycomb+ devices, this improves the performance when
			// leaving and resuming the live wallpaper.
			//setPreserveEGLContextOnPause(true);
			
		}
		
		@Override public void onTouchEvent(MotionEvent event) {
			glSurfaceView.onTouchEvent(event);
		}
		
		@Override public void onVisibilityChanged(boolean visible) {
			Log.d(TAG, "onVisibilityChanged(" + visible + ")");
			super.onVisibilityChanged(visible);
			if (true==visible) {
				glSurfaceView.onResume();
				// call C
				EWOL.onResume();
			} else {
				glSurfaceView.onPause();
				// call C
				EWOL.onPause();
			}
		}
		
		@Override public void onDestroy() {
			Log.d(TAG, "onDestroy()");
			super.onDestroy();
			// call C
			EWOL.onStop();
			EWOL.onDestroy();
			glSurfaceView.onDestroy();
		}
		
		protected void setPreserveEGLContextOnPause(boolean preserve) {
			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
				Log.d(TAG, "setPreserveEGLContextOnPause(" + preserve + ")");
				glSurfaceView.setPreserveEGLContextOnPause(preserve);
			}
		}
		
		protected void setEGLContextClientVersion(int version) {
			Log.d(TAG, "setEGLContextClientVersion(" + version + ")");
			glSurfaceView.setEGLContextClientVersion(version);
		}
	}
	
	public void keyboardUpdate(boolean show) {
		// never display keyboard on wallpaer...
		Log.d("EwolCallback", "KEABOARD UPDATE is not implemented ...");
	}
	
	public void eventNotifier(String[] args) {
		// just for the test ...
		Log.d("EwolCallback", "EVENT NOTIFIER is not implemented ...");
		
	}
	
	public void orientationUpdate(int screenMode) {
		Log.d("EwolCallback", "SET ORIENTATION is not implemented ...");
	}
	
	public void titleSet(String value) {
		// no title in the wallpaper ...
		Log.d("EwolCallback", "SET TITLE is not implemented ...");
	}
	
	public void openURI(String uri) {
		try {
			Intent myIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(uri));
			startActivity(myIntent);
		} catch (ActivityNotFoundException e) {
			Log.e("EwolActivity", "Can not request an URL");
		}
	}
	
	public void stop() {
		Log.d("EwolCallback", "STOP is not implemented ...");
	}
	
	public int audioGetDeviceCount() {
		Log.e("EwolActivity", "Get device List");
		return 0;
	}
	
	public String audioGetDeviceProperty(int idDevice) {
		Log.e("EwolActivity", "Get device property");
		return "";
	}
	
	public boolean audioOpenDevice(int idDevice, int freq, int nbChannel, int format) {
		Log.e("EwolActivity", "Open device");
		return false;
	}
	
	public boolean audioCloseDevice(int idDevice) {
		Log.e("EwolActivity", "Close device");
		return false;
	}
}


