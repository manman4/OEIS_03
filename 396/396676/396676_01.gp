\\ A(n,0) = (n-1)!; A(n,k) = Sum_{j=1..n} n^(n-j) * binomial(n-1,j-1) * A(j,k-1) for k > 0.
a(n, k) = if(k==0, (n-1)!, sum(j=1, n, n^(n-j)*binomial(n-1, j-1)*a(j, k-1)));
for(n=1, 10, for(k=1, n, print1(a(k,n-k),", ")));
