a(n) = 1/(3*n+1)! * sum(k=0, n, (3*n+k)! * stirling(n, k, 2));   
for(n=0, 16, print1(a(n),", ")) 
