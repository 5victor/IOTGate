package com.victor.ui;

import java.util.Vector;

import android.graphics.Canvas;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;

public class WindowManage {
	private Vector<Window> windows;
	private SurfaceHolder surfaceHolder;
	private Window currentWindow;
	
	public WindowManage(SurfaceHolder holder)
	{
		surfaceHolder = holder;
		
		holder.addCallback(new Callback()
		{

			@Override
			public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2,
					int arg3) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void surfaceCreated(SurfaceHolder arg0) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void surfaceDestroyed(SurfaceHolder arg0) {
				// TODO Auto-generated method stub
				
			}
			
		});
	}
	
	public Canvas getCanvas()
	{
		return surfaceHolder.lockCanvas();
	}
	
	public void commitCanvas(Canvas canvas)
	{
		surfaceHolder.unlockCanvasAndPost(canvas);
	}
	
	public void registerWindow(Window w)
	{
		windows.add(w);
	}
	
	public boolean onTouch(MotionEvent event)
	{
		return currentWindow.OnTouch(event);
	}
	
	public Window getWindowByName(String name)
	{
		int i;
		Window window;
		
		window = null;
		for (i = 0; i < windows.size(); i++)
		{
			if (windows.get(i).name == name)
			{
				window = windows.get(i);
				break;
			}
		}
		return window;
	}
	
	public void setCurrentWindowByName(String name)
	{
		currentWindow = getWindowByName(name);
	}
}
