M=23;

b(n, m) = if(n==0, 1, sum(k=1, n, (-1+(k==m))*binomial(n, k)*b(n-k, m)));
a(n) = b(n, 4);
for(n=0, M, print1(a(n), ", "));