T(n, k) = length(digits(binomial(n, k)));
for(n=0, 15, for(k=0, n, print1(T(n, k), ", ")));