a(n, m, k) = if (n == 1, 1, 1/(n-1) * sum(j=1, n-1, binomial(n,j) * j * a(n-j, m, k) * sum(i=1, m, a(j, k+i-1, k))) );

for(n=1, 10, print1(a(n, 1, 6), ", "));
