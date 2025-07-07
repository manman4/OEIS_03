M=15;

\\ a(0) = 1; a(n) = Sum_{i, j, k>=0 and i+2*j+3*k=n-1} a(i) * a(2*j) * a(3*k).
a(n) = if(n==0, 1, sum(k=0, (n-1)\3, sum(j=0, (n-1-3*k)\2, a(n-1-3*k-2*j) * a(2*j) * a(3*k))));
for(n=0, M, print1(a(n),", "));