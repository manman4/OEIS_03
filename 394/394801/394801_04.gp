M=20;

c(n) = my(v=[1, 1, 1]); if(n<3, v[n+1], c(n-1) + (n-2)*c(n-3));
for(n=0, 15, print1(c(n), ", "));
\\ a(n) = A394799(n) - Sum_{k=0..n-1} a(k) * A394799(n-1-k).
a(n) = c(n) - sum(k=0, n-1, a(k) * c(n-1-k));
for(n=0, 15, print1(a(n),", "));  
