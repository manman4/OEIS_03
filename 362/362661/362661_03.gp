M=18;

a(n) = n! * sum(k=0, n\3, (n-3*k)^k * (n-3*k+1)^(n-3*k-1) / (6^k * k! * (n-3*k)!));
for(n=0, M, print1(a(n), ", "))
