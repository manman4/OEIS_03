a(n, k, l) = if(n==1, 1, 1/(n-1) * sum(i=1, n-1, i * binomial(n,i) * a(n-i, k, l) * sum(j=1, k, a(i, l+j-1, l))) );

for(n=1, 8, print1(a(n, 1, 0), ", "));
for(n=1, 8, print1(a(n, 1, 1), ", "));
for(n=1, 8, print1(a(n, 1, 2), ", "));
for(n=1, 8, print1(a(n, 1, 3), ", "));
for(n=1, 8, print1(a(n, 1, 4), ", "));
for(n=1, 8, print1(a(n, 1, 5), ", "));
for(n=1, 8, print1(a(n, 1, 6), ", "));
