package com.industry.printer.exception;

public class TlkNotFoundException extends Exception {

	@Override
	public String getMessage() {
		return "Tlk file Not Found";
	}

	@Override
	public Throwable getCause() {
		// TODO Auto-generated method stub
		return super.getCause();
	}

		
}
