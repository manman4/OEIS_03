\\ a(n) = binomial(n+2,3)*a(n-1) + 1.
a(n) = if(n==0, 1, binomial(n+2,3)*a(n-1) + 1);
for(n=0, 20, print1(a(n), ", "));
