M=19;

a(n) = n! * sum(k=0, n\2, (-1)^k * (2*k+1)^(n-k-1) / (k! * (n-2*k)!));
for(n=0, M, print1(a(n), ", "))
