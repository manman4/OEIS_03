\\ A(n,k) = (A340968(n+1,k) - A340968(n,k))/k for k > 0.
a340968(n,k) = sum(j=0, n, k^j*binomial(n, j)*(2*j)!/(j!*(j+1)!));
a(n, k) = if(k==0, 1, (a340968(n+1,k) - a340968(n,k))/k);
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));
