a(n) = sum(k=0, n\7, ((n-3*k)%4==0)*binomial((n-3*k)/4, k));
for(n=0, 75, print1(a(n), ", "));
