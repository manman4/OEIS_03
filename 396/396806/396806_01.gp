a(n, k, l) = if (n==1, 1, 1/(n-1) * sum(i=1, n-1, i * binomial(n,i) * a(n-i, k, l) * sum(j=1, k, a(i, l+j-1, l))) );

for(n=1, 9, print1(a(n, 1, 6), ", "));
