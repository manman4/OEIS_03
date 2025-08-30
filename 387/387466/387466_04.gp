\\ A(n,k) = Sum_{j=0..floor(n/2)} k^(2*j) * binomial(2*(n-j),n-j) * binomial(n-j,j).
a(n,k) = sum(j=0, n\2, k^(2*j) * binomial(2*(n-j),n-j) * binomial(n-j,j));
for(n=0, 9, for(k=0, n, print1(a(k,n-k),", ")));;
