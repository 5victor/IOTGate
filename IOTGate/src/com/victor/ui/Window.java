package com.victor.ui;

import android.graphics.Canvas;
import android.view.MotionEvent;

public abstract class Window {
	String name; /* identify this window */
	
	public abstract void draw(Canvas canvas);
	public abstract boolean OnTouch(MotionEvent event);
}
