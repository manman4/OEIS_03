M=18;

a(n) = n! * sum(k=0, n\2, (n-2*k)^(n-k) / (2^k * k! * (n-2*k)!));
for(n=0, M, print1(a(n), ", "))
