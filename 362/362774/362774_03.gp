M=17;

a(n) = n! * sum(k=0, n, (2*k+1)^(k-1) * binomial(2*k,n-k)/k!);
for(n=0, M, print1(a(n), ", "))
