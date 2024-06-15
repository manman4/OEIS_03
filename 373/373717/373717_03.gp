\\ T(n,k) = Sum_{j=0..k} binomial(k,j) * T(n-2*j-1,k).
S(n, k) = sum(j=0, k*n\(2*k+1), binomial(k*(n-2*j), j));
T(n, k) = if(n<2*k-1, S(n, k), sum(j=0, k, binomial(k, j) * T(n-2*j-1, k)));
for(n=0, 12, for(k=0, n, print1(T(k, n-k),", "))) 
