\\ a(n,0,l) = 0^(n-1) and a(n,k,l) = a(n,k-1,l) + Sum_{j=1..n-1} a(j,k+l-1,l) * a(n-j,k-1,l) for k > 0.
a(n,k,l) = if(k==0, 0^(n-1), a(n,k-1,l) + sum(j=1, n-1, a(j,k+l-1,l) * a(n-j,k-1,l)));

matrix(6, 7, n, k, a(n, k-1, 5))

for(n=1, 8, print1(a(n,1,5),", "));

