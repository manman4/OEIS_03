a(n) = sum(k=1, n, (-1)^(k-1) * binomial(k+1,2) * (n\k) );
for(n=1, 52, print1(a(n), ", "));
