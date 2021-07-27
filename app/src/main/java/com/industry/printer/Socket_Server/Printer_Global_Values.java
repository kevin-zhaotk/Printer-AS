package com.industry.printer.Socket_Server;

@Deprecated
public class Printer_Global_Values {
	public int  Rows;
	public String File_Name="";
	public  void setRows(int Rows)
	{
		this.Rows=Rows;
	}
	public int getRows ()
	{
		return Rows;
	}
	public  void setFile_Name(String File_Name)
	{
		this.File_Name=File_Name;
	}
	public String getFile_Name ()
	{
		return File_Name;
	}
	public Printer_Global_Values() {
		File_Name="";
	}

	public Printer_Global_Values(/*int Rows,*/String File_Name) {
		super();
		//this.Rows = Rows;
		this.File_Name=File_Name;
		
	}
}
