a(n) = 1/(3*n+1)! * sum(k=0, n, (3*n+k)! * stirling(n, k, 1));   
for(n=0, 17, print1(a(n),", ")) 
