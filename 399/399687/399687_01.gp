\\ T(n,k) = binomial(k-1,2)*floor(n/(k-1)) + binomial(n mod (k-1),2) + ceiling(n/(k-1)) - 1.
T(n, k) = binomial(k-1, 2)*(n\(k-1))+binomial(n%(k-1), 2)+ceil(n/(k-1))-1;

for(n=3, 14, for(k=3, n, print1(T(n,k),", ")));
