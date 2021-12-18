M=52;

a(n) = sum(k=1, n, (-1)^(k+1)*(n\(2*k-1))^2);
for(n=1, M, print1(a(n), ", "));