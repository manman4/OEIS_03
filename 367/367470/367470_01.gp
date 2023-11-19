\\ a(n) = Sum_{k=0..n} 2^k * (k+1)! * Stirling2(n,k).
a(n) = sum(k=0, n, 2^k * (k+1)! * stirling(n,k,2));
for(n=0, 17, print1(a(n),", "))   
