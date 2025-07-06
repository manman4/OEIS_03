M=12;

\\ a(0) = 1; a(n) = Sum_{i, j, k>=0 and i+j+k=n-1} (2^j+1) * (2^k+1) * a(i) * a(j) * a(k).
a(n) = if(n==0, 1, sum(i=0, n-1, sum(j=0, n-1-i, (2^j+1) * (2^(n-1-i-j)+1) * a(i) * a(j) * a(n-1-i-j))));
for(n=0, M, print1(a(n),", "));

