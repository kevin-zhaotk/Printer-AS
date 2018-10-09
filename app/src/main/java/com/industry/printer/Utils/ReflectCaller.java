package com.industry.printer.Utils;

import android.app.AlarmManager;

import android.app.IAlarmManager;
import android.content.Context;
import android.os.IPowerManager;
import android.os.PowerManager;

import java.lang.reflect.Method;

public class ReflectCaller {

	/**
	 * hide method upgrade PowerManagerService
	 */
	public static void PowerManagerUpgrade(Context context) {
		Class<PowerManager> managerClass = PowerManager.class;
		
		try{
			Method upgrade = managerClass.getDeclaredMethod("upgrade", (Class[])null);
			upgrade.setAccessible(true);
			PowerManager pwManager = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
			IPowerManager iManager = (IPowerManager) upgrade.invoke(pwManager, (Object[])null);
			
			iManager.upgrade();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * hide method upgrade PowerManagerService
	 */
	public static void AlarmManagerSystemClockToHw(Context context) {
		Class<AlarmManager> managerClass = AlarmManager.class;
		
		try{
			Method hw = managerClass.getDeclaredMethod("SystemClockToHw", (Class[])null);
			hw.setAccessible(true);
			AlarmManager aManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
			IAlarmManager iManager = (IAlarmManager) hw.invoke(aManager, (Object[])null);
			
			iManager.SystemClockToHw();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static void AlarmManagerHwToSystemClock(Context context) {
		Class<AlarmManager> managerClass = AlarmManager.class;
		
		try{
			Method hw = managerClass.getDeclaredMethod("HwToSystemClock", (Class[])null);
			hw.setAccessible(true);
			AlarmManager aManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
			IAlarmManager iManager = (IAlarmManager) hw.invoke(aManager, (Object[])null);
			if (iManager != null) {
				iManager.HwToSystemClock();
			}
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	
	public static void SysPropUpgrade() {
		try {
			Class<?> mClassType = Class.forName("android.os.SystemProperties");
			Method mSetMethod = mClassType.getDeclaredMethod("set", String.class, String.class);
			mSetMethod.invoke(mClassType, "ctl.start", "Upgrade");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
