\\ a(n) = (1/n) * (6*binomial(4*n-4,n-1) - 6*binomial(4*n-4,n-2)) for n > 0.
a(n) = if(n==0, -1, (1/n) * (6*binomial(4*n-4,n-1) - 6*binomial(4*n-4,n-2)));
for(n=0, 22, print1(a(n), ", "));

\\ a(n) = (1/n) * (12*binomial(4*n-4,n-1) - 6*binomial(4*n-3,n-1)) for n > 0.
b(n) = if(n==0, -1, (1/n) * (12*binomial(4*n-4,n-1) - 6*binomial(4*n-3,n-1)));
for(n=0, 50, print1(a(n)-b(n), ", "));