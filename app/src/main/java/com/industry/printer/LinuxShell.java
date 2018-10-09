package com.industry.printer;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class LinuxShell {
	public static boolean isRoot(Runtime r, long wait)
			throws IOException, InterruptedException {
		boolean root = false;
		Process p = null;
		BufferedReader errReader = null;
		p = Runtime.getRuntime().exec("su");
		Thread.sleep(wait);
		Runtime.getRuntime().exec("chmod 777 /dev");
		p = Runtime.getRuntime().exec("chmod 666 /dev/ttyACM0");
		Thread.sleep(wait);
		errReader = new BufferedReader(
				new InputStreamReader(p.getErrorStream()));
		if (!errReader.ready()) { 
			root = true;
			destroyProcess(p);
		}
		return root;
	}
	
	private static void destroyProcess(Process process) {
        try {
            if (process != null) {
                // use exitValue() to determine if process is still running. 
                process.exitValue();
            }
        } catch (IllegalThreadStateException e) {
            // process is still running, kill it.
            process.destroy();
        }
    }
}
