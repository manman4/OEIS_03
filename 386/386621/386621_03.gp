\\ A(n,k) = Sum_{j=0..n} (k-i)^j * (k+i)^(n-j) * binomial(n,j)^2, where i is the imaginary unit.
a(n,k) = sum(j=0, n, (k-I)^j * (k+I)^(n-j) * binomial(n,j)^2);
for(n=0, 10, for(k=0, n, print1(a(k,n-k),", ")));

