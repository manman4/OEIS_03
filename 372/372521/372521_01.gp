

b(n, m) = if(n<m, 0, if(n==m, 1, 1/5 * (5^(n-m) * sum(l=0, m, binomial(l,n-3*m+2*l) * binomial(m,l)) - sum(l=m+1, n-1, (b(n, l) + sum(k=l, n, (b(n, k) + sum(j=k, n, (b(n, j) + sum(i=j, n, b(n, i) * b(i, j))) * b(j, k))) * b(k, l))) * b(l, m)))));
a(n) = b(n, 1);
for(n=0, 9, print1(a(n), ", "))
