package ydk.test;

import ydk.lang.IO;

public class QuickSort {
    private static void quickSort(int[] arr, int left, int right){
        if(left>right) return;
        int i = left;
        int j = right;
        int pivot = arr[left];
        while(i<j){
            while(i<j && arr[j]>pivot)j--;
            if(i<j){
                arr[i++] = arr[j];
            }
            while(i<j && arr[i]<pivot)i++;
            if(i<j){
                arr[j--] = arr[i];
            }
        }
        arr[i] = pivot;
        quickSort(arr,left,i-1);
        quickSort(arr,i+1,right);
    }
    public static void main(String[] args){
        int[] arr= {1,5,9,4,12,9,4,1,96,98,7,74,6,8989,1,9,7,4,1,1,0};
        quickSort(arr,0,arr.length-1);
        for(int x:arr){
            IO.print(x);
            IO.print(" ");
        }
        IO.print('\n');
    }
}
