M=17;

a007559(n) = prod(k=0, n-1, 3*k+1);
for(n=0, M, print1(a007559(n), ", "));

\\ a(n) = Sum_{k=0..n} A007559(k+1) * |Stirling1(n,k)|.
a(n) = sum(k=0, n, a007559(k+1) * abs(stirling(n,k,1)));
for(n=0, M, print1(a(n), ", "));
