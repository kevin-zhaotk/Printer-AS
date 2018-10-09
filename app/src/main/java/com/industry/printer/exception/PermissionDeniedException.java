package com.industry.printer.exception;

public class PermissionDeniedException extends Exception {

	@Override
	public String getMessage() {
		return "Permission Denied: no enough permission for the target file";
	}

	@Override
	public Throwable getCause() {
		// TODO Auto-generated method stub
		return super.getCause();
	}

		
}
