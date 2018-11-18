package com.industry.printer.Socket_Server;

import java.lang.reflect.Method;
import java.text.Collator;
import java.util.Comparator;

public class SortComparator implements java.lang.Comparable<SortComparator> {
    public SortComparator(String ss) {
        this.s = ss;
    }
    private String s;
    public String getS() {
        return s;
    }
    public int compareTo(SortComparator o) {
        try {
            String ss = o.getS();
            String ss_string = ss.replaceAll("[\\d]+", "");
            String s_string = s.replaceAll("[\\d]+", "");
            if (s_string.compareTo(ss_string) != 0) {
                return s_string.compareTo(ss_string);
            } else {
                int s_int = Integer.parseInt(s.replaceAll("[\\D]+", ""));
                int ss_int = Integer.parseInt(ss.replaceAll("[\\D]+", ""));
                return s_int - ss_int;
            }
        } catch (Exception e) {
            return 0;
        }
    }
    
}