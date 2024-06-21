a000217(n) = n*(n+1)/2;
a024916(n) = {if(n==0, 0, m = round(sqrt(n));
 sum(i=1, n\m, n\i*i) + sum(j=1, m-1, j*(a000217(n\j)-a000217(n\(j+1))));
)}
for(n=1, 100, print1(a024916(n), ", "))

b(n) = 1+8*a024916(n)-32*a024916(n\4);
a(n) = b(10^n);
for(n=0, 13, print1(a(n), ", "))

