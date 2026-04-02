M=20;

c(n) = my(v=[1, 1, 1, 1]); if(n<4, v[n+1], c(n-1) + (n-3)*c(n-4));
for(n=0, 15, print1(c(n), ", "));
\\ a(n) = A394800(n) - Sum_{k=0..n-1} a(k) * A394800(n-1-k).
a(n) = c(n) - sum(k=0, n-1, a(k) * c(n-1-k));
for(n=0, 15, print1(a(n),", "));  
