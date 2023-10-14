M=12;

a(n) = polcoef((1-x)^(n-1)/((1-x)^n+x^(n+1)+x*O(x^n^2)), n^2);
for(n=0, M, print1(a(n), ", "));