M=19;

a(n) = n! * sum(k=0, n\2, k^(n-k) / (k! * (n-2*k)!));
for(n=0, M, print1(a(n), ", "))
