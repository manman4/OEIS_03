M=20;

b(n, m) = if(n==0, 1, sum(k=1, n, (1+(k==m)*m!)*binomial(n, k)*b(n-k, m)));
a(n) = b(n, 2);
for(n=0, M, print1(a(n), ", "));