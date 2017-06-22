#The idea :

sorting a large array of integers in parallel taking O(nlogn) time where n is the sub size of each process 

1-	master takes array of numbers 
2-	master Scatter the array among slaves
3- 	each slave call it's own merge sort function to get a sorted subArray
4- 	master Gather the sorted subArrays 
5-	master does a final merge on the sorted subArrays to get a fully sorted array
6-	master sorts the reminder using merge sort and merge the result with the main sorted array
7-	master prints the array