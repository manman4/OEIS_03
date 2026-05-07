\\ a(n) = (1/n) * (7*binomial(5*n-4,n-1) - 8*binomial(5*n-4,n-2)) for n > 0.
a(n) = if(n==0, -1, (1/n) * (7*binomial(5*n-4,n-1) - 8*binomial(5*n-4,n-2)));
for(n=0, 22, print1(a(n), ", "));

\\ a(n) = (1/n) * (15*binomial(5*n-4,n-1) - 8*binomial(5*n-3,n-1)) for n > 0.
b(n) = if(n==0, -1, (1/n) * (15*binomial(5*n-4,n-1) - 8*binomial(5*n-3,n-1)));
for(n=0, 50, print1(a(n)-b(n), ", "));