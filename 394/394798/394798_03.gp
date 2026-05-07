\\ a(n) = (1/n) * (4*binomial(3*n-3,n-1) - 2*binomial(3*n-3,n-2)) for n > 0.
a(n) = if(n==0, -1, (1/n) * (4*binomial(3*n-3,n-1) - 2*binomial(3*n-3,n-2)));
for(n=0, 24, print1(a(n), ", "));

\\ a(n) = (1/n) * (6*binomial(3*n-3,n-1) - 2*binomial(3*n-2,n-1)) for n > 0.
b(n) = if(n==0, -1, (1/n) * (6*binomial(3*n-3,n-1) - 2*binomial(3*n-2,n-1)));
for(n=0, 50, print1(a(n)-b(n), ", "));