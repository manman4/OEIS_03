\\ a(n) = (1/n) * (6*binomial(5*n-3,n-1) - 4*binomial(5*n-3,n-2)) for n > 0.
a(n) = if(n==0, -1, (1/n) * (6*binomial(5*n-3,n-1) - 4*binomial(5*n-3,n-2)));
for(n=0, 21, print1(a(n), ", "));

\\ a(n) = (1/n) * (10*binomial(5*n-3,n-1) - 4*binomial(5*n-2,n-1)) for n > 0.
b(n) = if(n==0, -1, (1/n) * (10*binomial(5*n-3,n-1) - 4*binomial(5*n-2,n-1)));
for(n=0, 50, print1(a(n)-b(n), ", "));