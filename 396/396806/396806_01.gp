a(n, m, l) = if (n == 1, 1, 1/(n-1) * sum(j=1, n-1, binomial(n,j) * j * a(n-j, m, l) * sum(i=1, m, a(j, l+i-1, l))) );

for(n=1, 10, print1(a(n, 1, 6), ", "));
