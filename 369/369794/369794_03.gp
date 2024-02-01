\\ Sum_{k=0..floor(n/5)} binomial(k,n-5*k)
a017837(n) = sum(k=0, n\5, binomial(k, n-5*k));
for(n=0, 50, print1(a017837(n), ", "));
\\ a(n) = A017837(6*n-5) for n>0.
a(n) = if(n==0, 1, a017837(6*n-5));
for(n=0, 34, print1(a(n), ", "));