M=17;

a(n) = n! * sum(k=0, n\2, (n-2*k)^k * (2*n-4*k+1)^(n-2*k-1) / (k! * (n-2*k)!));
for(n=0, M, print1(a(n), ", "))
