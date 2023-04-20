M=15;

a(n) = n! * sum(k=0, n\4, (k+1)^(n-3*k-1) / (k! * (n-4*k)!));
for(n=0, M, print1(a(n), ", "))
