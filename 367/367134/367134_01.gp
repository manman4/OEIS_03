a(n) = 1/(2*n+1)! * sum(k=0, n, (2*n+k)! * stirling(n, k, 2));   
for(n=0, 17, print1(a(n),", ")) 
