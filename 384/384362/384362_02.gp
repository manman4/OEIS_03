\\ A(n,k) = (1/2) * Sum_{j>=1} (2/3)^(j+n) * binomial(j+n-1,n)^k for k > 0.
a(n, k) = (1/2) * sum(j=1, 1000, (2/3)^(j+n) * binomial(j+n-1, n)^k)+ 0.;
for(n=0, 8, for(k=0, n, print1(a(k,n-k), ", ")));
print("k=0の時がダメ");
a(n, k) = if(k==0, 1, round((1/2) * sum(j=1, 1000, (2/3)^(j+n) * binomial(j+n-1, n)^k)+ 0.));
for(n=0, 8, for(k=0, n, print1(a(k,n-k), ", ")));

