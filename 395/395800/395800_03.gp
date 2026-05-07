\\ a(n) = (1/n) * (8*binomial(5*n-5,n-1) - 12*binomial(5*n-5,n-2)) for n > 0.
a(n) = if(n==0, -1, (1/n) * (8*binomial(5*n-5,n-1) - 12*binomial(5*n-5,n-2)));
for(n=0, 21, print1(a(n), ", "));

\\ a(n) = (1/n) * (20*binomial(5*n-5,n-1) - 12*binomial(5*n-4,n-1)) for n > 0.
b(n) = if(n==0, -1, (1/n) * (20*binomial(5*n-5,n-1) - 12*binomial(5*n-4,n-1)));
for(n=0, 50, print1(a(n)-b(n), ", "));