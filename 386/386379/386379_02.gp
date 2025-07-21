M=30;

\\ a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/5)} a(5*k) * a(n-1-5*k).
a(n) = if(n==0, 1, sum(k=0, (n-1)\5, a(5*k) * a(n-1-5*k)));
for(n=0, M, print1(a(n),", "));