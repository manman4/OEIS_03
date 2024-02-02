a017847(n) = sum(k=0, n\6, binomial(k, n-6*k));
for(n=0, 50, print1(a017847(n), ", "));
\\ a(n) = A017847(7*n-6)
a(n) = if(n==0, 1, a017847(7*n-6));
for(n=0, 35, print1(a(n), ", "));