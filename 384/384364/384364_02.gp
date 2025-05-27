\\ A(n,k) = (1/3) * Sum_{j>=1} (3/4)^(j+n) * binomial(j+n-1,n)^k for k > 0.
a(n, k) = (1/3) * sum(j=1, 1000, (3/4)^(j+n) * binomial(j+n-1, n)^k)+ 0.;
for(n=0, 8, for(k=0, n, print1(a(k,n-k), ", ")));
matrix(4, 4, n, k, a(n-1, k-1))
print("k=0の時がダメ");
a(n, k) = if(k==0, 1, round((1/3) * sum(j=1, 1000, (3/4)^(j+n) * binomial(j+n-1, n)^k)+ 0.));
for(n=0, 8, for(k=0, n, print1(a(k,n-k), ", ")));

